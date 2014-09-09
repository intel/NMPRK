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

#include "RmcppIpmiPayload.h"
#include "CryptoProxy.h"

#include <stdexcept>

using namespace std;

void
IpmiPayload::serializeRmcpp(ByteVector& out, const IpmiPayloadData& payload)
{
    // 2-bytes length
    out += static_cast<byte_t>(payload.data.length() & 0xff);
    out += static_cast<byte_t>((payload.data.length() >> 8) & 0xff);
    out.append(payload.data);
}

void
IpmiPayload::deserializeRmcpp(ByteVector& outData, const ByteVector& in, size_t& pos)
{
    // get 2-bytes length
    byte_t lsb = in[pos++];
    byte_t msb = in[pos++];
    short int length = (msb << 8) + lsb;
    outData.copy(in.c_ptr(static_cast<int>(pos)), length);
    pos += length;
}

void 
IpmiPayload::serializeEncryptedRmcpp(ByteVector& out, const ByteVector& key, const IpmiPayloadData& payload)
{
    IpmiPayloadData cryptedPayload;
    // generate random IV
    ByteVector iv;
    CryptoProxy::getInstance()->genRand(16, iv);
    
    // calculate padding length
    byte_t padLength = (16 - static_cast<byte_t>((payload.data.length() + 1/*for padLength field*/) % 16)) % 16;
    ByteVector payloadToCrypt(payload.data);
    
    // append padding
    for (byte_t i = 1; i <= padLength; ++i)
    {
        payloadToCrypt += i;
    }
    // append padding length
    payloadToCrypt += padLength;
    
    // crypt payload
    CryptoProxy::getInstance()->encrypt(key, iv, payloadToCrypt, cryptedPayload.data);
    
    // prefix crypted payload with IV
    cryptedPayload.data.insert(cryptedPayload.data.begin(), iv.begin(), iv.end());
    // serialize payload
    serializeRmcpp(out, cryptedPayload);
}

void 
IpmiPayload::deserializeEncryptedRmcpp(ByteVector& outData, const ByteVector& key, const ByteVector& in, size_t& pos)
{
    // deserialize payload
    deserializeRmcpp(outData, in, pos);
    // strip IV from payload
    ByteVector iv(outData.c_ptr(), 16);

    outData.erase(outData.begin(), outData.begin() + 16);
    // decrypt payload
    ByteVector decryptedPayload;
    CryptoProxy::getInstance()->decrypt(key, iv, outData, decryptedPayload);
    // verify padding
    byte_t padLength = decryptedPayload[decryptedPayload.length() - 1];
    const byte_t* pad = decryptedPayload.c_ptr(decryptedPayload.length() - padLength -1);
    for (byte_t i = 1; i <= padLength; ++i, ++pad)
    {
        if (*pad != i)
            throw runtime_error("Incorrect padding on incoming encrypted message");
    }
    // strip padding
    outData.assign(decryptedPayload.begin(), decryptedPayload.end() - padLength -1);
}

IpmiPayloadDataIpmiMsg::IpmiPayloadDataIpmiMsg()
{
}

IpmiPayloadDataIpmiMsg::IpmiPayloadDataIpmiMsg(const ipmi_req_t& req, 
                                               byte_t rqAddr, byte_t rqSeq, byte_t rqLun)
{
    data += req.rsAddr;
    data += (req.netFun<<2) + req.rsLun;
    data += checksum1(req);
    data += rqAddr;
    data += rqLun; //rqSeq;
    data += req.cmd;
    data.append(req.data,(int) req.len);
    data += checksum2(req, rqAddr, rqSeq, rqLun);
}

byte_t
IpmiPayloadDataIpmiMsg::checksum1(const ipmi_req_t& req)
{
    return static_cast<byte_t>(~(req.rsAddr + (req.netFun<<2) + req.rsLun) + 1);
}

byte_t
IpmiPayloadDataIpmiMsg::checksum2(const ipmi_req_t& req, byte_t rqAddr, byte_t rqSeq, byte_t rqLun)
{
    byte_t cs2 = rqAddr + (rqSeq<<2) + rqLun + req.cmd;
    for (size_t i = 0; (i < req.len) & (i < MAX_IPMI_SIZE); i++)
    {
        cs2 = cs2 + req.data[i];
    }
    cs2 = ~cs2 + 1;
    return cs2;
}

IpmiPayloadDataSolData::IpmiPayloadDataSolData(const ipmi_req_t& req)
{
	data.copy(req.data,(int)req.len);
}

IpmiPayloadDataSolData::IpmiPayloadDataSolData()
{
}
