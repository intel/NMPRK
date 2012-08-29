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

#include "transport.h"
#include "ByteVector.h"
#include "RmcppIpmiHeader.h"
#include <cstring>
#include <memory>
#include <stdexcept>
#include <assert.h>

using namespace std;

namespace RmcppTransportPlugin
{

IpmiSessionHeader::IpmiSessionHeader()
                 :authType(AUTH_TYPE_NONE)
{
};

IpmiSessionHeader::IpmiSessionHeader(ipmi_auth_type authType)
                 :authType(authType)
{
}

void
IpmiSessionHeader::serialize(ByteVector& out) const
{
    out += 0x06; // IPMI Version
    out += 0x00; // IPMI Reserved
    out += 0xFF; // IPMI Seq#
    out += 0x07; // IPMI class
    out += static_cast<byte_t>(authType);
}

void
IpmiSessionHeader::deserialize(IpmiSessionHeader& header, const ByteVector& in, size_t& pos)
{
    if (in[pos++] != 0x06) //IPMI Version
        throw runtime_error("Incorrect IPMI Version");
    if (in[pos++] != 0x00) // IPMI reserved
        throw runtime_error("2nd byte expected to be 0x00");
    if (in[pos++] != 0xFF) // IPMI Seq#
        throw runtime_error("Incorrect IPMI Seq#");
    if (in[pos++] != 0x07) // IPMI class
        throw runtime_error("Incorrect IPMI class");
    header.authType = static_cast<ipmi_auth_type>(in[pos++]);
}


RmcppSessionHeader::RmcppSessionHeader()
{
}

RmcppSessionHeader::RmcppSessionHeader(const ipmi_payload_t& payload)
                  :IpmiSessionHeader(AUTH_TYPE_RMCPP)
{
    this->payload = payload;
    // empty session id and session seq
    memset(sidc, 0, sizeof(SessionId));
    memset(managedSequence, 0, sizeof(Sequence));
}

RmcppSessionHeader::RmcppSessionHeader(const ipmi_payload_t& payload, const SessionId& sidc, 
                                               const Sequence& managedSequence)
                  :IpmiSessionHeader(AUTH_TYPE_RMCPP)
{
    this->payload = payload;
    memcpy(this->sidc, sidc, sizeof(SessionId));
    memcpy(this->managedSequence, managedSequence, sizeof(Sequence));
}

void
RmcppSessionHeader::serialize(ByteVector& out) const
{
    IpmiSessionHeader::serialize(out);
    out += payload.value;
    out.append(sidc, sizeof(SessionId));
    out.append(managedSequence, sizeof(Sequence));
}

void
RmcppSessionHeader::deserialize(RmcppSessionHeader& header, const ByteVector& in, size_t& pos)
{
    IpmiSessionHeader::deserialize(header, in, pos);
    header.payload.value = in[pos++];
    memcpy(header.sidc, &in.at(pos), sizeof(SessionId));
    pos += sizeof(SessionId);
    memcpy(header.managedSequence, &in.at(pos), sizeof(Sequence));
    pos += sizeof(Sequence);
}

}
