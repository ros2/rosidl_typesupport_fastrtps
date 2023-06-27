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

namespace rosidl_typesupport_fastrtps_cpp {

template<typename Allocator>
inline void get_string_size(
    const std::basic_string<char, std::char_traits<char>, Allocator>& str,
    size_t& current_alignment)
{
  const size_t padding = 4;
  current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
      str.size() + 1;
}

template<typename Allocator>
inline void get_string_size(
    const std::basic_string<char16_t, std::char_traits<char16_t>, Allocator>& str,
    size_t& current_alignment)
{
  const size_t padding = 4;
  const size_t wchar_size = 4;

  current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
      wchar_size * (str.size() + 1);
}

template<uint32_t Capacity>
inline void get_string_size(
    const rosidl_runtime_cpp::CDRCompatibleFixedCapacityString<Capacity>& /* str */,
    size_t& current_alignment)
{
  const size_t padding = 4;
  current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
      Capacity;
}

template <typename T>
struct max_string_size_helper
{
};

template <typename Allocator>
struct max_string_size_helper<std::basic_string<char, std::char_traits<char>, Allocator> >
{
    static inline void max_string_size(
        bool& full_bounded,
        bool& is_plain,
        size_t& current_alignment,
        const size_t array_size,
        const size_t max_size)
    {
        const size_t padding = 4;

        full_bounded = false;
        is_plain = false;
        for (size_t index = 0; index < array_size; ++index) {
            current_alignment += padding +
                eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
                max_size + 1;
        }
    }
};

template <typename Allocator>
struct max_string_size_helper<std::basic_string<char16_t, std::char_traits<char16_t>, Allocator> >
{
    static inline void max_string_size(
        bool& full_bounded,
        bool& is_plain,
        size_t& current_alignment,
        const size_t array_size,
        const size_t max_size)
    {
        const size_t padding = 4;
        const size_t wchar_size = 4;

        full_bounded = false;
        is_plain = false;
        for (size_t index = 0; index < array_size; ++index) {
            current_alignment += padding +
                eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
                wchar_size * (max_size + 1);
        }
    }
};

template<uint32_t Capacity>
struct max_string_size_helper<rosidl_runtime_cpp::CDRCompatibleFixedCapacityString<Capacity> >
{
    static inline void max_string_size(
        bool& /*full_bounded*/,
        bool& /*is_plain*/,
        size_t& current_alignment,
        const size_t array_size,
        const size_t /*max_size*/)
    {
        const size_t padding = 4;
        for (size_t index = 0; index < array_size; ++index) {
            current_alignment += padding +
                eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
                Capacity;
        }
    }
};

template<typename T, std::size_t N>
struct max_string_size_helper<std::array<T, N> > : public max_string_size_helper<T>
{};

template<typename T, typename Allocator>
struct max_string_size_helper<std::vector<T, Allocator> >
{
    static inline void max_string_size(
        bool& full_bounded,
        bool& is_plain,
        size_t& current_alignment,
        const size_t array_size,
        const size_t max_size)
    {
        full_bounded = false;
        is_plain = false;

        max_string_size_helper<T>::max_string_size(full_bounded, is_plain, current_alignment, array_size, max_size);
    }
};

template<typename T, std::size_t N, typename Allocator>
struct max_string_size_helper<rosidl_runtime_cpp::BoundedVector<T, N, Allocator> >
{
    static inline void max_string_size(
        bool& full_bounded,
        bool& is_plain,
        size_t& current_alignment,
        const size_t array_size,
        const size_t max_size)
    {
        is_plain = false;

        max_string_size_helper<T>::max_string_size(full_bounded, is_plain, current_alignment, array_size, max_size);
    }
};

}  // namespace rosidl_typesupport_fastrtps_cpp

#endif  // ROSIDL_TYPESUPPORT_FASTRTPS_CPP__SERIALIZATION_HELPERS_HPP_
