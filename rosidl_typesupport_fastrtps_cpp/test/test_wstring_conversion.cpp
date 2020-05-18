// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include <osrf_testing_tools_cpp/scope_exit.hpp>
#include <string>

#include "rosidl_typesupport_fastrtps_cpp/wstring_conversion.hpp"

using rosidl_typesupport_fastrtps_cpp::u16string_to_wstring;
using rosidl_typesupport_fastrtps_cpp::wstring_to_u16string;

TEST(test_wstring_conversion, wstring_round_trip)
{
  std::wstring wstring(L"¡Hola, Mundo!");
  std::u16string u16string;

  // wstr -> u16str
  EXPECT_TRUE(wstring_to_u16string(wstring, u16string));
  EXPECT_EQ(wstring.length(), wstring.length());

  // u16str -> wstr
  std::wstring converted_wstring;
  u16string_to_wstring(u16string, converted_wstring);
  EXPECT_EQ(wstring, converted_wstring);
}
