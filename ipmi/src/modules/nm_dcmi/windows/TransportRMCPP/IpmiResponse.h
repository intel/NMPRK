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


#ifndef _RMCPP_IPMI_RESPONSE_H
#define _RMCPP_IPMI_RESPONSE_H

#include "RmcppTypes.h"

namespace RmcppTransportPlugin
{

// Following structures as per IPMI 2.0 spec.

#pragma pack(1)

struct get_channel_auth_cap_response
{
    byte_t channelNumber;
    union
    {
        byte_t value;
        struct
        {
            byte_t iolNone      : 1;
            byte_t iolMd2       : 1;
            byte_t iolMd5       : 1;
            byte_t reserved1    : 1;
            byte_t iolStraight  : 1;
            byte_t oemProp      : 1;
            byte_t reserved2    : 1;
            byte_t ipmi2Sup     : 1;
        } b;
    } authTypeSupport;
    union
    {
        byte_t value;
        struct
        {
            byte_t anonymousLoginEnabled    : 1;
            byte_t nullUsernamesEnabled     : 1;
            byte_t nonNullUsernamesEnabled  : 1;
            byte_t userLevelAuthDisabled    : 1;
            byte_t perMsgAuthDisabled       : 1;
            byte_t kgSetToNonZero           : 1;
            byte_t reserved1                : 2;
        } b;
    } authStatus;
    union
    {
        byte_t value;
        struct
        {
            byte_t ipmi15Supported  : 1;
            byte_t ipmi20Supported  : 1;
            byte_t reserved1        : 6;
        } b;
    } extendedCaps;
};

struct rmcpp_os_payload
{
    byte_t type;
    byte_t reserved1[2];
    byte_t length;
    byte_t algorithm;
    byte_t reserved2[3];
};

struct rmcpp_open_session_response
{
    byte_t messageTag;
    byte_t statusCode;
    byte_t maxPrivLevel;
    byte_t reserved1;
    SessionId sidm;
    SessionId sidc;
    rmcpp_os_payload authPayload;
    rmcpp_os_payload integPayload;
    rmcpp_os_payload confidPayload;
};

#pragma warning(disable:4200)
struct rmcpp_rakp2
{
    byte_t messageTag;
    byte_t statusCode;
    byte_t reserved1[2];
    SessionId sidm;
    byte_t randC[16];
    Guid guidC;
    // keyExAuthCode may be 0 or 16 or 20 depending on rakp auth algo
    byte_t keyExAuthCode[0];
};

struct rmcpp_rakp4
{
    byte_t messageTag;
    byte_t statusCode;
    byte_t reserved2[2];
    SessionId sidm;
    // integCheckValue size depends on auth algo
    byte_t integCheckValue[0];
};
#pragma warning(default:4200)

#pragma pack()

}

#endif

