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

#ifndef _RMCPP_TRANSPORT_PLUGIN_IMPL
#define _RMCPP_TRANSPORT_PLUGIN_IMPL

#define IP_MAX_LEN 16
#define USERNAME_MAX_LEN 16
#define PASSWORD_MAX_LEN 20

#include "ByteVector.h"
#include "RmcppTypes.h"
#include "OsSpecificSockets.h"
#include <map>

// forward declarations
struct dcmi_req_t;
struct dcmi_rsp_t;

namespace RmcppTransportPlugin
{

// forward declarations
class IpmiSession;
class RmcppIpmiSession;


/**
 * Singleton implementation for WS-MAN Inband Transport Plug-in.
 */
class RmcppTransportPluginImpl
{
public:
    /**
     * Dtor.
     */
    ~RmcppTransportPluginImpl();

    /**
     * Retrieves the only instance of the plug-in object.
     * 
     * @return Pointer to the plug-in object.
     */
    static RmcppTransportPluginImpl* getInstance();

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

	IPMI_SESSION_HANDLE createSession();

	void destroySession(IPMI_SESSION_HANDLE h);

    /**
     * Sets a configuration option of the plug-in.
     * 
     * The following options are supported:
     *
     * targetIP : string
     *    IP of the UUT
     *
     * username : string
     *    User Name
     *
     * password : string
     *    Password
     *
     * confid : string
     *     Confidentiality algorithm to use for RMCP+ session encryption.
     *     One of NONE, AES_CBC_128.
     *     By default session is unencrypted (same as confid set to NONE).
     *
     * integ : string
     *     Integrity algorithm to use for RMCP+ session integrity.
     *     One of NONE, HMAC_SHA1_96, HMAC_MD5_128, MD5_128.
     *     By default session integrity is not checked (same as integ set to NONE).
     *
     * format : string
     *     One of  RMCPP_MD5, RMCPP_SHA, RMCPP_NONE, SOL_NONE
	 *
     * CipherSuite : number
     *     0-12, see DCMI Table 4-1 Supported Cipher Suites
     *
     *
     * bmcKey : string (40 bytes hex string)
     *     BMC Key used for two-key logins. 
     *     By default the BMC key is empty and user password is used to generate SIK.
     *     If the target returns non-null KG flag in response to Get Channel Auth Cap
     *     and the BMC Key is not provided by configuration, session cannot be established.
     *     40 bytes hex string example: "0102030405060708090a0b0c0d0e0f1011121314"
     *
     * ping : bool ("true" | "false")
     *     Indicates whether the target should be pinged with ASF Presence Ping before
     *     trying to establish session.
     *     By default ping is "false".
     *
     * @param paramName Name of the option.
     * @param value Value of the option.
     *
     * @return See Transport Plug-in API specification for the 
     *    description of return values.
     */
    int configure(IPMI_SESSION_HANDLE h, const char* paramName, const char* value) throw();

    /**
     * Connects to the UUT.
     * @return See Transport Plug-in API specification for the
     *    description of return values.
     */
    int connect(IPMI_SESSION_HANDLE h) throw();

    /**
     * Sends an IPMI request to the UUT and receives a response.
     * 
     * @param req IPMI request.
     * @param resp IPMI response.
     *
     * @return See Transport Plug-in API specification for the
     *    description of return values.
     */
    int sendIpmiCmd(IPMI_SESSION_HANDLE h, const dcmi_req_t* req, dcmi_rsp_t* rsp) throw();

    /**
     * Receives payload received on session socket.
     * 
     * @param resp IPMI response.
     *
     * @return See Transport Plug-in API specification for the
     *    description of return values.
     */
    int recvSessionPayload(IPMI_SESSION_HANDLE h, const timeval timeoutVal, bool* timeout, dcmi_rsp_t* rsp) throw();

    /**
     * Sends payload to session socket.
     * 
     * @param resp IPMI response.
     *
     * @return See Transport Plug-in API specification for the
     *    description of return values.
     */
    int sendSessionPayload(IPMI_SESSION_HANDLE h, const dcmi_req_t* req) throw();

	/**
     * Disconnects from UUT.
     *
     * @return See Transport Plug-in API specificiation for the 
     *    description of return values.
     */
    int disconnect(IPMI_SESSION_HANDLE h) throw();

private:
    /// Hidden ctor for singleton.
    RmcppTransportPluginImpl();

    /// Hidden copy ctor for singleton.
    RmcppTransportPluginImpl(RmcppTransportPluginImpl const&);

    /// Hidden assignment operator for singleton.
    RmcppTransportPluginImpl& operator=(RmcppTransportPluginImpl const&);

    /// Pointer to the only instance of the class.
    static RmcppTransportPluginImpl* instance;

    // TODO: doc (if not subject to removal)
    int match(const char* base, int MAX_STRINGS, const char *strings, ...);

	// Check the Target IP format before making a connection attempt.
	int CheckTargetIP(const char* value);

	//Check the UserName format before making a connection attempt.
	int CheckUserName(const char* value);

	//Check the Password format before making a connection attempt.
	int CheckPassword(const char* value);

	// Check the session handle
	IpmiSession *validateIpmiSessionHandle(IPMI_SESSION_HANDLE h);

    /// Pointer to session established in <code>connect()</code>.
    IpmiSession* ipmiSession;

    /// Destination IP.
    std::string ip;
    /// Username
    std::string userName;
    /// Password
    std::string password;

    // RMCP+

    /// Auth Type / Format
    ipmi_auth_type authType;

    //RMCP+
    
    /// Authentication algorithm
    ipmi_auth_algo authAlgo;
    
    /// Integrity algorithm
    ipmi_integ_algo integAlgo;

    /// Confidentionality algorithm
    ipmi_confid_algo confidAlgo;

    /// indicates whether the target is PINGed first
    bool doPing;
    
    /// KG (BMC Key)
    ByteVector kG;

	// Session List
	std::map<unsigned int, RmcppIpmiSession *> sessionMap;
};

}

#endif //#ifndef _RMCPP_TRANSPORT_PLUGIN_IMPL
