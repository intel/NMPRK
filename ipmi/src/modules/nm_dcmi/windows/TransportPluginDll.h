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

/*******************************************************************************
***		 File		:	TransportPluginDll.h								  **
***																			  **
********************************************************************************/

#ifndef _TRANSPORT_PLUGIN_DLL_H
#define _TRANSPORT_PLUGIN_DLL_H

#include "transport.h"

// forward declarations
struct dcmi_req_t;
struct dcmi_rsp_t;
struct dcmi_init_info_t;

/// Type of  pointer to the <code>Dcmi_Initialize()</code> of Transport Plug-in API.
typedef int (*dcmi_initialize_t)(const dcmi_init_info_t* initInfo);

/// Type of  pointer to the <code>Dcmi_Shutdown()</code> of Transport Plug-in API.
typedef int (*dcmi_shutdown_t)();

/// Type of  pointer to the <code>Dcmi_Configure()</code> of Transport Plug-in API.
typedef int (*dcmi_configure_t)(const char* param_name, const char* val);

/// Type of pointer to the <code>Dcmi_Connect()</code> of Transport Plug-in API.
typedef int (*dcmi_connect_t)();

/// Type of pointer to the <code>Dcmi_SendCmd()</code> of Transport Plug-in API.
typedef int (*dcmi_send_ipmi_cmd_t)(const struct dcmi_req_t* req, struct dcmi_rsp_t* rsp);

/// Type of pointer to the <code>Dcmi_Disconnect()</code> of Transport Plug-in API.
typedef int (*dcmi_disconnect_t)();

typedef int (*ipmi_create_session_t)(IPMI_SESSION_HANDLE &h);

typedef int (*ipmi_destroy_session_t)(IPMI_SESSION_HANDLE h);

typedef int (*ipmi_configure_t)(IPMI_SESSION_HANDLE h, const char* param_name, const char* val);

typedef int (*ipmi_connect_t)(IPMI_SESSION_HANDLE h);

typedef int (*ipmi_send_ipmi_cmd_t)(IPMI_SESSION_HANDLE h, const struct dcmi_req_t* req, struct dcmi_rsp_t* rsp);

typedef int (*ipmi_disconnect_t)(IPMI_SESSION_HANDLE h);

typedef int (*ipmi_recv_data_t)(IPMI_SESSION_HANDLE h, const timeval timeoutVal, bool *timeout, struct dcmi_rsp_t* rsp);

typedef int (*ipmi_send_data_t)(IPMI_SESSION_HANDLE h, const struct dcmi_req_t* req);

/**
 * Defines interface of a Transport Plug-in library.
 */
struct transport_plugin_interface
{
    /// Entrypoint of the <code>Dcmi_Initialize()</code>.
    dcmi_initialize_t initialize;

    /// Entrypoint of the <code>Dcmi_Shutdown()</code>.
    dcmi_shutdown_t shutdown;

	ipmi_create_session_t ipmiCreateSession;

	ipmi_destroy_session_t ipmiDestroySession;

	ipmi_configure_t ipmiConfigure;

	ipmi_connect_t ipmiConnect;
	
	ipmi_disconnect_t ipmiDisconnect;

	ipmi_send_ipmi_cmd_t ipmiSendCmd;

	ipmi_recv_data_t ipmiRecvData;

	ipmi_send_data_t ipmiSendData;
};

/**
 * Numeric plugin ID and its name (unique as well).
 */
struct PluginId
{
    /// Numeric plugin ID.
    unsigned int id;
    /// Plugin name.
    const char* name;
};

/**
 * Transport Plugin DLL hides OS-specific implementation of loading,
 * unloading and obtaining interface to dynamically linked libraries.
 */
class TransportPluginDll
{
public:
    /**
     * Constructor: attempts to load the dynamic library.
     *
     * @param pluginName Name of the DLL plugin.
     */
    TransportPluginDll(const char* pluginName);

    /**
     * Destructor: unloads the dynamic library.
     */
    ~TransportPluginDll();

    /**
     * Obtains dynamic library interface
     *
     * @param pTpi pointer to the interface struct.
     *
     * @returns NONE.
     */
    void GetInterface(transport_plugin_interface* pTpi);

protected:

    /// Handle to the DLL.
    struct transport_dll_handle *pDllHandle;

    /**
     * Retrieves a pointer to a function of type <code>T</code> from
     * the dynamic library.
     *
     * @param procName Name of the function to retrieve the entrypoint to.
     *
     * @return Pointer to the function.
     */
    template<class T> T getProcAddress(const char* procName);
};

#endif //#ifndef _TRANSPORT_PLUGIN_DLL_H
