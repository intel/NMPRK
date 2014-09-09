/***************************************************************************
 * Copyright 2013 Intel Corporation                                        *
 * Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 * http://www.apache.org/licenses/LICENSE-2.0                              *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/

#ifndef _NMPRK_UTILITIES_H_
#define _NMPRK_UTILITIES_H_

#ifdef WIN32
#include <Windows.h>
#endif

#include "nmprkTypes.h"

void nmprkInitializeIpmiRequest(ipmi_req_t& req, byte_t netFun, byte_t cmd);
void nmprkInitializeRequest(ipmi_req_t& req, byte_t netFun, byte_t cmd);
byte_t nmprkGetByte(ulong_t value, int index);
nmprk_status_t nmprkGetNodeManagerVersion(nmprk_conn_handle_t h, byte_t *version);
//void copyRepoInfo(nm_ipmi_repo_info_t *info, nmprk::ipmi::repoInfo_t *retInfo);

#endif //_NMPRK_UTILITIES_H_

