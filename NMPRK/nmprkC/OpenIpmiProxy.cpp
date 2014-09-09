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
 ***		transmitted, distributed, or disclosed in any way without Intel�s	  **
 ***		prior express written permission.									  **
 ***																			  **
 ***		No license under any patent, copyright, trade secret or other		  **
 ***		intellectual property right is granted to or conferred upon you by	  **
 ***		disclosure or delivery of the Materials, either expressly, by		  **
 ***		implication, inducement, estoppel or otherwise. Any license under	  **
 ***		such intellectual property rights must be express and approved by	  **
 ***		Intel in writing.													  **
 ********************************************************************************/

#include <cstring>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ipmi.h>

#include "OpenIpmiProxy.h"

using namespace std;

OpenIpmiProxy::OpenIpmiProxy()
{
    openProxy();
}

void OpenIpmiProxy::openProxy(const char* ipmiDevice)
{
    this->ipmiReqMsgId = -1;

    ipmiDrvHandle = open(ipmiDevice, O_RDWR);
    if (ipmiDrvHandle < 0)
    {
        throw IpmiProxyException(NMPRK_PROXY_ERROR, "Cannot open IPMI device");
    }

    int events = 0;
    if (ioctl(ipmiDrvHandle, IPMICTL_SET_GETS_EVENTS_CMD, &events) < 0)
    {
        throw IpmiProxyException(NMPRK_PROXY_ERROR, "Cannot configure events");
    }

    unsigned int bmcSlaveAddr = 0x20;
    if (ioctl(ipmiDrvHandle, IPMICTL_SET_MY_ADDRESS_CMD, &bmcSlaveAddr) < 0)
    {
        throw IpmiProxyException(NMPRK_PROXY_ERROR, "Cannot set BMC Address");
    }
}

OpenIpmiProxy::~OpenIpmiProxy()
{
    if (ipmiDrvHandle >= 0)
    {
        close(ipmiDrvHandle);
    }
}

ipmi_rsp_t OpenIpmiProxy::RequestResponse(const ipmi_req_t& req) throw()
{
    if (ipmiDrvHandle < 0)
    {
        throw IpmiProxyException(NMPRK_PROXY_ERROR_XFER, "Driver handle error");
    }

    if (req.netFun == 0x06 && req.cmd == 0x34)
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
}

ipmi_rsp_t OpenIpmiProxy::GetAsyncResponse(int msTimeout) throw()
{
    if(asyncResponseValid == true)
    {
        asyncResponseValid = false;
        return asyncResponse;
    }
    else
    {
        ipmi_rsp_t rsp;
        rsp.compCode = 0x80;
        rsp.len = 0;
        return rsp;
    }
}

void OpenIpmiProxy::request(const ipmi_req_t& req)
{
    if (ipmiDrvHandle < 0)
    {
        throw IpmiProxyException(NMPRK_PROXY_ERROR, "Driver handle error");
    }

    struct ipmi_system_interface_addr bmcAddr;
    memset(&bmcAddr, 0, sizeof (bmcAddr));
    bmcAddr.lun = req.rsLun;
    bmcAddr.addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE;
    bmcAddr.channel = IPMI_BMC_CHANNEL;

    struct ipmi_req ipmiDrvRequest;
    memset(&ipmiDrvRequest, 0, sizeof (ipmiDrvRequest));
    ipmiDrvRequest.addr = (unsigned char*) &bmcAddr;
    ipmiDrvRequest.addr_len = sizeof (bmcAddr);
    ipmiDrvRequest.msgid = ++ipmiReqMsgId;

    ipmiDrvRequest.msg.netfn = req.netFun;
    ipmiDrvRequest.msg.cmd = req.cmd;

    
    ipmiDrvRequest.msg.data_len = req.len;
    ipmiDrvRequest.msg.data = (unsigned char *)req.data;

    if (ioctl(ipmiDrvHandle, IPMICTL_SEND_COMMAND, &ipmiDrvRequest) < 0)
    {
        throw IpmiProxyException(NMPRK_PROXY_ERROR_XFER, "Cannot send IPMI command");
    }
}

