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
#include "IpmbRequest.h"

int BridgedIpmiRequest::sequenceNumber_ = -1;

BridgedIpmiRequest::BridgedIpmiRequest(byte_t channel, IpmiRequest& eReq, std::string bridgedId, bool lan)
: IpmiRequest(0x06, 0x34, 0x20, 0x00, eReq.getId())
, encapReq_ (eReq)
{
    initializeBridgedIpmiRequest(channel, lan, eReq.getNetFunction(), eReq.getCommand(), eReq.getResponderAddr(), eReq.getResponderLun(), bridgedId);
    for (int i = 0; i < eReq.getDataLength(); i++)
        addDataByte(eReq.getDataByte(i));
}

//BridgedIpmiRequest::BridgedIpmiRequest(byte_t rsAddr, byte_t rsLun, byte_t channel, byte_t encapNetFn, byte_t encapCmd, byte_t encapRsAddr, byte_t encapRsLun, std::string bridgedId)

BridgedIpmiRequest::BridgedIpmiRequest(byte_t rsAddr, byte_t rsLun, byte_t channel, IpmiRequest& eReq, std::string bridgedId, bool lan)
: IpmiRequest(0x06, 0x34, rsAddr, rsLun, eReq.getId())
, encapReq_ (eReq)
{
    initializeBridgedIpmiRequest(channel, lan, eReq.getNetFunction(), eReq.getCommand(), eReq.getResponderAddr(), eReq.getResponderLun(), bridgedId);
    for (int i = 0; i < eReq.getDataLength(); i++)
        addDataByte(eReq.getDataByte(i));
}

//BridgedIpmiRequest::BridgedIpmiRequest(byte_t netFn, byte_t cmd, byte_t rsAddr, byte_t rsLun, byte_t channel, byte_t encapNetFn, byte_t encapCmd, byte_t encapRsAddr, byte_t encapRsLun, std::string bridgedId)

BridgedIpmiRequest::BridgedIpmiRequest(byte_t netFn, byte_t cmd, byte_t rsAddr, byte_t rsLun, byte_t channel, IpmiRequest& eReq, std::string bridgedId, bool lan)
: IpmiRequest(netFn, cmd, rsAddr, rsLun, eReq.getId())
, encapReq_ (eReq)
{
    initializeBridgedIpmiRequest(channel, lan, eReq.getNetFunction(), eReq.getCommand(), eReq.getResponderAddr(), eReq.getResponderLun(), bridgedId);
    for (int i = 0; i < eReq.getDataLength(); i++)
        addDataByte(eReq.getDataByte(i));
}

void BridgedIpmiRequest::initializeBridgedIpmiRequest(byte_t channel, bool lan, byte_t encapNetFn, byte_t encapCmd, byte_t encapRsAddr, byte_t encapRsLun, std::string bridgedId)
{
    checksumAdded_ = false;
    bridgedId_ = bridgedId;

    byte_t chk1 = 0;

    // The tracking bit used to be set to 1 all the time, but Dell didn't like it so it was changed to 0.  Now only set it when requested (non-KCS)
    // Byte 0 - 0000 0110 (00 = track request, 0 = encryption, 0 = authentication, 0110 = channel)
    if (lan)
        addDataByte(0x40 | (channel & 0x0F));
    else
        addDataByte(0x00 | (channel & 0x0F));

    // Byte 1 - Target Address
    byte_t hdr1 = encapRsAddr;
    chk1 += hdr1 % 256;
    addDataByte(hdr1);

    // Byte 2 - 1011 1000 (101110 = NetFn, 00 = Target LUN)
    byte_t hdr2 = ((encapNetFn << 2) & 0xFC) | (encapRsLun & 0x03);
    chk1 += hdr2 % 256;
    addDataByte(hdr2);

    // Byte 3 - Checksum
    addDataByte(-chk1);

    // Byte 4 - Source Address
    addDataByte(0x20);

    // increment sequence number
    sequenceNumber_ = (sequenceNumber_ + 1) % 0x3F;

    // Byte 5 - XXXX XX00 (XXXXXX = Seq Number, 10 = Source LUN)
	// The Source LUN used to always be set to 0x02, but SuperMicro had a problem with this over the lan
	// Now we set it to 0x00 when using a lan connection
	if(lan)
		addDataByte(((sequenceNumber_ << 2) & 0xFC) | 0x00);
	else
		addDataByte(((sequenceNumber_ << 2) & 0xFC) | 0x02);

    // Byte 6 - Command
    addDataByte(encapCmd);

    isBridged_ = true;
}

const dcmi_req_t& BridgedIpmiRequest::getRequestStruct()
{
    byte_t chk2 = 0;

    int dataLength = getDataLength();
    if (checksumAdded_ == true)
        dataLength -= 1;

    for (int i = 4; i < dataLength; i++)
    {
        byte_t byte = 0;
        if (getDataByte(i, byte) == IPMI_SUCCESS)
            chk2 += byte % 256;
    }
    chk2 = -chk2;

    if (checksumAdded_ == true)
    {
        setDataByte(getDataLength() - 1, chk2);
    }
    else
    {
        addDataByte(chk2);
        checksumAdded_ = true;
    }

    return IpmiRequest::getRequestStruct();
}
