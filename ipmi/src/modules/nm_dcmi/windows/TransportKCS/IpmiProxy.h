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


#ifndef _IPMI_PROXY_H
#define _IPMI_PROXY_H

#define _WIN32_DCOM
#include <atlbase.h>
#include <wbemidl.h>
#include <comdef.h>
#include <SetupAPI.h>
#include <string>

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
	static bool GetIpmiProxyInstance(IpmiProxy **proxy, std::string& errorMessage);

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
 * Provides an access to the Microsoft_IPMI interface
 * exposed by the WMI.
 */
class MicrosoftDriverIpmiProxy : public IpmiProxy
{
public:
    /**
     * The default ctor opens a connection to the local
     * Wbem service and obtains a path to the Microsoft_IPMI
     * object. If Wbem cannot be accessed or Microsoft_IPMI
     * is not accessible, an exception is thrown.
     */
    MicrosoftDriverIpmiProxy();

    /**
     * Dtor.
     */
    ~MicrosoftDriverIpmiProxy();

    /**
     * Invokes Microsoft_IPMI::RequestResponse method.
     *
     * @param req IPMI request that is passed to the Microsoft_IPMI.
     * @return IPMI response that is retrieved from the Microsoft_IPMI.
     */
    KcsIpmiResponse requestResponse(const KcsIpmiRequest& req);

	void SMS_Attention(bool& AttentionSet, unsigned char& StatusRegisterValue);

private:
    /// Ptr to the IWbemServices interface.
    CComPtr<IWbemServices> wbemServices;

    /// Path the Microsoft_IPMI instance.
    bstr_t ipmiInstancePath;

    /// BMC Address retrieved from the Microsoft_IPMI instance.
    unsigned char bmcAddress;

    /// Signature of [in] parameters to the Microsoft_IPMI::RequestResponse.
    CComPtr<IWbemClassObject> inParamsDef;
};

/**
 * Provides an access to the Microsoft_IPMI interface
 * exposed by the WMI.
 */
class IntelImbDriverIpmiProxy : public IpmiProxy
{
public:
    /**
     * The default ctor opens a connection to the local
     * Wbem service and obtains a path to the Microsoft_IPMI
     * object. If Wbem cannot be accessed or Microsoft_IPMI
     * is not accessible, an exception is thrown.
     */
    IntelImbDriverIpmiProxy();

    /**
     * Dtor.
     */
    ~IntelImbDriverIpmiProxy();

    /**
     * Invokes Microsoft_IPMI::RequestResponse method.
     *
     * @param req IPMI request that is passed to the Microsoft_IPMI.
     * @return IPMI response that is retrieved from the Microsoft_IPMI.
     */
    KcsIpmiResponse requestResponse(const KcsIpmiRequest& req);

private:

	DWORD _seqNo;

	bool getMessageResponseValid_;
	unsigned int getMessageResponseDataLength_;
    unsigned char getMessageResponseData_[1024];
};

#endif //#ifndef _IPMI_PROXY_H
