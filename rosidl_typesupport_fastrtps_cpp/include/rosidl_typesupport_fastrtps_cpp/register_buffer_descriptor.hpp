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

#ifndef ROSIDL_TYPESUPPORT_FASTRTPS_CPP__REGISTER_BUFFER_DESCRIPTOR_HPP_
#define ROSIDL_TYPESUPPORT_FASTRTPS_CPP__REGISTER_BUFFER_DESCRIPTOR_HPP_

#include <memory>
#include <string>

#include "rosidl_typesupport_fastrtps_cpp/buffer_serialization.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support_decl.hpp"

namespace rosidl_typesupport_fastrtps_cpp
{

/// Register FastCDR serialization functions for a buffer descriptor message type.
///
/// This leverages the existing rosidl-generated type support callbacks
/// (cdr_serialize/cdr_deserialize via message_type_support_callbacks_t) so that
/// backend vendors do not need to manually write registration code or build
/// separate registration libraries.
///
/// Backend implementations should call this once during construction:
///   rosidl_typesupport_fastrtps_cpp::register_buffer_descriptor<
///     my_backend_msgs::msg::MyDescriptor>("my_backend");
///
/// @tparam DescriptorMsgT  The rosidl-generated descriptor message type
///                          (e.g., demo_buffer_backend_msgs::msg::DemoBufferDescriptor).
/// @param backend_name      The backend type name used as the registry key
///                          (e.g., "demo", "cuda").
template<typename DescriptorMsgT>
inline void register_buffer_descriptor(const std::string & backend_name)
{
  const auto * ts_handle =
    rosidl_typesupport_fastrtps_cpp::get_message_type_support_handle<DescriptorMsgT>();
  const auto * callbacks =
    static_cast<const message_type_support_callbacks_t *>(ts_handle->data);

  DescriptorSerializers desc_ser;

  desc_ser.serialize = [callbacks](
    eprosima::fastcdr::Cdr & cdr,
    const std::shared_ptr<void> & desc_ptr,
    const rmw_topic_endpoint_info_t & endpoint_info)
    {
      if (callbacks->cdr_serialize_with_endpoint) {
        callbacks->cdr_serialize_with_endpoint(desc_ptr.get(), cdr, endpoint_info);
      } else {
        callbacks->cdr_serialize(desc_ptr.get(), cdr);
      }
    };

  desc_ser.deserialize = [callbacks](
    eprosima::fastcdr::Cdr & cdr,
    const rmw_topic_endpoint_info_t & endpoint_info) -> std::shared_ptr<void>
    {
      auto desc = std::make_shared<DescriptorMsgT>();
      if (callbacks->cdr_deserialize_with_endpoint) {
        callbacks->cdr_deserialize_with_endpoint(cdr, desc.get(), endpoint_info);
      } else {
        callbacks->cdr_deserialize(cdr, desc.get());
      }
      return desc;
    };

  auto & serializers = get_descriptor_serializers();
  serializers[backend_name] = desc_ser;
}

}  // namespace rosidl_typesupport_fastrtps_cpp

#endif  // ROSIDL_TYPESUPPORT_FASTRTPS_CPP__REGISTER_BUFFER_DESCRIPTOR_HPP_
