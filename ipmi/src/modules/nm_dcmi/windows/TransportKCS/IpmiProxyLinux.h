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

#ifndef _IPMI_PROXY_LINUX_H
#define _IPMI_PROXY_LINUX_H

#include <vector>

// fwd declarations

class KcsIpmiRequest;
class KcsIpmiResponse;

class IpmiProxyException : public std::exception
{
public:
    IpmiProxyException(std::string str) { msg_ = str; }
    ~IpmiProxyException() throw () {}
    virtual const char* what() const throw () { return msg_.c_str(); }
private:
    std::string msg_;
};

/**
 * Provides an access to the Microsoft_IPMI interface
 * exposed by the WMI.
 */
class IpmiProxy
{
public:
	static bool GetIpmiProxyInstance(IpmiProxy **proxy, std::string& errMsg);

    /**
     * Dtor.
     */
	virtual ~IpmiProxy() { }

    /**
     * Invokes Microsoft_IPMI::RequestResponse method.
     *
     * @param req IPMI request that is passed to the Microsoft_IPMI.
     * @return IPMI response that is retrieved from the Microsoft_IPMI.
     */
    virtual KcsIpmiResponse requestResponse(const KcsIpmiRequest& req) = 0;

protected:
    /**
     * The default ctor opens a connection to the local
     * Wbem service and obtains a path to the Microsoft_IPMI
     * object. If Wbem cannot be accessed or Microsoft_IPMI
     * is not accessible, an exception is thrown.
     */
    IpmiProxy() { }
};

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

#endif //#ifndef _IPMI_PROXY_LINUX_H
