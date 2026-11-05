@# Included from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
@{
from rosidl_generator_c import idl_structure_type_to_c_typename
from rosidl_generator_type_description import GET_DESCRIPTION_FUNC
from rosidl_generator_type_description import GET_HASH_FUNC
from rosidl_generator_type_description import GET_SOURCES_FUNC
from rosidl_parser.definition import AbstractNestedType
from rosidl_parser.definition import AbstractSequence
from rosidl_parser.definition import AbstractString
from rosidl_parser.definition import AbstractWString
from rosidl_parser.definition import ACTION_FEEDBACK_MESSAGE_SUFFIX
from rosidl_parser.definition import ACTION_FEEDBACK_SUFFIX
from rosidl_parser.definition import ACTION_GOAL_SUFFIX
from rosidl_parser.definition import ACTION_RESULT_SUFFIX
from rosidl_parser.definition import SERVICE_EVENT_MESSAGE_SUFFIX
from rosidl_parser.definition import SERVICE_REQUEST_MESSAGE_SUFFIX
from rosidl_parser.definition import SERVICE_RESPONSE_MESSAGE_SUFFIX
from rosidl_parser.definition import BasicType
from rosidl_parser.definition import NamespacedType
from rosidl_parser.definition import UnboundedSequence
from rosidl_pycommon import convert_camel_case_to_lower_case_underscore

# Service Request/Response/Event and Action Goal/Result/Feedback/FeedbackMessage
# types are always generated into a single shared file together with their
# parent interface, and are always emitted earlier in that same file due to
# generation order, so nested references to them must not be turned into a
# separate #include (there is no separate file for them).
_self_file_suffixes = (
    SERVICE_REQUEST_MESSAGE_SUFFIX,
    SERVICE_RESPONSE_MESSAGE_SUFFIX,
    SERVICE_EVENT_MESSAGE_SUFFIX,
    ACTION_GOAL_SUFFIX,
    ACTION_RESULT_SUFFIX,
    ACTION_FEEDBACK_SUFFIX,
    ACTION_FEEDBACK_MESSAGE_SUFFIX,
)

# Detect direct Buffer fields (only uint8[] UnboundedSequence becomes Buffer<T>).
has_direct_buffer_fields = False
for member in message.structure.members:
    if isinstance(member.type, UnboundedSequence):
        if isinstance(member.type.value_type, BasicType) and member.type.value_type.typename == 'uint8':
            has_direct_buffer_fields = True
            break

# Collect nested message types so the generated C helper can compose
# has_buffer_fields transitively without needing foreign IDL structures at
# template expansion time.
nested_type_keys = []
_seen_nested_type_keys = set()
for member in message.structure.members:
    type_ = member.type
    if isinstance(type_, AbstractNestedType):
        type_ = type_.value_type
    if isinstance(type_, NamespacedType):
        key = (*type_.namespaces, type_.name)
        if key not in _seen_nested_type_keys:
            _seen_nested_type_keys.add(key)
            nested_type_keys.append(key)

if has_direct_buffer_fields:
    has_buffer_fields_expression = 'true'
else:
    has_buffer_field_checks = [
        'has_buffer_fields_%s()' % '__'.join(nested_key)
        for nested_key in nested_type_keys
    ]
    has_buffer_fields_expression = ' ||\n    '.join(has_buffer_field_checks) or 'false'

include_parts = [package_name] + list(interface_path.parents[0].parts) + [
    'detail', convert_camel_case_to_lower_case_underscore(interface_path.stem)]
include_base = '/'.join(include_parts)


header_files = [
    'cassert',
    'cstddef',
    # Provides the rosidl_typesupport_fastrtps_c__identifier symbol declaration.
    'rosidl_typesupport_fastrtps_c/identifier.h',
    # Provides the definition of the message_type_support_callbacks_t struct.
    'rosidl_typesupport_fastrtps_cpp/message_type_support.h',
    package_name + '/msg/rosidl_typesupport_fastrtps_c__visibility_control.h',
    include_base + '__struct.h',
    include_base + '__functions.h',
    # Inline (de)serialization implementation for this message, and
    # transitively for any nested message types. Including it here (instead
    # of forward-declaring the nested functions) allows the compiler to
    # fully inline nested (de)serialization logic.
    include_base + '__rosidl_typesupport_fastrtps_c_impl.hpp',
    'fastcdr/Cdr.h',
]
}@
@[for header_file in header_files]@
@[    if header_file in include_directives]@
// already included above
// @
@[    else]@
@{include_directives.add(header_file)}@
@[    end if]@
@[    if '/' not in header_file]@
#include <@(header_file)>
@[    else]@
#include "@(header_file)"
@[    end if]@
@[end for]@
@# Buffer-backed uint8[] fields use the is_rosidl_buffer flag on the sequence struct.

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

