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

#include "rosidl_runtime_c/string_functions.h"
#include "rosidl_runtime_c/u16string_functions.h"

#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"

#include "performance_test_fixture/performance_test_fixture.hpp"

using performance_test_fixture::PerformanceTest;

BENCHMARK_DEFINE_F(PerformanceTest, wstring_to_u16string_complexity)(benchmark::State & st)
{
  size_t len = st.range(0);
  std::wstring wstring(len, '*');

  rosidl_runtime_c__U16String s;
  if (!rosidl_runtime_c__U16String__init(&s)) {
    st.SkipWithError("String initialization failed");
    return;
  }

  reset_heap_counters();

  for (auto _ : st) {
    rosidl_typesupport_fastrtps_c::wstring_to_u16string(wstring, s);
  }

  st.SetComplexityN(len);

  rosidl_runtime_c__U16String__fini(&s);
}

BENCHMARK_REGISTER_F(PerformanceTest, wstring_to_u16string_complexity)
->RangeMultiplier(2)->Range(1 << 3, 1 << 12);


BENCHMARK_DEFINE_F(PerformanceTest, u16string_to_wstring_complexity)(benchmark::State & st)
{
  size_t len = st.range(0);
  std::wstring data(len, '*');

  rosidl_runtime_c__U16String s;
  if (!rosidl_runtime_c__U16String__init(&s)) {
    st.SkipWithError("String initialization failed");
    return;
  }

  // Just do a copy
  rosidl_typesupport_fastrtps_c::wstring_to_u16string(data, s);

  reset_heap_counters();

  for (auto _ : st) {
    std::wstring actual;
    rosidl_typesupport_fastrtps_c::u16string_to_wstring(s, actual);
  }

  st.SetComplexityN(len);

  rosidl_runtime_c__U16String__fini(&s);
}

BENCHMARK_REGISTER_F(PerformanceTest, u16string_to_wstring_complexity)
->RangeMultiplier(2)->Range(1 << 3, 1 << 12);
