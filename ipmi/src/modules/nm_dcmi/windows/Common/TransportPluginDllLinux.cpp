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


#include <sstream>
#include <stdexcept>
#include <dlfcn.h>
#include <cstdlib>
#include "TransportPluginDll.h"
#include "version.h"
#include <iostream>

#ifndef MODULE_PATH_ENV
#  define MODULE_PATH_ENV        "MODULE_PATH"
#endif

using namespace std;

struct transport_dll_handle
{
    void *handle;
};

TransportPluginDll::~TransportPluginDll()
{
    if (NULL != pDllHandle->handle)
    {
        dlclose(pDllHandle->handle);
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
    T addr = (T)(dlsym(pDllHandle->handle, procName));
    if (NULL == addr)
    {
        throw runtime_error(string("Failed get address of ") + procName);
    }
    return addr;
}

void
TransportPluginDll::GetInterface(transport_plugin_interface* pTpi)
{
    /*
    pTpi->initialize = getProcAddress<dcmi_initialize_t>("Dcmi_Initialize");
    pTpi->shutdown = getProcAddress<dcmi_shutdown_t>("Dcmi_Shutdown");
    pTpi->configure = getProcAddress<dcmi_configure_t>("Dcmi_Configure");
    pTpi->connect = getProcAddress<dcmi_connect_t>("Dcmi_Connect");
    pTpi->sendIpmiCmd = getProcAddress<dcmi_send_ipmi_cmd_t>("Dcmi_SendCmd");
    pTpi->disconnect = getProcAddress<dcmi_disconnect_t>("Dcmi_Disconnect");
    */
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
    /*
    if (lt_dlinit())
    {
        stringstream msg;
        msg << "Plugin: " << pluginName << ". Failed to initialize libltdl. Error msg: " << lt_dlerror();
        throw runtime_error(msg.str());
    }
    */
    // Attempt to load the DLL
    pDllHandle = new struct transport_dll_handle;
    stringstream libraryName;
    libraryName << "lib" << pluginName << ".so";

    /*
    const char *modPath = getenv(MODULE_PATH_ENV);
    if (modPath != NULL)
    {
        if (!lt_dlsetsearchpath(modPath))
        {
            stringstream msg;
            msg << "Plugin: " << pluginName << ". Failed lt_dlsetsearchpath. Error msg: " << dlerror();
            throw runtime_error(msg.str());
        }
    }
    */

    //cerr << "Lib: " << libraryName.str() << std::endl;
    pDllHandle->handle = dlopen(libraryName.str().c_str(), RTLD_GLOBAL|RTLD_NOW);

    if (!pDllHandle->handle)
    {
        //dlclose(pDllHandle->handle);
        
        stringstream msg;
        msg << "Failed to load plug-in dll " << pluginName << ": LoadLibrary() Error " << dlerror();
        cerr << msg.str() << std::endl;
        throw runtime_error(msg.str());
    }

    
}

