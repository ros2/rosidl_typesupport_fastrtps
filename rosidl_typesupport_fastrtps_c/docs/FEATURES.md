# `rosidl_typesupport_fastrtps_c` features

`rosidl_typesupport_fastrtps_c` provides a Python generator [executable](bin/rosidl_typesupport_fastrtps_c) based on Empy to create rosidl C source files for use with eProsima's FastRTPS.

The templates utilized by this generator executable are located in resource directory and generate both headers and sources.
Headers:
* [Messages](resource/msg__rosidl_typesupport_fastrtps_c.h.em)
* [Services](resource/srv__rosidl_typesupport_fastrtps_c.h.em)

Sources:
* [Messages](resource/msg__type_support_c.cpp.em)
* [Services](resource/srv__type_support_c.cpp.em)

It does not generate separate files for actions.

The generator also generates a [visibility_control](resource/rosidl_typesupport_fastrtps_c__visibility_control.h.in) header based on https://gcc.gnu.org/wiki/Visibility.

## Non-Generated helper files

`rosidl_typesupport_fastrtps_c` defines a typesupport identifier, which is declared in [identifier.h](include/rosidl_typesupport_fastrtps_c/identifier.h).

`rosidl_typesupport_fastrtps_c` provides the following functionality for incorporation into generated typesupport source files.

* [wstring_conversion.hpp](include/rosidl_typesupport_fastrtps_c/wstring_conversion.hpp): Simple conversion functions from u16string types to wstring and vice versa.