@# // Include the message header for each non-primitive field.
#if defined(__cplusplus)
extern "C"
{
#endif

@{
includes = {}
for member in message.structure.members:
    keys = set([])
    if isinstance(member.type, AbstractSequence) and isinstance(member.type.value_type, BasicType):
        keys.add('rosidl_runtime_c/primitives_sequence.h')
        keys.add('rosidl_runtime_c/primitives_sequence_functions.h')
    type_ = member.type
    if isinstance(type_, AbstractNestedType):
        type_ = type_.value_type
    if isinstance(type_, AbstractString):
        keys.add('rosidl_runtime_c/string.h')
        keys.add('rosidl_runtime_c/string_functions.h')
    elif isinstance(type_, AbstractWString):
        keys.add('rosidl_runtime_c/u16string.h')
        keys.add('rosidl_runtime_c/u16string_functions.h')
    elif isinstance(type_, NamespacedType):
        import sys
        if (
            type_.name.endswith(SERVICE_REQUEST_MESSAGE_SUFFIX) or
            type_.name.endswith(SERVICE_RESPONSE_MESSAGE_SUFFIX)
        ):
            continue
        if (
            type_.name.endswith(ACTION_GOAL_SUFFIX) or
            type_.name.endswith(ACTION_RESULT_SUFFIX) or
            type_.name.endswith(ACTION_FEEDBACK_SUFFIX)
        ):
            typename = type_.name.rsplit('_', 1)[0]
        else:
            typename = type_.name
        keys.add('/'.join(type_.namespaces + ['detail', convert_camel_case_to_lower_case_underscore(typename)]) + '__functions.h')
    for key in keys:
        if key not in includes:
            includes[key] = set([])
        includes[key].add(member.name)
}@
@[for header_file in sorted(includes.keys())]@
@[    if header_file in include_directives]@
// already included above
// @
@[    else]@
@{include_directives.add(header_file)}@
@[    end if]@
#include "@(header_file)"  // @(', '.join(sorted(includes[header_file])))
@[end for]@

// Include the inline (de)serialization headers of nested message types so
// their bodies are visible here and can be fully inlined by the compiler.
@{
_nested_impl_keys = [k for k in nested_type_keys if not k[-1].endswith(_self_file_suffixes)]
}@
@[for key in _nested_impl_keys]@
@{
nested_impl_header = '/'.join(
    list(key[:-1]) +
    ['detail', convert_camel_case_to_lower_case_underscore(key[-1])]
) + '__rosidl_typesupport_fastrtps_c_impl.hpp'
}@
@[  if nested_impl_header in include_directives]@
// already included above
// @
@[  else]@
@{include_directives.add(nested_impl_header)}@
@[  end if]@
#include "@(nested_impl_header)"
@[end for]@

// forward declare type support functions still needed directly in this
// file. cdr_serialize/cdr_deserialize/get_serialized_size/max_serialized_size
// and their _key/_with_endpoint variants are provided (inline) by the
// included impl headers above instead.
@{
forward_declares = {}
for member in message.structure.members:
    type_ = member.type
    if isinstance(type_, AbstractNestedType):
        type_ = type_.value_type
    if isinstance(type_, NamespacedType):
        key = (*type_.namespaces, type_.name)
        forward_declares[key] = True
}@
@[for key in sorted(forward_declares.keys())]@

@[  if key[0] != package_name]@
ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_@(package_name)
@[  end if]@
const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, @(', '.join(key)))();

