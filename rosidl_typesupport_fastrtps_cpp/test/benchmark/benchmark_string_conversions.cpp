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

#include <string>

#include "fastcdr/Cdr.h"
#include "rcutils/macros.h"

#include "rosidl_typesupport_fastrtps_cpp/serialization_helpers.hpp"

#include "performance_test_fixture/performance_test_fixture.hpp"

using performance_test_fixture::PerformanceTest;

namespace
{
constexpr const uint64_t kSize = 1024;
}

BENCHMARK_F(PerformanceTest, wstring_to_u16string)(benchmark::State & st)
{
  namespace fastcdr = eprosima::fastcdr;

  std::wstring wstring(kSize, '*');
  char raw_buffer[kSize * 4 + 4];  // 4 bytes per character + 4 bytes for the length
  fastcdr::FastBuffer buffer(raw_buffer, sizeof(raw_buffer));
  fastcdr::Cdr cdr(buffer, fastcdr::Cdr::DEFAULT_ENDIAN, fastcdr::Cdr::DDS_CDR);
  cdr << wstring;

  reset_heap_counters();

  for (auto _ : st) {
    RCUTILS_UNUSED(_);
    std::u16string u16string;
    cdr.reset();
    rosidl_typesupport_fastrtps_cpp::cdr_deserialize(cdr, u16string);
  }
}

BENCHMARK_F(PerformanceTest, u16string_to_wstring)(benchmark::State & st)
{
  namespace fastcdr = eprosima::fastcdr;

  std::u16string u16string(kSize, '*');
  char raw_buffer[kSize * 4 + 4];  // 4 bytes per character + 4 bytes for the length
  fastcdr::FastBuffer buffer(raw_buffer, sizeof(raw_buffer));
  fastcdr::Cdr cdr(buffer, fastcdr::Cdr::DEFAULT_ENDIAN, fastcdr::Cdr::DDS_CDR);

  reset_heap_counters();

  for (auto _ : st) {
    RCUTILS_UNUSED(_);
    cdr.reset();
    rosidl_typesupport_fastrtps_cpp::cdr_serialize(cdr, u16string);
  }
}
