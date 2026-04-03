// Copyright 2026 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ROSIDL_TYPESUPPORT_FASTRTPS_CPP__BUFFER_SERIALIZATION_HPP_
#define ROSIDL_TYPESUPPORT_FASTRTPS_CPP__BUFFER_SERIALIZATION_HPP_

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "rosidl_buffer/buffer.hpp"
#include "rosidl_buffer_backend/buffer_backend.hpp"
#include "rosidl_buffer_backend/buffer_descriptor_ops.hpp"
#include "rosidl_runtime_c/primitives_sequence.h"
#include "rosidl_runtime_c/primitives_sequence_functions.h"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_fastrtps_cpp/visibility_control.h"
#include "fastcdr/Cdr.h"
#include "rmw/topic_endpoint_info.h"
#include "rcutils/logging_macros.h"

namespace rosidl_typesupport_fastrtps_cpp
{

/// Forward declaration — BufferDescriptorSerializers and BufferSerializationContext are
/// intentionally mutually dependent:
///
///   BufferDescriptorSerializers  references  BufferSerializationContext  (by const-ref
///                                             in its std::function signatures)
///   BufferSerializationContext   contains     BufferDescriptorSerializers (by value)
///
/// This circularity exists because descriptor messages themselves may contain Buffer<T>
/// fields (e.g. uint8[] data in a descriptor .msg).  When such a field is backed by a
/// non-CPU backend, the generated cdr_serialize_with_endpoint for the descriptor message
/// calls serialize_buffer_with_endpoint, which needs the full context to look up the
/// inner backend's ops and serializers.
struct BufferSerializationContext;

/// FastCDR-specific descriptor serialization functions (technology-specific).
struct BufferDescriptorSerializers
{
  std::function<void(eprosima::fastcdr::Cdr &, const std::shared_ptr<void> &,
    const rmw_topic_endpoint_info_t &, const BufferSerializationContext &)> serialize;
  std::function<std::shared_ptr<void>(eprosima::fastcdr::Cdr &,
    const rmw_topic_endpoint_info_t &, const BufferSerializationContext &)> deserialize;
};

/// RMW-owned descriptor context passed through endpoint-aware callbacks.
struct BufferSerializationContext
{
  std::unordered_map<std::string, rosidl::BufferDescriptorOps> descriptor_ops;
  std::unordered_map<std::string, BufferDescriptorSerializers> descriptor_serializers;
};

/// Two-word magic marker for descriptor-backed Buffer payloads.
/// CPU vector path: first uint32 is the sequence length (doesn't match marker pair).
/// Descriptor path: first two uint32s == kBufferDescriptorMarker1 + kBufferDescriptorMarker2,
/// followed by backend_type string and the serialized descriptor.
inline constexpr uint32_t kBufferDescriptorMarker1 = 0xFFFFFFFFu;
inline constexpr uint32_t kBufferDescriptorMarker2 = 0x524F5332u;  // "ROS2" in ASCII

/// Get serialized size of Buffer<T> - for use by generated type support code
template<typename T, typename Allocator>
inline size_t get_buffer_serialized_size(
  const rosidl::Buffer<T, Allocator> & buffer,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;
  const size_t padding = 4;

  const std::string backend_type = buffer.get_backend_type();

  // CPU-based wire estimate — always computed because non-CPU backends
  // may fall back to this format at serialization time.
  size_t cpu_alignment = current_alignment;
  {
    size_t array_size = buffer.size();
    cpu_alignment += eprosima::fastcdr::Cdr::alignment(cpu_alignment, padding);
    cpu_alignment += padding;
    if (array_size > 0) {
      size_t item_size = sizeof(T);
      cpu_alignment += eprosima::fastcdr::Cdr::alignment(cpu_alignment, item_size);
      cpu_alignment += array_size * item_size;
    }
  }

  if (backend_type == "cpu") {
    current_alignment = cpu_alignment;
  } else {
    // Descriptor estimate: marker pair + backend_type string + descriptor payload.
    size_t descriptor_alignment = current_alignment;
    descriptor_alignment += eprosima::fastcdr::Cdr::alignment(descriptor_alignment, padding);
    descriptor_alignment += padding;  // kBufferDescriptorMarker1
    descriptor_alignment += padding;  // kBufferDescriptorMarker2
    descriptor_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(descriptor_alignment, padding) +
      backend_type.size() + 1;
    descriptor_alignment += rosidl::kMaxBufferDescriptorSize;

    // Take the max: serialization may use the descriptor path or fall back to CPU.
    current_alignment = std::max(descriptor_alignment, cpu_alignment);
  }

  return current_alignment - initial_alignment;
}

/// Serialize Buffer<T> with endpoint awareness.
/// Calls endpoint-specific descriptor creation for optimization.
/// If the backend returns nullptr from create_descriptor_with_endpoint(), the buffer
/// is serialized as std::vector<T> (CPU fallback) for legacy wire compatibility.
template<typename T, typename Allocator>
inline void serialize_buffer_with_endpoint(
  eprosima::fastcdr::Cdr & cdr,
  const rosidl::Buffer<T, Allocator> & buffer,
  const rmw_topic_endpoint_info_t & endpoint_info,
  const BufferSerializationContext & serialization_context)
{
  const std::string backend_type = buffer.get_backend_type();

  RCUTILS_LOG_INFO_NAMED("serialize_buffer_with_endpoint",
    ("Serializing buffer (backend: " + backend_type + ")").c_str());

  if (backend_type == "cpu") {
    RCUTILS_LOG_INFO_NAMED("serialize_buffer_with_endpoint", "Serializing buffer as std::vector");
    const std::vector<T, Allocator> & vec = buffer;
    cdr << vec;
    return;
  }

  const auto * impl = buffer.get_impl();
  if (!impl) {
    throw std::runtime_error("Buffer implementation is null");
  }

  auto ops_it = serialization_context.descriptor_ops.find(backend_type);
  auto ser_it = serialization_context.descriptor_serializers.find(backend_type);
  if (ops_it == serialization_context.descriptor_ops.end() ||
    ser_it == serialization_context.descriptor_serializers.end())
  {
    RCUTILS_LOG_WARN_NAMED(
      "serialize_buffer_with_endpoint",
      "Backend '%s' not available (shutdown?), falling back to CPU wire format",
      backend_type.c_str());
    std::vector<T, Allocator> vec = buffer.to_vector();
    cdr << vec;
    return;
  }

  auto descriptor = ops_it->second.create_descriptor_with_endpoint(impl, endpoint_info);

  // nullptr means the backend cannot handle this endpoint — fall back to CPU wire format.
  if (!descriptor) {
    RCUTILS_LOG_INFO_NAMED(
      "serialize_buffer_with_endpoint", "Backend returned null descriptor, falling back to CPU");
    std::vector<T, Allocator> vec = buffer.to_vector();
    cdr << vec;
    return;
  }

  // Two-word magic marker for descriptor-backed payload.
  cdr << static_cast<uint32_t>(kBufferDescriptorMarker1);
  cdr << static_cast<uint32_t>(kBufferDescriptorMarker2);
  cdr << backend_type;

  RCUTILS_LOG_INFO_NAMED("serialize_buffer_with_endpoint",
    ("Serializing descriptor for backend: " + backend_type).c_str());

  ser_it->second.serialize(cdr, descriptor, endpoint_info, serialization_context);
}

/// Deserialize Buffer<T> with endpoint awareness.
/// Returns true on success, false if deserialization could not be completed
/// (e.g. backend unavailable after shutdown).
template<typename T, typename Allocator>
inline bool deserialize_buffer_with_endpoint(
  eprosima::fastcdr::Cdr & cdr,
  rosidl::Buffer<T, Allocator> & buffer,
  const rmw_topic_endpoint_info_t & endpoint_info,
  const BufferSerializationContext & serialization_context)
{
  RCUTILS_LOG_INFO_NAMED("deserialize_buffer_with_endpoint", "Starting buffer deserialization");

  // Peek to disambiguate CPU vector bytes vs descriptor payload.
  // Only read the second word when the first matches — an empty vector (uint32 length = 0)
  // may leave fewer than 8 bytes in the CDR buffer, so reading two words unconditionally
  // would overread and throw.
  auto original_state = cdr.get_state();
  uint32_t first_word = 0u;
  cdr >> first_word;
  bool is_descriptor_path = false;
  if (first_word == kBufferDescriptorMarker1) {
    uint32_t second_word = 0u;
    cdr >> second_word;
    is_descriptor_path = (second_word == kBufferDescriptorMarker2);
  }

  if (!is_descriptor_path) {
    RCUTILS_LOG_INFO_NAMED(
      "deserialize_buffer_with_endpoint", "Legacy vector path: deserializing std::vector");
    cdr.set_state(original_state);
    std::vector<T, Allocator> & storage = buffer;
    cdr >> storage;
    return true;
  }

  std::string backend_type;
  cdr >> backend_type;
  RCUTILS_LOG_INFO_NAMED("deserialize_buffer_with_endpoint",
    (backend_type + " backend: deserializing descriptor").c_str());

  auto ops_it = serialization_context.descriptor_ops.find(backend_type);
  auto ser_it = serialization_context.descriptor_serializers.find(backend_type);
  if (ops_it == serialization_context.descriptor_ops.end() ||
    ser_it == serialization_context.descriptor_serializers.end())
  {
    RCUTILS_LOG_ERROR_NAMED(
      "deserialize_buffer_with_endpoint",
      "Backend '%s' not available (shutdown?), cannot deserialize descriptor payload",
      backend_type.c_str());
    return false;
  }

  // Deserialize descriptor
  RCUTILS_LOG_INFO_NAMED("deserialize_buffer_with_endpoint", "Deserializing descriptor");
  auto descriptor = ser_it->second.deserialize(cdr, endpoint_info, serialization_context);

  // Create buffer implementation with endpoint awareness
  RCUTILS_LOG_INFO_NAMED("deserialize_buffer_with_endpoint", "Creating buffer from descriptor");
  auto impl_erased = ops_it->second.from_descriptor_with_endpoint(descriptor.get(), endpoint_info);

  std::unique_ptr<rosidl::BufferImplBase<T>> typed_impl(
    static_cast<rosidl::BufferImplBase<T> *>(impl_erased.release()));
  buffer = rosidl::Buffer<T, Allocator>(std::move(typed_impl));
  return true;
}

/// Serialize a C uint8 sequence that may hold either a plain data array or
/// a rosidl::Buffer<uint8_t>* (indicated by the is_rosidl_buffer flag).
/// Plain sequences are serialized as legacy uint8[] wire format (uint32 size + raw bytes).
/// Buffer-backed sequences delegate to serialize_buffer_with_endpoint().
inline void serialize_buffer_or_c_sequence_with_endpoint(
  eprosima::fastcdr::Cdr & cdr,
  const rosidl_runtime_c__uint8__Sequence & seq,
  const rmw_topic_endpoint_info_t & endpoint_info,
  const BufferSerializationContext & serialization_context)
{
  if (seq.is_rosidl_buffer) {
    auto * buffer = reinterpret_cast<const rosidl::Buffer<uint8_t> *>(seq.data);
    serialize_buffer_with_endpoint(cdr, *buffer, endpoint_info, serialization_context);
  } else {
    cdr << static_cast<uint32_t>(seq.size);
    if (seq.size > 0) {
      cdr.serialize_array(seq.data, seq.size);
    }
  }
}

/// Deserialize into a C uint8 sequence, handling both legacy wire format and
/// descriptor-backed Buffer payloads.
/// Legacy path: deserializes directly into the C sequence (no intermediate Buffer).
/// Descriptor path: creates a temporary rosidl::Buffer<uint8_t> via
/// deserialize_buffer_with_endpoint(), then either stashes the Buffer* in the
/// sequence (non-CPU backend) or copies the data out to a plain C sequence (CPU).
inline bool deserialize_buffer_or_c_sequence_with_endpoint(
  eprosima::fastcdr::Cdr & cdr,
  rosidl_runtime_c__uint8__Sequence & seq,
  const rmw_topic_endpoint_info_t & endpoint_info,
  const BufferSerializationContext & serialization_context)
{
  auto original_state = cdr.get_state();
  uint32_t first_word = 0u;
  cdr >> first_word;
  bool is_descriptor_path = false;
  if (first_word == kBufferDescriptorMarker1) {
    uint32_t second_word = 0u;
    cdr >> second_word;
    is_descriptor_path = (second_word == kBufferDescriptorMarker2);
  }
  cdr.set_state(original_state);

  if (!is_descriptor_path) {
    uint32_t seq_size = 0u;
    cdr >> seq_size;
    if (seq.data) {
      rosidl_runtime_c__uint8__Sequence__fini(&seq);
    }
    if (!rosidl_runtime_c__uint8__Sequence__init(&seq, seq_size)) {
      RCUTILS_LOG_ERROR_NAMED(
        "deserialize_buffer_or_c_sequence_with_endpoint",
        "Failed to init uint8 sequence (size %u)", seq_size);
      return false;
    }
    if (seq_size > 0) {
      cdr.deserialize_array(seq.data, seq_size);
    }
    seq.is_rosidl_buffer = false;
    return true;
  }

  auto buffer = std::make_unique<rosidl::Buffer<uint8_t>>();
  if (!deserialize_buffer_with_endpoint(cdr, *buffer, endpoint_info, serialization_context)) {
    return false;
  }

  if (seq.data) {
    rosidl_runtime_c__uint8__Sequence__fini(&seq);
  }
  seq.size = buffer->size();
  seq.data = reinterpret_cast<uint8_t *>(buffer.release());
  seq.capacity = 0;
  seq.is_rosidl_buffer = true;
  seq.owns_rosidl_buffer = true;

  return true;
}

}  // namespace rosidl_typesupport_fastrtps_cpp

