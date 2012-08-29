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
#include "KcsTransportPluginImpl.h"
#include "KcsIpmiRequest.h"
#include "KcsIpmiResponse.h"
#ifdef WIN32
#include "IpmiProxy.h"
#else
#include "IpmiProxyLinux.h"
#endif

#include "transport.h"
#include "RemoteLogger.h"

#include <iostream>
#include <stdexcept>
#include <iomanip>

using namespace std;
using namespace TransportPlugin;

namespace KcsTransportPlugin
{

KcsTransportPluginImpl* KcsTransportPluginImpl::instance = NULL;

KcsTransportPluginImpl::KcsTransportPluginImpl()
                            :ipmiProxy(NULL)
{
}

KcsTransportPluginImpl::~KcsTransportPluginImpl()
{
}

KcsTransportPluginImpl*
KcsTransportPluginImpl::getInstance()
{
    if (NULL == instance)
    {
        instance = new KcsTransportPluginImpl;
    }
    return instance;
}

void
KcsTransportPluginImpl::initialize()
{
    getInstance();
}

void
KcsTransportPluginImpl::destroy()
    throw()
{
    try
    {
        if (NULL != instance)
        {
            delete instance;
            instance = NULL;
        }
    }
    catch (...)
    {
    }
}

int
KcsTransportPluginImpl::start(const dcmi_init_info_t* initInfo)
{
    if (initInfo == NULL)
    {
        return E_OTHER;
    }

    RemoteLogger::setLogPrefix("KCS transport");
    RemoteLogger::setLoggerFunc(initInfo->loggerFun);

    LOG_HIDEBUG << "Successful plugin initialization.";

    return E_OK;
}

int
KcsTransportPluginImpl::shutdown()
{
    return E_OK;
}

int
KcsTransportPluginImpl::configure(const char* paramName, const char* value)
    throw()
{
    LOG_HIDEBUG << "Configure: " << paramName << " = " << value;

    try
    {
        
        {
            LOG_WARN << "Configure: Unknown configuration parameter:" << paramName;
            return E_NAME_UNKNOWN;
        }
        return E_OK;
    }
    catch (...)
    {
        return E_OTHER;
    }
}

int
KcsTransportPluginImpl::connect()
    throw()
{
    try
    {
        if (NULL == ipmiProxy)
        {
			string errorMsg;
			bool ret = IpmiProxy::GetIpmiProxyInstance(&ipmiProxy, errorMsg);
			if(ret == false)
				throw IpmiProxyException(errorMsg);
        }
        return E_OK;
    }
	catch (IpmiProxyException& e)
	{
		throw;
	}
    catch (exception& e)
    {
		LOG_HIDEBUG << "\nERROR: connect(): " << e.what() << endl;
		throw;
    }
    catch (...)
    {
        return E_OTHER;
    }
}

int
KcsTransportPluginImpl::sendIpmiCmd(const struct dcmi_req_t* req, struct dcmi_rsp_t* rsp)
    throw()
{
    LOG_HIDEBUG << "Dcmi_SendCmd(): Request to be sent: " \
        << "netFun=0x" << hex << setw(2) << setfill('0') << (unsigned int)req->netFun \
        << ", cmd=0x" << hex << setw(2) << setfill('0') << (unsigned int)req->cmd \
        << ", data length=" << dec << (unsigned int)req->len \
        << ", rsAddr=0x" << hex << setw(2) << setfill('0') << (unsigned int)req->rsAddr \
        << ", rsLun=0x" << hex << setw(2) << setfill('0') << (unsigned int)req->rsLun;

    try
    {
		if(ipmiProxy == NULL)
			throw KcsTransportPluginException("IpmiProxy was not initilized correctly!");

        KcsIpmiRequest ipmiReq(req->rsAddr, req->netFun, req->rsLun, req->cmd, static_cast<unsigned int>(req->len), req->data);
        KcsIpmiResponse kcsRsp(ipmiProxy->requestResponse(ipmiReq));

        rsp->compCode = kcsRsp.getCompletionCode();
        if (kcsRsp.getData().size() > 0)
        {
            memcpy(rsp->data, &kcsRsp.getData().front(), kcsRsp.getData().size());
        }
        rsp->len = (int)kcsRsp.getData().size();

        LOG_HIDEBUG << "Dcmi_SendCmd(): Received response: " \
            << "compCode=0x" << hex << setw(2) << setfill('0') << (unsigned int)rsp->compCode \
            << ", data length=" << dec << (unsigned int)rsp->len;

		/*
		Sleep(1000);

		bool AttentionSet;
		unsigned char StatusRegisterValue;
		ipmiProxy->SMS_Attention(AttentionSet, StatusRegisterValue);

		LOG_HIDEBUG << "Dcmi_SendCmd(): SMS_Attention: " << "AttentionSet: " << AttentionSet << ", StatusRegisterValue: " << hex << setw(2) << setfill('0') << (int)StatusRegisterValue;
		*/
        return E_OK;
    }
    catch (exception& e)
    {
        LOG_ERR << "ERROR: sendIpmiCmd(): " << e.what() << endl;
        return E_OTHER;
    }
    catch (...)
    {
        return E_OTHER;
    }
}

int
KcsTransportPluginImpl::disconnect()
    throw()
{
    try
    {
		if(ipmiProxy != NULL)
			delete ipmiProxy;
        ipmiProxy = NULL;
        return E_OK;
    }
    catch (exception& e)
    {
        LOG_HIDEBUG << "\nERROR: disconnect(): " << e.what() << endl;
        return E_OTHER;
    }
    catch (...)
    {
        return E_OTHER;
    }
}

}
