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



#ifndef _TRANSPORT_PLUGIN_COMMON_H
#define _TRANSPORT_PLUGIN_COMMON_H

#include "transport.h"

#ifdef __cplusplus

#include <ostream>

namespace TransportPlugin
{

/**
 * Writes the content of the <code>req</code> object
 * to the specified output stream; intended for debugging purposes.
 *
 * @param os Output stream.
 * @param req Request.
 *
 * @return Reference to <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const struct dcmi_req_t& req);

/**
 * Writes the content of the <code>rsp</code> object
 * to the specified output stream. May be useful to log
 * the content of the IPMI response object; intended for debugging purposes.
 *
 * @param os Output stram.
 * @param rsp Response.
 *
 * @return Reference to <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const struct dcmi_rsp_t& rsp);

}

#endif //#ifdef __cplusplus
#endif //#ifndef _TRANSPORT_PLUGIN_COMMON_H
