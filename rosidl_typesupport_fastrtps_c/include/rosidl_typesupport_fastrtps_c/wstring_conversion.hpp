// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#ifndef ROSIDL_TYPESUPPORT_FASTRTPS_C__WSTRING_CONVERSION_HPP_
#define ROSIDL_TYPESUPPORT_FASTRTPS_C__WSTRING_CONVERSION_HPP_

#include <string>

#include "rcutils/macros.h"
#include "rosidl_runtime_c/u16string.h"
#include "rosidl_typesupport_fastrtps_c/visibility_control.h"

namespace rosidl_typesupport_fastrtps_c
{

/// Convert a `rosidl_runtime_c__U16String` into a std::wstring
/**
 * \param[in] u16str The 16-bit character string to copy from.
 * \param[in,out] wstr The std::wstring to copy to.
 */
ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC
  RCUTILS_DEPRECATED_WITH_MSG("not used by core packages")
void u16string_to_wstring(
  const rosidl_runtime_c__U16String & u16str, std::wstring & wstr);

/// Convert a std::wstring into a `rosidl_runtime_c__U16String`.
/**
 * \param[in] wstr The std::wstring to copy from.
 * \param[in,out] u16str The u16string to copy to.
 * \return true if resizing u16str and assignment succeeded, otherwise false.
 */
ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC
  RCUTILS_DEPRECATED_WITH_MSG("not used by core packages")
bool wstring_to_u16string(
  const std::wstring & wstr, rosidl_runtime_c__U16String & u16str);

}  // namespace rosidl_typesupport_fastrtps_c

#endif  // ROSIDL_TYPESUPPORT_FASTRTPS_C__WSTRING_CONVERSION_HPP_
