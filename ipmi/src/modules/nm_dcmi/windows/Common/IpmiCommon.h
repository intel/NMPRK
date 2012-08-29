/***************************************************************************
 * Copyright 2012 Intel Corporation                                        *
 *Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 * http://www.apache.org/licenses/LICENSE-2.0                              *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/
/**************************************************************************
 * Author: Stewart Dale <IASI NM TEAM>                                    *
 * Updates:							                                      *
 * 4/30: prep for initial external release                                *
 **************************************************************************/
#ifndef _IPMI_COMMON_H
#define _IPMI_COMMON_H

#include "transport.h"
#include "Logger.h"

typedef int IPMI_STATUS;

#define IPMI_SUCCESS 0
#define IPMI_FAILURE 1
#define IPMI_PLUGIN_LOAD_ERROR 2
#define IPMI_CONNECT_ERROR 3
#define IPMI_DISCONNECT_ERROR 4
#define IPMI_SEND_REQUEST_ERROR 5
#define IPMI_INDEX_OUT_OF_RANGE 6

#define IPMI_NETWORK_FUNCTION_REQ_CHASSIS	0x00
#define IPMI_NETWORK_FUNCTION_REQ_BRIDGE	0x02
#define IPMI_NETWORK_FUNCTION_REQ_SENSOR	0x04
#define IPMI_NETWORK_FUNCTION_REQ_APP		0x06
#define IPMI_NETWORK_FUNCTION_REQ_FIRMWARE	0x08
#define IPMI_NETWORK_FUNCTION_REQ_STORAGE	0x0A
#define IPMI_NETWORK_FUNCTION_REQ_TRANSPORT	0x0C

#endif //_IPMI_COMMON_H