@[  if key[0] != package_name]@
ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_@(package_name)
@[  end if]@
bool has_buffer_fields_@('__'.join(key))();
@[end for]@

@# // Make callback functions specific to this message type.

using _@(message.structure.namespaced_type.name)__ros_msg_type = @('__'.join(message.structure.namespaced_type.namespaced_name()));

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_@(package_name)
bool cdr_serialize_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
  const @('__'.join(message.structure.namespaced_type.namespaced_name())) * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  return impl_cdr_serialize_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(ros_message, cdr);
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_@(package_name)
bool cdr_deserialize_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
  eprosima::fastcdr::Cdr & cdr,
  @('__'.join(message.structure.namespaced_type.namespaced_name())) * ros_message)
{
  return impl_cdr_deserialize_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(cdr, ros_message);
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_@(package_name)
size_t get_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  return impl_get_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(untyped_ros_message, current_alignment);
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_@(package_name)
size_t max_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  return impl_max_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
    full_bounded, is_plain, current_alignment);
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_@(package_name)
bool cdr_serialize_key_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
  const @('__'.join(message.structure.namespaced_type.namespaced_name())) * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  return impl_cdr_serialize_key_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(ros_message, cdr);
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_@(package_name)
size_t get_serialized_size_key_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  return impl_get_serialized_size_key_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(untyped_ros_message, current_alignment);
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_@(package_name)
size_t max_serialized_size_key_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  return impl_max_serialized_size_key_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
    full_bounded, is_plain, current_alignment);
}

@[  if message.structure.has_any_member_with_annotation('key') ]@
static bool _@(message.structure.namespaced_type.name)__cdr_serialize_key(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const @('__'.join(message.structure.namespaced_type.namespaced_name())) * ros_message = static_cast<const @('__'.join(message.structure.namespaced_type.namespaced_name())) *>(untyped_ros_message);
  (void)ros_message;
  return cdr_serialize_key_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(ros_message, cdr);
}

static size_t _@(message.structure.namespaced_type.name)__get_serialized_size_key(
  const void * untyped_ros_message)
{
  return get_serialized_size_key_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
    untyped_ros_message, 0);
}

static
size_t
_@(message.structure.namespaced_type.name)__max_serialized_size_key(
  bool & is_unbounded)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_key_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
    full_bounded, is_plain, 0);

  is_unbounded = !full_bounded;
  return ret_val;
}

static message_type_support_key_callbacks_t __key_callbacks_@(message.structure.namespaced_type.name) = {
  _@(message.structure.namespaced_type.name)__max_serialized_size_key,
  _@(message.structure.namespaced_type.name)__get_serialized_size_key,
  _@(message.structure.namespaced_type.name)__cdr_serialize_key
};
@[  end if]@
@
@# // Collect the callback functions and provide a function to get the type support struct.

static bool _@(message.structure.namespaced_type.name)__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const @('__'.join(message.structure.namespaced_type.namespaced_name())) * ros_message = static_cast<const @('__'.join(message.structure.namespaced_type.namespaced_name())) *>(untyped_ros_message);
  (void)ros_message;
  return cdr_serialize_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(ros_message, cdr);
}

static bool _@(message.structure.namespaced_type.name)__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  @('__'.join(message.structure.namespaced_type.namespaced_name())) * ros_message = static_cast<@('__'.join(message.structure.namespaced_type.namespaced_name())) *>(untyped_ros_message);
  (void)ros_message;
  return cdr_deserialize_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(cdr, ros_message);
}

static uint32_t _@(message.structure.namespaced_type.name)__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
      untyped_ros_message, 0));
}

static size_t _@(message.structure.namespaced_type.name)__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}

