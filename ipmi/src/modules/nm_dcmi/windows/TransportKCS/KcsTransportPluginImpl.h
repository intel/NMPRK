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


#ifndef _KCS_TRANSPORT_PLUGIN_IMPL_H
#define _KCS_TRANSPORT_PLUGIN_IMPL_H

#include <string>
#include <limits.h>
#include "OsSpecificSockets.h"

// forward declarations

struct dcmi_req_t;
struct dcmi_rsp_t;
struct dcmi_init_info_t;

class IpmiProxy;

namespace KcsTransportPlugin
{

    class KcsTransportPluginException : public std::exception
    {
    public:
        KcsTransportPluginException(std::string str) { msg_ = str; }
        ~KcsTransportPluginException() throw() { }
        virtual const char* what() const throw() { return msg_.c_str(); }

    private:
        std::string msg_;
    };
/**
 * Singleton implementation for TCP Inband Transport Plug-in.
 */
class KcsTransportPluginImpl
{
public:
    /**
     * Dtor.
     */
    ~KcsTransportPluginImpl();

    /**
     * Retrieves the only instance of the plug-in object.
     * 
     * @return Pointer to the plug-in object.
     */
    static KcsTransportPluginImpl* getInstance();

    /**
     * Initializes the plugin. Internally the method calls <code>getInstance()</code>. 
     * This method may be called in client code to control creation time
     * of the plug-in object without calling <code>getInstance()</code> explicitely
     * just to create the object.
     */
    static void initialize();

    /**
     * Destroys the instance of the plug-in. Internally the method calls destructor.
     * This method may be called in client code to control destruction time
     * of the plug-in object without calling dtor explicitely.
     */
    static void destroy() throw();

    // public Transport Plug-in API implementation

    /**
     *
     * Initializes the transport library and sets initial mandatory
	 * configuration parameters, such as logger functionality.
     *
     * @param initInfo Pointer to the initialization information data structure.
     *
     * @retval E_OK Successful initialization.
     * @retval E_OTHER Other error.
     */
    int start(const dcmi_init_info_t* initInfo);

    /**
     *
     * Performs final cleanup such as freeing allocated memory.
     *
     * @retval E_OK Successful shutdown.
     * @retval E_OTHER Other error.
     */
    int shutdown();

    /**
     * Sets a configuration option of the plug-in.
     * 
     * The following options are supported:
     *
     * Hostname : string
     *    Name of the UUT.
     *
     * Verbose : boolean
     *    Toggles verbose logging of the plug-in activities.
     *
     * @param paramName Name of the option.
     * @param value Value of the option.
     *
     * @return See Transport Plug-in API specification for the 
     *    description of return values.
     */
    int configure(const char* paramName, const char* value) throw();

    /**
     * Connects to the UUT.
     * @return See Transport Plug-in API specification for the
     *    description of return values.
     */
    int connect() throw();

    /**
     * Sends an IPMI request to the UUT and receives a response.
     * 
     * @param req IPMI request.
     * @param rsp IPMI response.
     *
     * @return See Transport Plug-in API specification for the
     *    description of return values.
     */
    int sendIpmiCmd(const struct dcmi_req_t* req, struct dcmi_rsp_t* rsp) throw();

    /**
     * Disconnects from UUT.
     *
     * @return See Transport Plug-in API specificiation for the 
     *    description of return values.
     */
    int disconnect() throw();

private:
    /// Hidden ctor for singleton.
    KcsTransportPluginImpl();

    /// Hidden copy ctor for singleton.
    KcsTransportPluginImpl(KcsTransportPluginImpl const&);

    /// Hidden assignment operator for singleton.
    KcsTransportPluginImpl& operator=(KcsTransportPluginImpl const&);

    /// Pointer to the only instance of the class.
    static KcsTransportPluginImpl* instance;

    // Configuration

    // End-of-Configuration

    IpmiProxy* ipmiProxy;
};

}

#endif //#ifndef _TCP_INBAND_TRANSPORT_PLUGIN_IMPL_H
