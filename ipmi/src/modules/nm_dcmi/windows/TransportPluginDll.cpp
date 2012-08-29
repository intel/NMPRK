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


#include <windows.h>

#include <sstream>

#include "TransportPluginDll.h"

using namespace std;

struct transport_dll_handle
{
    HINSTANCE handle;
};

TransportPluginDll::~TransportPluginDll()
{
    if (NULL != pDllHandle->handle)
    {
        FreeLibrary(pDllHandle->handle);
    }
    delete pDllHandle;
}

template<class T>
T
TransportPluginDll::getProcAddress(const char* procName)
{
    if (NULL == pDllHandle->handle)
    {
        throw logic_error("NULL pointer to library");
    }
    T addr = reinterpret_cast<T>(GetProcAddress(pDllHandle->handle, procName));
    if (NULL == addr)
    {
        throw runtime_error(string("Failed to get the address of ") + procName);
    }
    return addr;
}

void
TransportPluginDll::GetInterface(transport_plugin_interface* pTpi)
{
    pTpi->initialize = getProcAddress<dcmi_initialize_t>("Dcmi_Initialize");
	pTpi->shutdown = getProcAddress<dcmi_shutdown_t>("Dcmi_Shutdown");

	try
	{
		pTpi->ipmiCreateSession = NULL;
		pTpi->ipmiDestroySession = NULL;
		pTpi->ipmiConfigure = NULL;
		pTpi->ipmiConnect = NULL;
		pTpi->ipmiDisconnect = NULL;
		pTpi->ipmiSendCmd = NULL;
		pTpi->ipmiRecvData = NULL;
		pTpi->ipmiSendData = NULL;

		pTpi->ipmiCreateSession = getProcAddress<ipmi_create_session_t>("Ipmi_CreateSession");
		pTpi->ipmiDestroySession = getProcAddress<ipmi_destroy_session_t>("Ipmi_DestroySession");
		pTpi->ipmiConfigure = getProcAddress<ipmi_configure_t>("Ipmi_Configure");
		pTpi->ipmiConnect = getProcAddress<ipmi_connect_t>("Ipmi_Connect");
		pTpi->ipmiDisconnect = getProcAddress<ipmi_disconnect_t>("Ipmi_Disconnect");
		pTpi->ipmiSendCmd = getProcAddress<ipmi_send_ipmi_cmd_t>("Ipmi_SendCmd");
		pTpi->ipmiSendData = getProcAddress<ipmi_send_data_t>("Ipmi_SendData");		
		pTpi->ipmiRecvData = getProcAddress<ipmi_recv_data_t>("Ipmi_RecvData");


	}
	catch(...)
	{
	}
}

TransportPluginDll::TransportPluginDll(const char* pluginName)
{
    // Attempt to load the DLL
    pDllHandle = new struct transport_dll_handle;
	

    pDllHandle->handle = LoadLibrary((string(pluginName) + ".dll").c_str());
    if (NULL == pDllHandle->handle)
    {
        stringstream msg;
        msg << "Failed to load plug-in dll " << pluginName << ": LoadLibrary() Error "
                << GetLastError();
        throw runtime_error(msg.str());
    }
}

//*************************************************************************