namespace eprosima
{
namespace fastcdr
{

/// FastCDR serialize() function for Buffer<T> (called by FastCDR internally)
template<typename T, typename Allocator>
inline void serialize(Cdr & cdr, const rosidl::Buffer<T, Allocator> & buffer)
{
  cdr << buffer;  // Delegate to our custom operator<<
}

/// FastCDR deserialize() function for Buffer<T> (called by FastCDR internally)
template<typename T, typename Allocator>
inline void deserialize(Cdr & cdr, rosidl::Buffer<T, Allocator> & buffer)
{
  cdr >> buffer;  // Delegate to our custom operator>>
}

/// Serialize Buffer<T>.
/// CPU backend: serializes directly via zero-copy reference to underlying storage.
/// Other backends: force-convert to CPU backend and serialize as std::vector<T>.
template<typename T, typename Allocator>
inline Cdr & operator<<(Cdr & cdr, const rosidl::Buffer<T, Allocator> & buffer)
{
  const std::string backend_type = buffer.get_backend_type();
  if (backend_type == "cpu") {
    const std::vector<T, Allocator> & vec = buffer;
    cdr << vec;
  } else {
    RCUTILS_LOG_INFO_NAMED("Serialize Buffer<T>",
      ("Force-converting to CPU buffer for serialization (backend: " + backend_type + ")").c_str());
    std::vector<T, Allocator> vec = buffer.to_vector();
    cdr << vec;
  }
  return cdr;
}

/// Deserialize Buffer<T>.
/// CPU backend: deserializes directly from std::vector<T> (fully backward compatible)
/// Other backends: use descriptor message approach
template<typename T, typename Allocator>
inline Cdr & operator>>(Cdr & cdr, rosidl::Buffer<T, Allocator> & buffer)
{
  // Only supports legacy vector-compatible CPU path.
  auto original_state = cdr.get_state();
  uint32_t first_word = 0u;
  cdr >> first_word;
  bool is_descriptor_path = false;
  if (first_word == rosidl_typesupport_fastrtps_cpp::kBufferDescriptorMarker1) {
    uint32_t second_word = 0u;
    cdr >> second_word;
    is_descriptor_path = (second_word == rosidl_typesupport_fastrtps_cpp::kBufferDescriptorMarker2);
  }
  cdr.set_state(original_state);
  if (is_descriptor_path) {
    throw std::runtime_error(
            "Deserializing Buffer<T> with operator>> only supports CPU vector bytes");
  }

  // Buffer defaults to CPU backend — deserialize directly into its underlying storage.
  std::vector<T, Allocator> & storage = buffer;
  cdr >> storage;
  return cdr;
}

}  // namespace fastcdr
}  // namespace eprosima

#endif  // ROSIDL_TYPESUPPORT_FASTRTPS_CPP__BUFFER_SERIALIZATION_HPP_
