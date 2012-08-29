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


#include "RmcppIpmiPayload.h"
#include "CryptoProxy.h"
#include "RemoteLogger.h"

#include <stdexcept>

using namespace std;

namespace RmcppTransportPlugin
{

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
    LOG_LODEBUG << "IV: " << iv;
    
    // calculate padding length
    byte_t padLength = (16 - static_cast<byte_t>((payload.data.length() + 1/*for padLength field*/) % 16)) % 16;
    ByteVector payloadToCrypt(payload.data);
    LOG_LODEBUG << "Payload to crypt: " << payloadToCrypt;
    
    // append padding
    for (byte_t i = 1; i <= padLength; ++i)
    {
        payloadToCrypt += i;
    }
    // append padding length
    payloadToCrypt += padLength;
    LOG_LODEBUG << "Payload to crypt with padding: " << payloadToCrypt;
    
    // crypt payload
    CryptoProxy::getInstance()->encrypt(key, iv, payloadToCrypt, cryptedPayload.data);
    LOG_LODEBUG << "Crypted payload: " << cryptedPayload.data;
    
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
    LOG_LODEBUG << "IV: " << iv;

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

IpmiPayloadDataIpmiMsg::IpmiPayloadDataIpmiMsg(const dcmi_req_t& req, 
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
IpmiPayloadDataIpmiMsg::checksum1(const dcmi_req_t& req)
{
    return static_cast<byte_t>(~(req.rsAddr + (req.netFun<<2) + req.rsLun) + 1);
}

byte_t
IpmiPayloadDataIpmiMsg::checksum2(const dcmi_req_t& req, byte_t rqAddr, byte_t rqSeq, byte_t rqLun)
{
    byte_t cs2 = rqAddr + (rqSeq<<2) + rqLun + req.cmd;
    for (size_t i = 0; (i < req.len) & (i < MAX_IPMI_SIZE); i++)
    {
        cs2 = cs2 + req.data[i];
    }
    cs2 = ~cs2 + 1;
    return cs2;
}

IpmiPayloadDataSolData::IpmiPayloadDataSolData(const dcmi_req_t& req)
{
	data.copy(req.data,(int)req.len);
}

IpmiPayloadDataSolData::IpmiPayloadDataSolData()
{
}

}
