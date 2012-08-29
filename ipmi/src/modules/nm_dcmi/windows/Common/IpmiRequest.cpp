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
#include "IpmiRequest.h"
#include "Logger.h"
#include <iomanip>

/*
IpmiRequest::IpmiRequest()
{
        req_.cmd = 0x00;
        req_.len = 0;
        req_.netFun = 0x00;
        req_.rsAddr = 0x20;
        req_.rsLun = 0x00;
        dataIndex_ = 0;
}
 */
IpmiRequest::IpmiRequest(byte_t netFn, byte_t cmd, byte_t rsAddr, byte_t rsLun, std::string id/*, std::string eId*/)
: isBridged_(false)
{
    id_ = id;
    req_.cmd = cmd;
    req_.len = 0;
    req_.netFun = netFn;
    req_.rsAddr = rsAddr;
    req_.rsLun = rsLun;
}

/*
void IpmiRequest::setNetFunction(byte_t b)
{
        req_.netFun = b;
}

void IpmiRequest::setCommand(byte_t b)
{
        req_.cmd = b;
}

void IpmiRequest::setResponderAddr(byte_t b)
{
        req_.rsAddr = b;
}

void IpmiRequest::setResponderLun(byte_t b)
{
        req_.rsLun = b;
}
 */
void IpmiRequest::addDataByte(byte_t b)
{
    if (req_.len < MAX_IPMI_SIZE)
    {
        req_.data[req_.len] = b;
        req_.len++;
    }
}

byte_t IpmiRequest::getDataByte(int idx)
{
    if (idx >= 0 && idx < (int) req_.len)
        return req_.data[idx];
    else
        return 0;
}

IPMI_STATUS IpmiRequest::getDataByte(int idx, byte_t &b)
{
    if (idx >= 0 && idx < (int) req_.len)
    {
        b = req_.data[idx];
        return IPMI_SUCCESS;
    }
    else
    {
        return IPMI_INDEX_OUT_OF_RANGE;
    }
}

IPMI_STATUS IpmiRequest::setDataByte(int idx, byte_t b)
{
    if (idx >= 0 && idx < (int) req_.len)
    {
        req_.data[idx] = b;
        return IPMI_SUCCESS;
    }
    else
    {
        return IPMI_INDEX_OUT_OF_RANGE;
    }
}

void PrintIpmiRequestToLog(IpmiRequest *req)
{
    LOG_LODEBUG << "Request: " << std::endl;
    LOG_LODEBUG << "  NetFn: 0x" << std::setw(2) << std::setfill('0') << std::hex << (int) req->getNetFunction() << std::endl;
    LOG_LODEBUG << "  Command: 0x" << std::setw(2) << std::setfill('0') << std::hex << (int) req->getCommand() << std::endl;
    LOG_LODEBUG << "  RsAddr: 0x" << std::setw(2) << std::setfill('0') << std::hex << (int) req->getResponderAddr() << std::endl;
    LOG_LODEBUG << "  RsLUN: 0x" << std::setw(2) << std::setfill('0') << std::hex << (int) req->getResponderLun() << std::endl;

    for (int i = 0; i < req->getDataLength(); i++)
    {
        LOG_LODEBUG << "  DataByte " << std::setw(3) << std::setfill(' ') << std::dec << i << ": 0x" << std::setw(2) << std::setfill('0') << std::hex << (int) req->getDataByte(i) << std::endl;
    }

}
