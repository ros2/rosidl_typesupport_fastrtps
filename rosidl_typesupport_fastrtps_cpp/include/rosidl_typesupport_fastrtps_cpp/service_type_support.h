// Copyright 2014-2015 Open Source Robotics Foundation, Inc.
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

#ifndef ROSIDL_TYPESUPPORT_FASTRTPS_CPP__SERVICE_TYPE_SUPPORT_H_
#define ROSIDL_TYPESUPPORT_FASTRTPS_CPP__SERVICE_TYPE_SUPPORT_H_

#include <stdint.h>
#include <rmw/types.h>
#include "rosidl_runtime_c/service_type_support_struct.h"

#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"

/// Encapsulates the callbacks for getting properties of this rosidl type.
typedef struct service_type_support_callbacks_t
{
  /// The C++ namespace of this service.
  const char * service_namespace_;
  /// The typename of this service.
  const char * service_name_;
  /// Pointer to the request message typesupport members.
  const rosidl_message_type_support_t * request_members_;
  /// Pointer to the response message typesupport members.
  const rosidl_message_type_support_t * response_members_;
} service_type_support_callbacks_t;

#endif  // ROSIDL_TYPESUPPORT_FASTRTPS_CPP__SERVICE_TYPE_SUPPORT_H_
