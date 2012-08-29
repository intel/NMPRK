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
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ipmi.h>

#include "IpmiProxyLinux.h"
#include "KcsIpmiRequest.h"
#include "KcsIpmiResponse.h"
#include "CExcept.h"

using namespace std;

bool IpmiProxy::GetIpmiProxyInstance(IpmiProxy **proxy, std::string& errMsg)
{
    *proxy = new OpenIpmiDriverIpmiProxy;
    return true;
}

OpenIpmiDriverIpmiProxy::OpenIpmiDriverIpmiProxy(const char* ipmiDevice)
{
    this->ipmiReqMsgId = -1;

    ipmiDrvHandle = open(ipmiDevice, O_RDWR);
    if (ipmiDrvHandle < 0)
    {
        throw CError("Cannot open IPMI device");
    }

    int events = 0;
    if (ioctl(ipmiDrvHandle, IPMICTL_SET_GETS_EVENTS_CMD, &events) < 0)
    {
        throw CError("Cannot configure events");
    }

    unsigned int bmcSlaveAddr = 0x20;
    if (ioctl(ipmiDrvHandle, IPMICTL_SET_MY_ADDRESS_CMD, &bmcSlaveAddr) < 0)
    {
        throw CError("Cannot open IPMI device");
    }
}

OpenIpmiDriverIpmiProxy::~OpenIpmiDriverIpmiProxy()
{
    if (ipmiDrvHandle >= 0)
    {
        close(ipmiDrvHandle);
    }
}

void OpenIpmiDriverIpmiProxy::request(const KcsIpmiRequest& req)
{
    if (ipmiDrvHandle < 0)
    {
        throw CError("Driver handle error");
    }

    struct ipmi_system_interface_addr bmcAddr;
    memset(&bmcAddr, 0, sizeof(bmcAddr));
    bmcAddr.lun = req.getLun();
    bmcAddr.addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE;
    bmcAddr.channel = IPMI_BMC_CHANNEL;

    struct ipmi_req ipmiDrvRequest;
    memset(&ipmiDrvRequest, 0, sizeof(ipmiDrvRequest));
    ipmiDrvRequest.addr = (unsigned char*)&bmcAddr;
    ipmiDrvRequest.addr_len = sizeof(bmcAddr);
    ipmiDrvRequest.msgid = ++ipmiReqMsgId;

    ipmiDrvRequest.msg.netfn = req.getNetFn();
    ipmiDrvRequest.msg.cmd = req.getCmd();

    std::vector<unsigned char> data = req.getData();
    ipmiDrvRequest.msg.data_len = data.size();
    ipmiDrvRequest.msg.data = new unsigned char[data.size()];
    for (unsigned int idx = 0; idx < data.size(); idx++)
    {
        ipmiDrvRequest.msg.data[idx] = data[idx];
    }

    if (ioctl(ipmiDrvHandle, IPMICTL_SEND_COMMAND, &ipmiDrvRequest) < 0)
    {
        throw CError("Cannot sendIPMI command");
    }
    delete ipmiDrvRequest.msg.data;
}

KcsIpmiResponse OpenIpmiDriverIpmiProxy::response()
{
    if (ipmiDrvHandle < 0)
    {
        throw CError("Driver handle error");
    }

    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(ipmiDrvHandle, &descriptors);

    struct timeval waitTimeout;
    waitTimeout.tv_sec = 60;
    waitTimeout.tv_usec = 00;

    if (select(ipmiDrvHandle+1, &descriptors, NULL, NULL, &waitTimeout) < 0)
    {
        throw CError("Failed to wait for IPMI response");
    }

    if (!FD_ISSET(ipmiDrvHandle, &descriptors))
    {
        throw CError("Timeout waiting for IPMI response", 0);
    }

    struct ipmi_recv ipmiDrvResp;
    struct ipmi_addr ipmiDrvRespAddr;
    unsigned char respDataBuffer[1024]; //TBD: Replace 1024 with some constant.

    ipmiDrvResp.addr = (unsigned char *) &ipmiDrvRespAddr;
    ipmiDrvResp.addr_len = sizeof(ipmiDrvRespAddr);
    ipmiDrvResp.msg.data = respDataBuffer;
    ipmiDrvResp.msg.data_len = sizeof(respDataBuffer);

    if (ioctl(ipmiDrvHandle, IPMICTL_RECEIVE_MSG_TRUNC, &ipmiDrvResp) < 0)
    {
        throw CError("Error reading IPMI response");
    }

    KcsIpmiResponse resp(ipmiDrvResp.msg.data[0], ipmiDrvResp.msg.data_len, respDataBuffer, true);
    return resp;
}

