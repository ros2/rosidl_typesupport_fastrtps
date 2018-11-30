@# Included from rosidl_typesupport_fastrtps_cpp/resource/idl__type_support.cpp.em
@{
from rosidl_cmake import convert_camel_case_to_lower_case_underscore

include_parts = [package_name] + list(interface_path.parents[0].parts) + \
    [convert_camel_case_to_lower_case_underscore(interface_path.stem)]
include_base = '/'.join(include_parts)
}@
#include "@(include_base)__struct.hpp"

@{
TEMPLATE(
    'msg__type_support.cpp.em',
    package_name=package_name, interface_path=interface_path, message=service.request_message,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__type_support.cpp.em',
    package_name=package_name, interface_path=interface_path, message=service.response_message,
    include_directives=include_directives)
}@

@{
header_files = [
    'rmw/error_handling.h',
    'rosidl_typesupport_fastrtps_cpp/identifier.hpp',
    'rosidl_typesupport_fastrtps_cpp/service_type_support.h',
    'rosidl_typesupport_fastrtps_cpp/service_type_support_decl.hpp',
    include_base + '__struct.hpp',
]
}@
@[for header_file in header_files]@
@[    if header_file in include_directives]@
// already included above
// @
@[    else]@
@{include_directives.add(header_file)}@
@[    end if]@
#include "@(header_file)"
@[end for]@
@[  for ns in service.structure_type.namespaces]@

namespace @(ns)
{
@[  end for]@

namespace typesupport_fastrtps_cpp
{

static service_type_support_callbacks_t _@(service.structure_type.name)__callbacks = {
  "@(package_name)",
  "@(service.structure_type.name)",
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, @(', '.join([package_name] + list(interface_path.parents[0].parts))), @(service.structure_type.name)_Request)(),
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, @(', '.join([package_name] + list(interface_path.parents[0].parts))), @(service.structure_type.name)_Response)(),
};

static rosidl_service_type_support_t _@(service.structure_type.name)__handle = {
  rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
  &_@(service.structure_type.name)__callbacks,
  get_service_typesupport_handle_function,
};

}  // namespace typesupport_fastrtps_cpp
@[  for ns in reversed(service.structure_type.namespaces)]@

}  // namespace @(ns)
@[  end for]@

namespace rosidl_typesupport_fastrtps_cpp
{

template<>
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_EXPORT_@(package_name)
const rosidl_service_type_support_t *
get_service_type_support_handle<@('::'.join([package_name] + list(interface_path.parents[0].parts) + [service.structure_type.name]))>()
{
  return &@('::'.join([package_name] + list(interface_path.parents[0].parts)))::typesupport_fastrtps_cpp::_@(service.structure_type.name)__handle;
}

}  // namespace rosidl_typesupport_fastrtps_cpp

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, @(', '.join([package_name] + list(interface_path.parents[0].parts))), @(service.structure_type.name))() {
  return &@('::'.join([package_name] + list(interface_path.parents[0].parts)))::typesupport_fastrtps_cpp::_@(service.structure_type.name)__handle;
}

#ifdef __cplusplus
}
#endif
