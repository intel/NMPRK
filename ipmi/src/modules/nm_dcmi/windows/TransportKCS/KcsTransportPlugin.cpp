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

#include <time.h>
#include <iostream>

#include "KcsTransportPluginImpl.h"
#include "KcsIpmiRequest.h"
#include "KcsIpmiResponse.h"

#include "transport.h"

using namespace std;
using namespace KcsTransportPlugin;

#ifdef WIN32

BOOL
APIENTRY
DllMain(HMODULE hModule,
        DWORD ul_reason_for_call,
        LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // do the necesasry initialization
        try
        {
            KcsTransportPluginImpl::initialize();
        }
        catch (exception& e)
        {
            cerr << "ERROR: Failed to initialize TCP Inband Transport Plug-in." << endl
            << e.what() << endl;
            return FALSE;
        }
        catch (...)
        {
            cerr << "ERROR: Failed to initialize TCP Inband Transport Plug-in. Unknown error." << endl;
            return FALSE;
        }
        DisableThreadLibraryCalls(hModule);
        break;

    case DLL_PROCESS_DETACH:
        KcsTransportPluginImpl::destroy();
        break;

    default:
        break;
    }
    return TRUE;
}

#endif // WIN32

EXPORTS
int
Dcmi_Initialize(const dcmi_init_info_t* initInfo)
{
    return KcsTransportPluginImpl::getInstance()->start(initInfo);
}

EXPORTS
int
Dcmi_Shutdown()
{
    return KcsTransportPluginImpl::getInstance()->shutdown();
}

/*
EXPORTS
int
Dcmi_Configure(const char* paramName, const char* val)
{
    return KcsTransportPluginImpl::getInstance()->configure(paramName, val);
}

EXPORTS
int
Dcmi_Connect(void)
{
    return KcsTransportPluginImpl::getInstance()->connect();
}

EXPORTS
int
Dcmi_SendCmd(const struct dcmi_req_t* req, struct dcmi_rsp_t* rsp)
{
    return KcsTransportPluginImpl::getInstance()->sendIpmiCmd(req, rsp);
}

EXPORTS
int
Dcmi_Disconnect(void)
{
    return KcsTransportPluginImpl::getInstance()->disconnect();
}
*/
/*****************************************************************************/
// Multi Session EXPORTS
/*****************************************************************************/

EXPORTS
int
Ipmi_Initialize(const dcmi_init_info_t* initInfo)
{
    return KcsTransportPluginImpl::getInstance()->start(initInfo);
}

EXPORTS
int
Ipmi_Shutdown()
{
    return KcsTransportPluginImpl::getInstance()->shutdown();
}

EXPORTS
int
Ipmi_CreateSession(IPMI_SESSION_HANDLE &h)
{
	h = 0;
	return E_OK;
}

EXPORTS
int
Ipmi_DestroySession(IPMI_SESSION_HANDLE h)
{
	return E_OK;
}

EXPORTS
int
Ipmi_Configure(IPMI_SESSION_HANDLE h, const char* paramName, const char* val)
{
    return KcsTransportPluginImpl::getInstance()->configure(paramName, val);
}

EXPORTS
int
Ipmi_Connect(IPMI_SESSION_HANDLE h)
{
    return KcsTransportPluginImpl::getInstance()->connect();
}

EXPORTS
int
Ipmi_SendCmd(IPMI_SESSION_HANDLE h, const struct dcmi_req_t* req, struct dcmi_rsp_t* rsp)
{
    return KcsTransportPluginImpl::getInstance()->sendIpmiCmd(req, rsp);
}

EXPORTS
int
Ipmi_Disconnect(IPMI_SESSION_HANDLE h)
{
    return KcsTransportPluginImpl::getInstance()->disconnect();
}

EXPORTS
int
Ipmi_RecvData(IPMI_SESSION_HANDLE h, const timeval timeoutVal, bool *timeout, dcmi_rsp_t* rsp)
{
	// Software Forge Inc. --- Start ------------------------------------------
	// Implemented this function for KCS
	dcmi_req_t getMsgReq;
	getMsgReq.netFun = 0x06;
	getMsgReq.cmd = 0x33;
	getMsgReq.rsAddr = 0x20;
	getMsgReq.rsLun = 0x00;
	getMsgReq.len = 0;

	time_t tNow, tStart;
    time(&tStart);
	tNow = tStart;

	int ret = 0;
	do
	{
		try
		{
			dcmi_rsp_t rsp1;
			ret = Ipmi_SendCmd(h, &getMsgReq, &rsp1);
			if(ret != E_OK)
				break;

			if(rsp1.compCode != 0x80)
			{
				ret = E_OK;
				if(rsp1.compCode == 0x00 && rsp1.len > 7)
				{
					rsp->compCode = rsp1.data[6];
					rsp->len = 0;
					for(int i=7; i<rsp1.len-1; i++)
						rsp->data[rsp->len++] = rsp1.data[i];
				}
				break;
			}

			ret = E_INVALID_VALUE;
			Sleep(10);
		}
		catch (KcsTransportPluginException& e)
		{
			ret = E_NOT_CONNECTED;
			break;
		}
	}
	while((tNow - tStart) < ((timeoutVal.tv_sec * 1000) + (timeoutVal.tv_usec / 1000)));

	return ret;
	// Software Forge Inc. --- End --------------------------------------------
}

EXPORTS
int
Ipmi_SendData(IPMI_SESSION_HANDLE h, const dcmi_req_t* req)
{
    //return RmcppTransportPluginImpl::getInstance()->sendSessionPayload(h, req);
	return -1;
}
