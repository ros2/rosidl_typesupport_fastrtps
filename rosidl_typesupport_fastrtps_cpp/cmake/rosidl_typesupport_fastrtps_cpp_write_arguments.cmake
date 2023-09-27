# Copyright 2023 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(_output_path "${CMAKE_CURRENT_BINARY_DIR}/rosidl_typesupport_fastrtps_cpp/${PROJECT_NAME}")

# Create a list of IDL files from other packages that this generator should depend on
set(_dependency_files "")
set(_dependencies "")
foreach(_pkg_name ${rosidl_generate_interfaces_DEPENDENCY_PACKAGE_NAMES})
  foreach(_idl_file ${${_pkg_name}_IDL_FILES})
    # ${{_pkg_name}_DIR} is absolute path ending in 'share/<pkg_name>/cmake', so go back one
    # directory for IDL files
    set(_abs_idl_file "${${_pkg_name}_DIR}/../${_idl_file}")
    normalize_path(_abs_idl_file "${_abs_idl_file}")
    list(APPEND _dependency_files "${_abs_idl_file}")
    list(APPEND _dependencies "${_pkg_name}:${_abs_idl_file}")
  endforeach()
endforeach()

# Create a list of templates and source files this generator uses, and check that they exist
set(target_dependencies
  ${rosidl_typesupport_fastrtps_cpp_GENERATOR_FILES}
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/idl__rosidl_typesupport_fastrtps_cpp.hpp.em"
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/idl__type_support.cpp.em"
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/msg__rosidl_typesupport_fastrtps_cpp.hpp.em"
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/msg__type_support.cpp.em"
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/srv__rosidl_typesupport_fastrtps_cpp.hpp.em"
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/srv__type_support.cpp.em"
  ${rosidl_generate_interfaces_ABS_IDL_FILES}
  ${_dependency_files})
foreach(dep ${target_dependencies})
  if(NOT EXISTS "${dep}")
    message(FATAL_ERROR "Target dependency '${dep}' does not exist")
  endif()
endforeach()

# Write all this to a file to work around command line length limitations on some platforms
set(generator_arguments_file "${CMAKE_CURRENT_BINARY_DIR}/rosidl_typesupport_fastrtps_cpp__arguments.json")
rosidl_write_generator_arguments(
  "${generator_arguments_file}"
  PACKAGE_NAME "${PROJECT_NAME}"
  IDL_TUPLES "${rosidl_generate_interfaces_IDL_TUPLES}"
  ROS_INTERFACE_DEPENDENCIES "${_dependencies}"
  OUTPUT_DIR "${_output_path}"
  TEMPLATE_DIR "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}"
  TARGET_DEPENDENCIES ${target_dependencies}
  GENERATOR_FILES "${rosidl_typesupport_fastrtps_cpp_GENERATOR_FILES}"
)

list(APPEND rosidl_generator_arguments_files ${generator_arguments_file})