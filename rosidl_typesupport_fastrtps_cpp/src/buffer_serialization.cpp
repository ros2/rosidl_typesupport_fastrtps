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

#include "rosidl_typesupport_fastrtps_cpp/buffer_serialization.hpp"

namespace rosidl_typesupport_fastrtps_cpp
{

std::unordered_map<std::string, BufferDescriptorOps> & get_backend_descriptor_ops()
{
  static std::unordered_map<std::string, BufferDescriptorOps> ops;
  return ops;
}

std::unordered_map<std::string, DescriptorSerializers> & get_descriptor_serializers()
{
  static std::unordered_map<std::string, DescriptorSerializers> serializers;
  return serializers;
}

}  // namespace rosidl_typesupport_fastrtps_cpp
