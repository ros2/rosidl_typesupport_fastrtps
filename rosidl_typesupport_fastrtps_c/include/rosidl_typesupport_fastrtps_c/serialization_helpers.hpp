// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#ifndef ROSIDL_TYPESUPPORT_FASTRTPS_C__SERIALIZATION_HELPERS_HPP_
#define ROSIDL_TYPESUPPORT_FASTRTPS_C__SERIALIZATION_HELPERS_HPP_

#include <string>

#include "fastcdr/Cdr.h"

#include "rosidl_runtime_c/u16string.h"
#include "rosidl_runtime_c/u16string_functions.h"

#include "rosidl_typesupport_fastrtps_c/visibility_control.h"

namespace rosidl_typesupport_fastrtps_c
{

inline void cdr_serialize(
  eprosima::fastcdr::Cdr & cdr,
  const rosidl_runtime_c__U16String & u16str)
{
  auto len = static_cast<uint32_t>(u16str.size);
  cdr << len;
  for (uint32_t i = 0; i < len; ++i) {
    uint32_t c = static_cast<uint32_t>(u16str.data[i]);
    cdr << c;
  }
}

inline bool cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  rosidl_runtime_c__U16String & u16str)
{
  uint32_t len;
  cdr >> len;
  bool succeeded = rosidl_runtime_c__U16String__resize(&u16str, len);
  if (!succeeded) {
    return false;
  }

  for (uint32_t i = 0; i < len; ++i) {
    uint32_t c;
    cdr >> c;
    u16str.data[i] = static_cast<char16_t>(c);
  }

  return true;
}

}  // namespace rosidl_typesupport_fastrtps_c

#endif  // ROSIDL_TYPESUPPORT_FASTRTPS_C__SERIALIZATION_HELPERS_HPP_
