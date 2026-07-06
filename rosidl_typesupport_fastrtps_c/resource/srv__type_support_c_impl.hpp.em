@# Included from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c_impl.hpp.em
@{
TEMPLATE(
    'msg__type_support_c_impl.hpp.em',
    package_name=package_name, interface_path=interface_path, message=service.request_message,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__type_support_c_impl.hpp.em',
    package_name=package_name, interface_path=interface_path, message=service.response_message,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__type_support_c_impl.hpp.em',
    package_name=package_name, interface_path=interface_path, message=service.event_message,
    include_directives=include_directives)
}@
