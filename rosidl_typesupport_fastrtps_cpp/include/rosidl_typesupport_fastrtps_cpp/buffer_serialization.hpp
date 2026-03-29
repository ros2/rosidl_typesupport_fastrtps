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

#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "rosidl_buffer/buffer.hpp"
#include "rosidl_buffer_backend/buffer_descriptor_ops.hpp"
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

/// Marker for descriptor-backed Buffer payloads.
/// CPU/legacy vector path: first uint32 is the sequence length (any value != marker).
/// Descriptor path: first uint32 == kBufferDescriptorMarker, followed by backend_type
/// string and the serialized descriptor.
inline constexpr uint32_t kBufferDescriptorMarker = 0xFFFFFFFFu;

/// Get serialized size of Buffer<T> - for use by generated type support code
template<typename T, typename Allocator>
inline size_t get_buffer_serialized_size(
  const rosidl::Buffer<T, Allocator> & buffer,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;
  const size_t padding = 4;

  const std::string backend_type = buffer.get_backend_type();

  if (backend_type == "cpu") {
    // CPU path is wire-compatible with std::vector<T>:
    // uint32 length + element bytes.
    size_t array_size = buffer.size();

    // Align to 4-byte boundary for the length field
    current_alignment += eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    // Add 4 bytes for the array length
    current_alignment += padding;

    // Add array elements
    if (array_size > 0) {
      size_t item_size = sizeof(T);
      // Elements might need alignment
      current_alignment += eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
      current_alignment += array_size * item_size;
    }
  } else {
    // Descriptor marker prefix.
    current_alignment += eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    current_alignment += padding;

    // backend_type string
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
      backend_type.size() + 1;  // +1 for null terminator

    // Vendor backends: account for descriptor payload
    // Conservative estimate: buffer data size + overhead for metadata fields
    size_t buffer_data_size = buffer.size() * sizeof(T);
    size_t metadata_overhead = 256;
    current_alignment += buffer_data_size + metadata_overhead;
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
    std::vector<T> vec = buffer.to_vector();
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
    std::vector<T> vec = buffer.to_vector();
    cdr << vec;
    return;
  }

  auto * non_const_impl = const_cast<rosidl::BufferImplBase<T> *>(impl);
  std::shared_ptr<void> impl_shared(static_cast<void *>(non_const_impl), [](void *){});

  auto descriptor = ops_it->second.create_descriptor_with_endpoint(impl_shared, endpoint_info);

  // nullptr means the backend cannot handle this endpoint — fall back to CPU wire format.
  if (!descriptor) {
    RCUTILS_LOG_INFO_NAMED(
      "serialize_buffer_with_endpoint", "Backend returned null descriptor, falling back to CPU");
    std::vector<T> vec = buffer.to_vector();
    cdr << vec;
    return;
  }

  // Descriptor-backed payload marker in first uint32.
  cdr << static_cast<uint32_t>(kBufferDescriptorMarker);
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

  // Peek first uint32 to disambiguate legacy vector bytes vs descriptor payload.
  auto original_state = cdr.get_state();
  uint32_t first_word = 0u;
  cdr >> first_word;
  cdr.set_state(original_state);

  // Legacy/vector path: first word is a sequence length (any value != marker).
  if (first_word != kBufferDescriptorMarker) {
    RCUTILS_LOG_INFO_NAMED(
      "deserialize_buffer_with_endpoint", "Legacy vector path: deserializing std::vector");
    std::vector<T> vec;
    cdr >> vec;

    buffer.resize(vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
      buffer[i] = vec[i];
    }
    return true;
  }

  // Descriptor path: consume the marker.
  cdr >> first_word;

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
  auto impl_shared = ops_it->second.from_descriptor_with_endpoint(descriptor, endpoint_info);

  auto typed_impl_shared =
    std::static_pointer_cast<rosidl::BufferImplBase<T>>(impl_shared);
  std::unique_ptr<rosidl::BufferImplBase<T>> typed_impl_unique =
    typed_impl_shared->clone();
  buffer = rosidl::Buffer<T, Allocator>(std::move(typed_impl_unique));
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
/// CPU backend: serializes directly as std::vector<T>
/// Other backends: force-convert to CPU backend and serialize as std::vector<T>
template<typename T, typename Allocator>
inline Cdr & operator<<(Cdr & cdr, const rosidl::Buffer<T, Allocator> & buffer)
{
  const std::string backend_type = buffer.get_backend_type();
  if (backend_type != "cpu") {
    RCUTILS_LOG_INFO_NAMED("Serialize Buffer<T>",
      ("Force-converting to CPU buffer for serialization (backend: " + backend_type + ")").c_str());
  }

  // Serialize as std::vector<T> for legacy wire compatibility.
  const std::vector<T> & vec = buffer.to_vector();
  cdr << vec;
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
  cdr.set_state(original_state);
  if (first_word == rosidl_typesupport_fastrtps_cpp::kBufferDescriptorMarker) {
    throw std::runtime_error(
            "Deserializing Buffer<T> with operator>> only supports legacy CPU vector bytes");
  }

  std::vector<T> vec;
  cdr >> vec;

  // Copy into buffer (which defaults to CPU backend)
  buffer.resize(vec.size());
  for (size_t i = 0; i < vec.size(); ++i) {
    buffer[i] = vec[i];
  }
  return cdr;
}

}  // namespace fastcdr
}  // namespace eprosima

#endif  // ROSIDL_TYPESUPPORT_FASTRTPS_CPP__BUFFER_SERIALIZATION_HPP_