ipmi_rsp_t OpenIpmiProxy::response()
{
    if (ipmiDrvHandle < 0)
    {
        throw IpmiProxyException(NMPRK_PROXY_ERROR, "Driver handle error");
    }

    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(ipmiDrvHandle, &descriptors);

    struct timeval waitTimeout;
    waitTimeout.tv_sec = 60;
    waitTimeout.tv_usec = 00;

    if (select(ipmiDrvHandle + 1, &descriptors, NULL, NULL, &waitTimeout) < 0)
    {
        throw IpmiProxyException(NMPRK_PROXY_ERROR_XFER, "Failed to wait for IPMI response");
    }

    if (!FD_ISSET(ipmiDrvHandle, &descriptors))
    {
        throw IpmiProxyException(NMPRK_PROXY_ERROR_XFER, "Timeout waiting for IPMI response");
    }

    struct ipmi_recv ipmiDrvResp;
    struct ipmi_addr ipmiDrvRespAddr;
    unsigned char respDataBuffer[1024]; //TBD: Replace 1024 with some constant.

    ipmiDrvResp.addr = (unsigned char *) &ipmiDrvRespAddr;
    ipmiDrvResp.addr_len = sizeof (ipmiDrvRespAddr);
    ipmiDrvResp.msg.data = respDataBuffer;
    ipmiDrvResp.msg.data_len = sizeof (respDataBuffer);

    if (ioctl(ipmiDrvHandle, IPMICTL_RECEIVE_MSG_TRUNC, &ipmiDrvResp) < 0)
    {
        throw IpmiProxyException(NMPRK_PROXY_ERROR_XFER, "Error reading IPMI response");
    }

    ipmi_rsp_t resp;
    resp.compCode = ipmiDrvResp.msg.data[0];
    resp.len = ipmiDrvResp.msg.data_len - 1;
    memcpy(resp.data, &respDataBuffer[1], resp.len);

    return resp;
}

ipmi_rsp_t OpenIpmiProxy::bridgedRequestResponse(const ipmi_req_t& req)
{
    struct ipmi_req ipmiDrvRequest;
    memset(&ipmiDrvRequest, 0, sizeof (ipmiDrvRequest));

    if (req.netFun == 0x06 && req.cmd == 0x34)
    {
        if (req.len < 8)
        {
            throw IpmiProxyException(NMPRK_PROXY_ERROR_XFER, "Data size of Send Message cmd less than expected!");
        }

        struct ipmi_ipmb_addr ipmbAddr;
        ipmbAddr.addr_type = IPMI_IPMB_ADDR_TYPE;
        ipmbAddr.channel = req.data[0] & 0x0F;
        ipmbAddr.slave_addr = req.data[1];
        ipmbAddr.lun = req.data[2] & 0x03;

        ipmiDrvRequest.addr = (unsigned char*) &ipmbAddr;
        ipmiDrvRequest.addr_len = sizeof (ipmbAddr);

        ipmiDrvRequest.msg.netfn = (req.data[2] >> 2) & 0x3F;
        ipmiDrvRequest.msg.cmd = req.data[6];
        ipmiDrvRequest.msgid = ++ipmiReqMsgId;

        ipmiDrvRequest.msg.data_len = req.len - 8;
        ipmiDrvRequest.msg.data = (unsigned char *)&req.data[7];

        if (ioctl(ipmiDrvHandle, IPMICTL_SEND_COMMAND, &ipmiDrvRequest) < 0)
        {
            throw IpmiProxyException(NMPRK_PROXY_ERROR_XFER, "Cannot send IPMI command");
        }

        asyncResponse = response();
        asyncResponseValid = true;

        ipmi_rsp_t sendRsp;
        sendRsp.compCode = 0;
        sendRsp.len = 0;

        return sendRsp;
    }
    else
    {
        throw IpmiProxyException(NMPRK_PROXY_ERROR, "Proxy Implementation Error");
    }
}
