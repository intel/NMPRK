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
#ifndef _IPMI_PROXY_LINUX_H
#define _IPMI_PROXY_LINUX_H

#include <vector>

#include "IpmiProxy.h"
#include "nmprkTypes.h"
#include "nmprkErrorCodes.h"

class OpenIpmiProxy : public IpmiProxy
{
public:
    OpenIpmiProxy(void);
    
    virtual ~OpenIpmiProxy(void);

    void openProxy(const char* ipmiDevice = "/dev/ipmi0");

    ipmi_rsp_t RequestResponse(const ipmi_req_t& req) throw();

    ipmi_rsp_t GetAsyncResponse(int msTimeout) throw();

private:

    int ipmiDrvHandle;
    unsigned int ipmiReqMsgId;
    ipmi_rsp_t asyncResponse;
    bool asyncResponseValid;

    unsigned char sendMessageRequestChannel_;
    unsigned char sendMessageRequestNetFn_;
    unsigned char sendMessageRequestLun_;
    unsigned char sendMessageRequestCmd_;
    unsigned char sendMessageRequestSlaveAddr_;
    std::vector<unsigned char> sendMessageRequestData_;

    bool getMessageResponseValid_;
    unsigned char getMessageCompletionCode_;
    std::vector<unsigned char> getMessageResponseData_;
    
    void request(const ipmi_req_t& req);
    ipmi_rsp_t response();
    ipmi_rsp_t bridgedRequestResponse(const ipmi_req_t& req);
};

#if 0
/**
 * Provides an access to the Open IPMI driver
 */
class OpenIpmiDriverIpmiProxy : public IpmiProxy
{
public:
    /**
     * The default ctor opens a connection to the OpenIPMI
     * driver and configures the driver.
     */
    OpenIpmiDriverIpmiProxy(const char* ipmiDevice = "/dev/ipmi0");

    /**
     * Dtor.
     */
    ~OpenIpmiDriverIpmiProxy();

    /**
     * Sends IPMI command to BMC via IPMI driver and receives response.
     *
     * @param req IPMI request that is passed to the OpenIPMI driver.
     * @return IPMI response that is retrieved from the OpenIPMI driver.
     */
    KcsIpmiResponse requestResponse(const KcsIpmiRequest& req);
    KcsIpmiResponse bridgedRequestResponse(const KcsIpmiRequest& req);

    void request(const KcsIpmiRequest& req);

    KcsIpmiResponse response();

private:
    int ipmiDrvHandle;
    unsigned int ipmiReqMsgId;

    unsigned char sendMessageRequestChannel_;
    unsigned char sendMessageRequestNetFn_;
    unsigned char sendMessageRequestLun_;
    unsigned char sendMessageRequestCmd_;
    unsigned char sendMessageRequestSlaveAddr_;
    std::vector<unsigned char> sendMessageRequestData_;

    bool getMessageResponseValid_;
    unsigned char getMessageCompletionCode_;
    std::vector<unsigned char> getMessageResponseData_;
};
#endif
#endif //#ifndef _IPMI_PROXY_LINUX_H
