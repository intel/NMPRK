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


#include <stdexcept>
#include <iostream>

#include "transport.h"

#include "RmcppTransportPluginImpl.h"

using namespace std;
using namespace RmcppTransportPlugin;

#ifdef WIN32

#include <windows.h>

BOOL
APIENTRY
DllMain(HMODULE module, DWORD reasonForCall, LPVOID reserved)
{
    UNREFERENCED_PARAMETER(reserved);

    switch (reasonForCall)
    {
    case DLL_PROCESS_ATTACH:
        // do the necessary initialization
        try
        {
            RmcppTransportPluginImpl::initialize();
        }
        catch (exception& e)
        {
            cerr << "ERROR: Failed to initialize RMCPP Transport Plug-in." << endl
            << e.what() << endl;
            return FALSE;
        }
        catch (...)
        {
            cerr << "ERROR: Failed to initialize RMCPP Transport Plug-in. Unknown error." << endl;
            return FALSE;
        }
        DisableThreadLibraryCalls(module);
        break;

    case DLL_PROCESS_DETACH:
        RmcppTransportPluginImpl::destroy();
        break;

    default:
        break;
    }
    return TRUE;
}

#endif //WIN32

EXPORTS
int
Dcmi_Initialize(const dcmi_init_info_t* initInfo)
{
    return RmcppTransportPluginImpl::getInstance()->start(initInfo);
}

EXPORTS
int
Dcmi_Shutdown()
{
    return RmcppTransportPluginImpl::getInstance()->shutdown();
}

EXPORTS
int
Dcmi_Configure(const char* paramName, const char* val)
{
    return RmcppTransportPluginImpl::getInstance()->configure(IPMI_SESSION_HANDLE_DEFAULT, paramName, val);
}

EXPORTS
int
Dcmi_Connect(void)
{
    return RmcppTransportPluginImpl::getInstance()->connect(IPMI_SESSION_HANDLE_DEFAULT);
}

EXPORTS
int
Dcmi_SendCmd(const struct dcmi_req_t* req, struct dcmi_rsp_t* rsp)
{
    return RmcppTransportPluginImpl::getInstance()->sendIpmiCmd(IPMI_SESSION_HANDLE_DEFAULT, req, rsp);
}

EXPORTS
int
Dcmi_Disconnect(void)
{
    return RmcppTransportPluginImpl::getInstance()->disconnect(IPMI_SESSION_HANDLE_DEFAULT);
}

EXPORTS
int
Dcmi_RecvData(const timeval timeoutVal, bool *timeout, dcmi_rsp_t* rsp)
{
	return RmcppTransportPluginImpl::getInstance()->recvSessionPayload(IPMI_SESSION_HANDLE_DEFAULT, timeoutVal, timeout, rsp);
}

EXPORTS
int
Dcmi_SendData(const dcmi_req_t* req)
{
    return RmcppTransportPluginImpl::getInstance()->sendSessionPayload(IPMI_SESSION_HANDLE_DEFAULT, req);
}

/*****************************************************************************/
// Multi Session EXPORTS
/*****************************************************************************/

EXPORTS
int
Ipmi_Initialize(const dcmi_init_info_t* initInfo)
{
    return RmcppTransportPluginImpl::getInstance()->start(initInfo);
}

EXPORTS
int
Ipmi_Shutdown()
{
    return RmcppTransportPluginImpl::getInstance()->shutdown();
}

EXPORTS
int
Ipmi_CreateSession(IPMI_SESSION_HANDLE &h)
{
	h = RmcppTransportPluginImpl::getInstance()->createSession();
	return E_OK;
}

EXPORTS
int
Ipmi_DestroySession(IPMI_SESSION_HANDLE h)
{
	RmcppTransportPluginImpl::getInstance()->destroySession(h);
	return E_OK;
}

EXPORTS
int
Ipmi_Configure(IPMI_SESSION_HANDLE h, const char* paramName, const char* val)
{
    return RmcppTransportPluginImpl::getInstance()->configure(h, paramName, val);
}

EXPORTS
int
Ipmi_Connect(IPMI_SESSION_HANDLE h)
{
    return RmcppTransportPluginImpl::getInstance()->connect(h);
}

EXPORTS
int
Ipmi_SendCmd(IPMI_SESSION_HANDLE h, const struct dcmi_req_t* req, struct dcmi_rsp_t* rsp)
{
    return RmcppTransportPluginImpl::getInstance()->sendIpmiCmd(h, req, rsp);
}

EXPORTS
int
Ipmi_Disconnect(IPMI_SESSION_HANDLE h)
{
    return RmcppTransportPluginImpl::getInstance()->disconnect(h);
}

EXPORTS
int
Ipmi_RecvData(IPMI_SESSION_HANDLE h, const timeval timeoutVal, bool *timeout, dcmi_rsp_t* rsp)
{
	return RmcppTransportPluginImpl::getInstance()->recvSessionPayload(h, timeoutVal, timeout, rsp);
}

EXPORTS
int
Ipmi_SendData(IPMI_SESSION_HANDLE h, const struct dcmi_req_t* req)
{
    return RmcppTransportPluginImpl::getInstance()->sendSessionPayload(h, req);
}