KcsIpmiResponse
OpenIpmiDriverIpmiProxy::bridgedRequestResponse(const KcsIpmiRequest& req)
{
    struct ipmi_req ipmiDrvRequest;
    memset(&ipmiDrvRequest, 0, sizeof(ipmiDrvRequest));

    if(req.getNetFn() == 0x06 && (req.getCmd() == 0x34))
    {
        sendMessageRequestChannel_ = req.getData()[0] & 0x0F;
        sendMessageRequestNetFn_ = (req.getData()[2] >> 2) & 0x3F;
        sendMessageRequestLun_ = req.getData()[2] & 0x03;
        sendMessageRequestCmd_ = req.getData()[6];
        sendMessageRequestSlaveAddr_  = req.getData()[1];
        sendMessageRequestData_ = req.getData();

        if(req.getData().size() < 8)
        {
            throw CError("Data size of Send Message cmd less than expected!");
        }

        struct ipmi_ipmb_addr ipmbAddr;
        ipmbAddr.addr_type = IPMI_IPMB_ADDR_TYPE;
        ipmbAddr.channel = req.getData()[0] & 0x0F;
        ipmbAddr.slave_addr = req.getData()[1];
        ipmbAddr.lun = req.getData()[2] & 0x03;

        ipmiDrvRequest.addr = (unsigned char*)&ipmbAddr;
        ipmiDrvRequest.addr_len = sizeof(ipmbAddr);

        ipmiDrvRequest.msg.netfn = (req.getData()[2] >> 2) & 0x3F;
        ipmiDrvRequest.msg.cmd = req.getData()[6];
        ipmiDrvRequest.msgid = ++ipmiReqMsgId;

        std::vector<unsigned char> data = req.getData();
        ipmiDrvRequest.msg.data_len = data.size() - 8;
        ipmiDrvRequest.msg.data = new unsigned char[data.size() - 8];

        for (unsigned int idx = 7; idx < data.size()-1; idx++)
        {
            ipmiDrvRequest.msg.data[idx-7] = data[idx];
        }

        /*
        printf("ipmbAddr.addr_type = 0x%x\n", ipmbAddr.addr_type);
        printf("ipmbAddr.channel = 0x%x\n", ipmbAddr.channel);
        printf("ipmbAddr.lun = 0x%x\n", ipmbAddr.lun);
        printf("ipmbAddr.slave_addr = 0x%x\n", ipmbAddr.slave_addr);

        printf("ipmiDrvRequest.msg.netfn = 0x%x\n", ipmiDrvRequest.msg.netfn);
        printf("ipmiDrvRequest.msg.cmd = 0x%x\n", ipmiDrvRequest.msg.cmd);

        int i = 0;
        for(i = 0; i < ipmiDrvRequest.msg.data_len; i++)
        {
            printf("ipmiDrvRequest.msg.data[%d] = 0x%x\n", i, ipmiDrvRequest.msg.data[i]);
        }
        */
        if (ioctl(ipmiDrvHandle, IPMICTL_SEND_COMMAND, &ipmiDrvRequest) < 0)
        {
            throw CError("Cannot sendIPMI command");
        }
        delete ipmiDrvRequest.msg.data;

        KcsIpmiResponse resp = response();

        getMessageCompletionCode_ = resp.getCompletionCode();
        getMessageResponseData_ = resp.getData();
        getMessageResponseValid_ = true;

        KcsIpmiResponse sendMessageResponse(0x00, 0, NULL, true);
        return sendMessageResponse;
    }
    else if(req.getNetFn() == 0x06 && (req.getCmd() == 0x33))
    {
        if(getMessageResponseValid_ == true)
        {
            getMessageResponseValid_ = false;

            std::vector<unsigned char> respData;
            respData.push_back(0x00); // This is needed becuase the KcsIpmiResponse constructor strips on one byte for completion code
            respData.push_back(sendMessageRequestChannel_); // channel
            respData.push_back(((sendMessageRequestNetFn_ | 1) << 2) | 0x02); // netFn/rsLUN
            respData.push_back(-(0x20 + respData[2])); // chk1
            respData.push_back(sendMessageRequestSlaveAddr_); // rsSA
            respData.push_back(ipmiReqMsgId << 2); // rqSeq/rsLUN
            respData.push_back(sendMessageRequestCmd_); // cmd
            respData.push_back(getMessageCompletionCode_); // comp code
            unsigned char chk2 = respData[4] + respData[5] + respData[6] + respData[7];
            for(int i = 0; i < getMessageResponseData_.size(); i++)
            {
                chk2 += getMessageResponseData_[i];
                respData.push_back(getMessageResponseData_[i]);
            }
            respData.push_back(-chk2); // chk2
            return KcsIpmiResponse(0x00, respData.size(), respData.data(), true);
        }
        else
        {
            return KcsIpmiResponse(0x80, 0, 0, true);
        }
    }
    else
    {
        // error
    }
}

