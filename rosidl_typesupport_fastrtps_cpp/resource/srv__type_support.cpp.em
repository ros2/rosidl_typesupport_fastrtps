@# Included from rosidl_typesupport_fastrtps_cpp/resource/idl__type_support.cpp.em
@{
from rosidl_pycommon import convert_camel_case_to_lower_case_underscore

include_parts = [package_name] + list(interface_path.parents[0].parts) + [
    'detail', convert_camel_case_to_lower_case_underscore(interface_path.stem)]
include_base = '/'.join(include_parts)
}@
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
TEMPLATE(
    'msg__type_support.cpp.em',
    package_name=package_name, interface_path=interface_path, message=service.event_message,
    include_directives=include_directives)
}@

@{
header_files = [
    'rmw/error_handling.h',
    'rosidl_typesupport_fastrtps_cpp/identifier.hpp',
    'rosidl_typesupport_fastrtps_cpp/service_type_support.h',
    'rosidl_typesupport_fastrtps_cpp/service_type_support_decl.hpp',
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
@[  for ns in service.namespaced_type.namespaces]@

namespace @(ns)
{
@[  end for]@

namespace typesupport_fastrtps_cpp
{

static service_type_support_callbacks_t _@(service.namespaced_type.name)__callbacks = {
  "@('::'.join([package_name] + list(interface_path.parents[0].parts)))",
  "@(service.namespaced_type.name)",
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, @(', '.join([package_name] + list(interface_path.parents[0].parts))), @(service.namespaced_type.name)_Request)(),
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, @(', '.join([package_name] + list(interface_path.parents[0].parts))), @(service.namespaced_type.name)_Response)(),
};



#ifdef __cplusplus
extern "C"
{
#endif

@{event_type = '::'.join([package_name, *interface_path.parents[0].parts, service.namespaced_type.name]) + '_Event'}

void *
rosidl_@('_'.join([package_name, *interface_path.parents[0].parts, service.namespaced_type.name]))_introspection_message_create
(
    const rosidl_service_introspection_info_t * info,
    rcutils_allocator_t * allocator,
    void * request_message,
    void * response_message,
    bool enable_message_payload)
{

  // Use allocator pattern b.c. shared_ptr ref count -> 0
  // add allocator arg
  // could malloc the class (?) manual construct/destruct


  auto * event_msg = static_cast<@event_type *>(allocator->zero_allocate(1, sizeof(@event_type), allocator->state));
  if (nullptr == event_msg) {
    return NULL;
  }
  event_msg = new(event_msg) @(event_type)(); 

  event_msg->info.set__event_type(info->event_type);
  event_msg->info.set__sequence_number(info->sequence_number);
  event_msg->info.stamp.set__sec(info->stamp_sec);
  event_msg->info.stamp.set__nanosec(info->stamp_nanosec);

  std::array<uint8_t, 16> client_id;
  std::move(std::begin(info->client_id), std::end(info->client_id), client_id.begin());
  event_msg->info.client_id.set__uuid(client_id);
  
  if (enable_message_payload) {
    if (nullptr == request_message) {
      event_msg->response.push_back(*static_cast<@('::'.join([package_name, *interface_path.parents[0].parts, service.namespaced_type.name]))_Response *> (response_message));
    } else if (nullptr == response_message) {
      event_msg->request.push_back(*static_cast<@('::'.join([package_name, *interface_path.parents[0].parts, service.namespaced_type.name]))_Request *> (request_message)); } else { // raise an error here
    }
  }

  return event_msg;
}

#include <memory>

bool
rosidl_@('_'.join([package_name, *interface_path.parents[0].parts, service.namespaced_type.name]))_introspection_message_destroy(
void* event_msg,
rcutils_allocator_t * allocator
)
{
  auto * event_msg_ = static_cast<@event_type *>(event_msg);
  // event_msg_->@('::'.join([package_name, *interface_path.parents[0].parts]) + '::~' + service.namespaced_type.name + '_Event')();
  // std::destroy_at(event_msg_); // Why can't I compile with c++17?
  event_msg_->~@(service.namespaced_type.name)_Event();
  allocator->deallocate(event_msg, allocator->state);
  return true;
}


static const rosidl_service_type_support_t _@(service.namespaced_type.name)__handle{
  rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
  &_@(service.namespaced_type.name)__callbacks,
  get_service_typesupport_handle_function,
  rosidl_@('_'.join([package_name, *interface_path.parents[0].parts, service.namespaced_type.name]))_introspection_message_create,
  rosidl_@('_'.join([package_name, *interface_path.parents[0].parts, service.namespaced_type.name]))_introspection_message_destroy,
  ::rosidl_typesupport_fastrtps_cpp::get_message_type_support_handle<@('::'.join([package_name, *interface_path.parents[0].parts, service.namespaced_type.name]))_Event>(),
};

#ifdef __cplusplus
}
#endif




}  // namespace typesupport_fastrtps_cpp
@[  for ns in reversed(service.namespaced_type.namespaces)]@

}  // namespace @(ns)
@[  end for]@

namespace rosidl_typesupport_fastrtps_cpp
{

template<>
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_EXPORT_@(package_name)
const rosidl_service_type_support_t *
get_service_type_support_handle<@('::'.join([package_name] + list(interface_path.parents[0].parts) + [service.namespaced_type.name]))>()
{
  return &@('::'.join([package_name] + list(interface_path.parents[0].parts)))::typesupport_fastrtps_cpp::_@(service.namespaced_type.name)__handle;
}

}  // namespace rosidl_typesupport_fastrtps_cpp

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, @(', '.join([package_name] + list(interface_path.parents[0].parts))), @(service.namespaced_type.name))() {
  return &@('::'.join([package_name] + list(interface_path.parents[0].parts)))::typesupport_fastrtps_cpp::_@(service.namespaced_type.name)__handle;
}

#ifdef __cplusplus
}
#endif
