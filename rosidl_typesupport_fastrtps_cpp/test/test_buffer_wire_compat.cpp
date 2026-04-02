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

#include <gtest/gtest.h>

#include <array>
#include <functional>
#include <string>
#include <vector>

#include "fastcdr/Cdr.h"
#include "fastcdr/FastBuffer.h"
#include "rosidl_buffer/buffer.hpp"
#include "rmw/topic_endpoint_info.h"
#include "rosidl_typesupport_fastrtps_cpp/buffer_serialization.hpp"

namespace
{

std::vector<uint8_t> serialize_to_bytes(const std::function<void(eprosima::fastcdr::Cdr &)> & fn)
{
  std::array<char, 4096> raw{};
  eprosima::fastcdr::FastBuffer fast_buffer(raw.data(), raw.size());
  eprosima::fastcdr::Cdr cdr(fast_buffer);
  fn(cdr);

  const auto serialized_len = cdr.get_serialized_data_length();
  return std::vector<uint8_t>(
    reinterpret_cast<uint8_t *>(raw.data()),
    reinterpret_cast<uint8_t *>(raw.data()) + serialized_len);
}

}  // namespace

TEST(BufferWireCompat, CpuBufferSerializationMatchesLegacyVectorBytes)
{
  const std::vector<uint8_t> payload{1, 2, 3, 4, 5, 6, 7, 8};

  rosidl::Buffer<uint8_t> buffer;
  buffer.resize(payload.size());
  for (size_t i = 0; i < payload.size(); ++i) {
    buffer[i] = payload[i];
  }

  const auto endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();
  rosidl_typesupport_fastrtps_cpp::BufferSerializationContext serialization_context;

  const auto buffer_bytes = serialize_to_bytes(
    [&](eprosima::fastcdr::Cdr & cdr) {
      rosidl_typesupport_fastrtps_cpp::serialize_buffer_with_endpoint(
        cdr, buffer, endpoint_info, serialization_context);
    });

  const auto vector_bytes = serialize_to_bytes(
    [&](eprosima::fastcdr::Cdr & cdr) {
      cdr << payload;
    });

  EXPECT_EQ(buffer_bytes, vector_bytes);
}

TEST(BufferWireCompat, DeserializeLegacyVectorBytesIntoCpuBuffer)
{
  const std::vector<uint8_t> payload{11, 22, 33, 44, 55};
  auto bytes = serialize_to_bytes(
    [&](eprosima::fastcdr::Cdr & cdr) {
      cdr << payload;
    });

  eprosima::fastcdr::FastBuffer fast_buffer(
    reinterpret_cast<char *>(bytes.data()), bytes.size());
  eprosima::fastcdr::Cdr cdr(fast_buffer);
  rosidl::Buffer<uint8_t> output;
  const auto endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();
  rosidl_typesupport_fastrtps_cpp::BufferSerializationContext serialization_context;

  rosidl_typesupport_fastrtps_cpp::deserialize_buffer_with_endpoint(
    cdr, output, endpoint_info, serialization_context);

  EXPECT_EQ(output.get_backend_type(), "cpu");
  EXPECT_EQ(output.to_vector(), payload);
}

TEST(BufferWireCompat, DescriptorMarkerIsNotInterpretedAsLegacyVector)
{
  auto bytes = serialize_to_bytes(
    [&](eprosima::fastcdr::Cdr & cdr) {
      cdr << static_cast<uint32_t>(rosidl_typesupport_fastrtps_cpp::kBufferDescriptorMarker1);
      cdr << static_cast<uint32_t>(rosidl_typesupport_fastrtps_cpp::kBufferDescriptorMarker2);
      cdr << std::string("demo");
    });

  eprosima::fastcdr::FastBuffer fast_buffer(
    reinterpret_cast<char *>(bytes.data()), bytes.size());
  eprosima::fastcdr::Cdr cdr(fast_buffer);
  rosidl::Buffer<uint8_t> output;
  const auto endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();
  rosidl_typesupport_fastrtps_cpp::BufferSerializationContext serialization_context;

  bool result = rosidl_typesupport_fastrtps_cpp::deserialize_buffer_with_endpoint(
    cdr, output, endpoint_info, serialization_context);
  EXPECT_FALSE(result) <<
    "Expected descriptor path deserialization to fail for unregistered backend";
}
