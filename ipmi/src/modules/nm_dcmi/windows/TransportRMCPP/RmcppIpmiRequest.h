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


#ifndef _RMCPP_IPMI_REQUEST_H
#define _RMCPP_IPMI_REQUEST_H

#include "transport.h"
#include "RmcppTypes.h"

// forward declaration
class ByteVector;

namespace RmcppTransportPlugin
{

/// This intermediate class, useful to initiate request with the defaults.
struct InternalIpmiReq : dcmi_req_t
{
    /// Ctor.
    InternalIpmiReq();
};

/// Initiates req for Get Channel Auth Cap message.
struct IpmiGetChannelAuthCap : InternalIpmiReq
{
    IpmiGetChannelAuthCap();
};

/// Initiates req for Get DCMI Capability Info Command.
struct IpmiGetDcmiCapInfo : InternalIpmiReq
{
	IpmiGetDcmiCapInfo();
};
/// Initiates the req for Open Session message.
struct IpmiOpenSession : InternalIpmiReq
{
    IpmiOpenSession(byte_t messageTag, byte_t maxPrivLevel, const SessionId& sidm, 
                    ipmi_auth_algo authAlgo, ipmi_integ_algo integAlgo, ipmi_confid_algo confidAlgo);
};

/// Initiates the req for RAKP-1 message.
struct IpmiRakp1 : InternalIpmiReq
{
    IpmiRakp1(byte_t messageTag, const SessionId& sidc, const ByteVector& randM, byte_t roleM,
              const ByteVector& uNameM);
};

/// Initiates the req for RAKP-3 message.
struct IpmiRakp3 : InternalIpmiReq
{
    IpmiRakp3(byte_t messageTag, const SessionId& sidc, const ByteVector& digest);
};

/// Initiates the req for Disconnect message.
struct IpmiDisconnectReq : InternalIpmiReq
{
    IpmiDisconnectReq(const SessionId& sidc);
};

}

#endif
