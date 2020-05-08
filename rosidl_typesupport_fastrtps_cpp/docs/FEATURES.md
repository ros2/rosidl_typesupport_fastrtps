# `rosidl_typesupport_fastrtps_cpp` features

## Empy Generated Files

`rosidl_typesupport_fastrtps_cpp` provides a Python generator [executable](bin/rosidl_typesupport_fastrtps_cpp) based on Empy to create rosidl C source files for use with eProsima's FastRTPS.

The templates utilized by this generator executable are located in resource directory and generate both headers and sources.
Headers:
* [Messages](resource/msg__rosidl_typesupport_fastrtps_cpp.h.em)
* [Services](resource/srv__rosidl_typesupport_fastrtps_cpp.h.em)

Sources:
* [Messages](resource/msg__type_support.cpp.em)
* [Services](resource/srv__type_support.cpp.em)

It does not generate separate files for actions.

The generator also generates a [visibility_control](resource/rosidl_typesupport_fastrtps_cpp__visibility_control.h.in) header based on https://gcc.gnu.org/wiki/Visibility.

## Non-Generated helper files

`rosidl_typesupport_fastrtps_cpp` defines a typesupport identifier, which is declared in [identifier.hpp](include/rosidl_typesupport_fastrtps_cpp/identifier.hpp).

`rosidl_typesupport_fastrtps_cpp` provides the following functionality for incorporation into generated typesupport source files.

* [wstring_conversion.hpp](include/rosidl_typesupport_fastrtps_cpp/wstring_conversion.hpp): Simple conversion functions from u16string types to wstring and vice versa.

`rosidl_typesupport_fastrtps_cpp` includes definitions for type support callback structs.
They are defined for both:
* [Messages](include/rosidl_typesupport_fastrtps_cpp/message_type_support.h)
* [Services](include/rosidl_typesupport_fastrtps_cpp/service_type_support.h)

`rosidl_typesupport_fastrtps_cpp` also defines several headers that declare simple handle getter functions for rosidl types that are implemented in the generated source files.
This is done for both:
* [Messages](include/rosidl_typesupport_fastrtps_cpp/message_type_support_decl.hpp)
* [Services](include/rosidl_typesupport_fastrtps_cpp/service_type_support_decl.hpp)
