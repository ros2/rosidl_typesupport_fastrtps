// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#ifndef ROSIDL_TYPESUPPORT_FASTRTPS_CPP__SERIALIZATION_HELPERS_HPP_
#define ROSIDL_TYPESUPPORT_FASTRTPS_CPP__SERIALIZATION_HELPERS_HPP_

#include <rosidl_typesupport_fastrtps_cpp/visibility_control.h>

#include <rosidl_runtime_cpp/cdr_compatible_fixed_capacity_string.hpp>
#include <fastcdr/Cdr.h>

#include <string>

namespace eprosima {
namespace fastcdr {

template<uint32_t Capacity>
inline eprosima::fastcdr::Cdr& operator << (
	eprosima::fastcdr::Cdr& cdr, const rosidl_runtime_cpp::CDRCompatibleFixedCapacityString<Capacity>& str)
{
	cdr << Capacity;
	cdr.serializeArray(str.c_str(), Capacity);
	return cdr;
}

template<uint32_t Capacity>
inline eprosima::fastcdr::Cdr& operator >> (
	eprosima::fastcdr::Cdr& cdr, rosidl_runtime_cpp::CDRCompatibleFixedCapacityString<Capacity>& str)
{
	std::string tmp_str;
	cdr >> tmp_str;
	str = tmp_str;
	return cdr;
}

}  // namespace fastcdr
}  // namespace eprosima

#endif  // ROSIDL_TYPESUPPORT_FASTRTPS_CPP__SERIALIZATION_HELPERS_HPP_
