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


#include <cstring>
#include "RmcppIpmiRequest.h"
#include "ByteVector.h"

#include <assert.h>
#include <stdexcept>

using namespace std;

namespace RmcppTransportPlugin
{

InternalIpmiReq::InternalIpmiReq()
{
    netFun = 0x06;
    cmd = 0x00;
    len = 0x00;
    rsAddr = 0x20;
    rsLun = 0x00;
    memset(data, 0x00, sizeof(data));
}

IpmiGetChannelAuthCap::IpmiGetChannelAuthCap()
{
	netFun = 0x06;
    cmd = 0x38;
    data[0] = 0x8e; //channel number (this channel) + retr IPMI 2.0 caps
    data[1] = 0x04; //max priv level (4=admin)
    len = 2;
}

IpmiGetDcmiCapInfo::IpmiGetDcmiCapInfo()
{
	// Issuing GetDCMICapabilityInfo Command for Parameter 1 only
	// for command execution successful or not.
	netFun = 0x2C;
	cmd = 0x01;
	data[0] = 0xDC;
	data[1] = 0x01;
	len = 2;
}
IpmiOpenSession::IpmiOpenSession(byte_t messageTag, byte_t maxPrivLevel, const SessionId& sidm,
                                 ipmi_auth_algo authAlgo, ipmi_integ_algo integAlgo, ipmi_confid_algo confidAlgo)
{
    data[len++] = messageTag;
    data[len++] = maxPrivLevel;
    len += 2; //reserved 00_00h
    memcpy(&data[len], sidm, 4);
    len += 4;

    //Authentication Payload
    data[len++] = 0x00; //type 00 = Auth payload Type
    len += 2; //reserved
    data[len++] = 0x08; //Payload Length
    data[len++] = static_cast<byte_t>(authAlgo);
    len +=3; //reserved

    //Integrity Payload
    data[len++] = 0x01; //type 01 = Integrity algorithm
    len += 2; //reserved
    data[len++] = 0x08; //Payload Length
    data[len++] = static_cast<byte_t>(integAlgo);
    len +=3; //reserved

    //Confidentiality Payload
    data[len++] = 0x02; //type 02 = Confidentiality algorithm
    len += 2; //reserved
    data[len++] = 0x08; //Payload Length
    data[len++] = static_cast<byte_t>(confidAlgo);
    len +=3; //reserved
}

IpmiRakp1::IpmiRakp1(byte_t messageTag, const SessionId& sidc, const ByteVector& randM,
                     byte_t roleM, const ByteVector& uNameM)
{
    data[len++] = messageTag;
    len += 3; //reserved 00_00_00h
    memcpy(&data[len], sidc, 4); //managed Session ID 
    len += 4;

    randM.copyto(&data[len], randM.length());
    len += 16;

    data[len++] = roleM;
    len += 2; //reserved 00_00h

    //user name length, 0h = null user name
    data[len++] = static_cast<byte_t>(uNameM.length());
    if (uNameM.length() > 16)
    {
        throw runtime_error("Rakp1: Username too long (> 16)");
    }
    uNameM.copyto(&data[len], uNameM.length());
    //len += 16;
    len += uNameM.length();
}

IpmiRakp3::IpmiRakp3(byte_t messageTag, const SessionId& sidc, const ByteVector& digest)
{
    data[len++] = messageTag; //Message Tag, match response with request?
    //len += 3; //reserved 00_00_00h
    data[len++] = 0x00; //status code
    len += 2; //reserved 00_00h
    memcpy(&data[len], sidc, 4);//managed Session ID 
    len += 4;

    digest.copyto(&data[len], digest.length());
    len += digest.length();
}

IpmiDisconnectReq::IpmiDisconnectReq(const SessionId& sidc)
{
    cmd = 0x3C;
    len = 4;
    memcpy(data, &sidc, 4);
}

}