KcsIpmiResponse
OpenIpmiDriverIpmiProxy::requestResponse(const KcsIpmiRequest& req)
{
    if (ipmiDrvHandle < 0)
    {
        throw CError("Driver handle error");
    }

    if(req.getNetFn() == 0x06 && (req.getCmd() == 0x33 || req.getCmd() == 0x34))
    {
        // Bridged
        return bridgedRequestResponse(req);
    }
    else
    {
        // Simple
        request(req);
        return response();
    }

#if 0
    if(req.getNetFn() == 0x06 && (req.getCmd() == 0x33 || req.getCmd() == 0x34))
    {
        //return bridgedRequestResponse(req);
    }

    struct ipmi_system_interface_addr bmcAddr;
    memset(&bmcAddr, 0, sizeof(bmcAddr));
    bmcAddr.lun = req.getLun();
    bmcAddr.addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE;
    bmcAddr.channel = IPMI_BMC_CHANNEL;

    struct ipmi_req ipmiDrvRequest;
    memset(&ipmiDrvRequest, 0, sizeof(ipmiDrvRequest));
    ipmiDrvRequest.addr = (unsigned char*)&bmcAddr;
    ipmiDrvRequest.addr_len = sizeof(bmcAddr);
    ipmiDrvRequest.msgid = ipmiReqMsgId++;

    ipmiDrvRequest.msg.netfn = req.getNetFn();
    ipmiDrvRequest.msg.cmd = req.getCmd();

    std::vector<unsigned char> data = req.getData();
    ipmiDrvRequest.msg.data_len = data.size();
    ipmiDrvRequest.msg.data = new unsigned char[data.size()];
    for (unsigned int idx = 0; idx < data.size(); idx++)
    {
        ipmiDrvRequest.msg.data[idx] = data[idx];
    }

    if (ioctl(ipmiDrvHandle, IPMICTL_SEND_COMMAND, &ipmiDrvRequest) < 0)
    {
        throw CError("Cannot sendIPMI command");
    }
    delete ipmiDrvRequest.msg.data;

    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(ipmiDrvHandle, &descriptors);

    struct timeval waitTimeout;
    waitTimeout.tv_sec = 60;
    waitTimeout.tv_usec = 00;

    if (select(ipmiDrvHandle+1, &descriptors, NULL, NULL, &waitTimeout) < 0)
    {
        throw CError("Failed to wait for IPMI response");
    }

    if (!FD_ISSET(ipmiDrvHandle, &descriptors))
    {
        throw CError("Timeout waiting for IPMI response", 0);
    }

    struct ipmi_recv ipmiDrvResp;
    struct ipmi_addr ipmiDrvRespAddr;
    unsigned char respDataBuffer[1024]; //TBD: Replace 1024 with some constant.

    ipmiDrvResp.addr = (unsigned char *) &ipmiDrvRespAddr;
    ipmiDrvResp.addr_len = sizeof(ipmiDrvRespAddr);
    ipmiDrvResp.msg.data = respDataBuffer;
    ipmiDrvResp.msg.data_len = sizeof(respDataBuffer);

    if (ioctl(ipmiDrvHandle, IPMICTL_RECEIVE_MSG_TRUNC, &ipmiDrvResp) < 0)
    {
        throw CError("Error reading IPMI response");
    }

//Anandhi     KcsIpmiResponse resp(ipmiDrvResp.msg.data[0], ipmiDrvResp.msg.data_len-1, respDataBuffer);
    KcsIpmiResponse resp(ipmiDrvResp.msg.data[0], ipmiDrvResp.msg.data_len, respDataBuffer);
    return resp;
#endif
}