@
ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_@(package_name)
bool cdr_serialize_with_endpoint_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
  const @('__'.join(message.structure.namespaced_type.namespaced_name())) * ros_message,
  eprosima::fastcdr::Cdr & cdr,
  const rmw_topic_endpoint_info_t & endpoint_info,
  const rosidl_typesupport_fastrtps_cpp::BufferSerializationContext & serialization_context)
{
  return impl_cdr_serialize_with_endpoint_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
    ros_message, cdr, endpoint_info, serialization_context);
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_@(package_name)
bool cdr_deserialize_with_endpoint_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
  eprosima::fastcdr::Cdr & cdr,
  @('__'.join(message.structure.namespaced_type.namespaced_name())) * ros_message,
  const rmw_topic_endpoint_info_t & endpoint_info,
  const rosidl_typesupport_fastrtps_cpp::BufferSerializationContext & serialization_context)
{
  return impl_cdr_deserialize_with_endpoint_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
    cdr, ros_message, endpoint_info, serialization_context);
}

static bool _@(message.structure.namespaced_type.name)__cdr_serialize_with_endpoint(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr,
  const rmw_topic_endpoint_info_t & endpoint_info,
  const rosidl_typesupport_fastrtps_cpp::BufferSerializationContext & serialization_context)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const @('__'.join(message.structure.namespaced_type.namespaced_name())) * ros_message =
    static_cast<const @('__'.join(message.structure.namespaced_type.namespaced_name())) *>(untyped_ros_message);
  return cdr_serialize_with_endpoint_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
    ros_message, cdr, endpoint_info, serialization_context);
}

static bool _@(message.structure.namespaced_type.name)__cdr_deserialize_with_endpoint(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message,
  const rmw_topic_endpoint_info_t & endpoint_info,
  const rosidl_typesupport_fastrtps_cpp::BufferSerializationContext & serialization_context)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  @('__'.join(message.structure.namespaced_type.namespaced_name())) * ros_message =
    static_cast<@('__'.join(message.structure.namespaced_type.namespaced_name())) *>(untyped_ros_message);
  return cdr_deserialize_with_endpoint_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
    cdr, ros_message, endpoint_info, serialization_context);
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_@(package_name)
bool has_buffer_fields_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))()
{
  return
    @(has_buffer_fields_expression);
}
@# // Collect the callback functions and provide a function to get the type support struct.

static message_type_support_callbacks_t __callbacks_@(message.structure.namespaced_type.name) = {
  "@('::'.join([package_name] + list(interface_path.parents[0].parts)))",
  "@(message.structure.namespaced_type.name)",
  _@(message.structure.namespaced_type.name)__cdr_serialize,
  _@(message.structure.namespaced_type.name)__cdr_deserialize,
  _@(message.structure.namespaced_type.name)__get_serialized_size,
  _@(message.structure.namespaced_type.name)__max_serialized_size,
@[  if message.structure.has_any_member_with_annotation('key') ]@
  &__key_callbacks_@(message.structure.namespaced_type.name),
@[  else]@
  nullptr,
@[  end if]@
  has_buffer_fields_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(),
  _@(message.structure.namespaced_type.name)__cdr_serialize_with_endpoint,
  _@(message.structure.namespaced_type.name)__cdr_deserialize_with_endpoint
};

static rosidl_message_type_support_t _@(message.structure.namespaced_type.name)__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_@(message.structure.namespaced_type.name),
  get_message_typesupport_handle_function,
  &@(idl_structure_type_to_c_typename(message.structure.namespaced_type))__@(GET_HASH_FUNC),
  &@(idl_structure_type_to_c_typename(message.structure.namespaced_type))__@(GET_DESCRIPTION_FUNC),
  &@(idl_structure_type_to_c_typename(message.structure.namespaced_type))__@(GET_SOURCES_FUNC),
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, @(', '.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])))() {
  return &_@(message.structure.namespaced_type.name)__type_support;
}

#if defined(__cplusplus)
}
#endif
