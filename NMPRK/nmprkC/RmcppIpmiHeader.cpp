/*******************************************************************************
********************************************************************************
***                                                                           **
***							INTEL CONFIDENTIAL								  **
***                    COPYRIGHT 2008 INTEL CORPORATION                       **
***							All Rights Reserved								  **
***                                                                           **
***                INTEL CORPORATION PROPRIETARY INFORMATION                  **
***                                                                           **
***		The source code contained or described herein and all documents		  **
***		related to the source code ("Material") are owned by Intel			  **
***		Corporation or its suppliers or licensors. Title to the Material	  **
***		remains with Intel Corporation or its suppliers and licensors.		  **
***		The Material contains trade secrets and proprietary and confidential  **
***		information of Intel or its suppliers and licensors.				  **
***		The Material is protected by worldwide copyright and trade secret	  **
***		laws and treaty provisions. No part of the Material may be used,	  **
***		copied, reproduced, modified, published, uploaded, posted,			  **
***		transmitted, distributed, or disclosed in any way without Intel’s	  **
***		prior express written permission.									  **
***																			  **
***		No license under any patent, copyright, trade secret or other		  **
***		intellectual property right is granted to or conferred upon you by	  **
***		disclosure or delivery of the Materials, either expressly, by		  **
***		implication, inducement, estoppel or otherwise. Any license under	  **
***		such intellectual property rights must be express and approved by	  **
***		Intel in writing.													  **
********************************************************************************/
#include "stdafx.h"

#include "ByteVector.h"
#include "RmcppIpmiHeader.h"
#include <cstring>
#include <memory>
#include <stdexcept>
#include <assert.h>

using namespace std;

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
