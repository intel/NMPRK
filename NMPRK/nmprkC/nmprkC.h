/***************************************************************************
 * Copyright 2013 Intel Corporation                                        *
 * Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 * http://www.apache.org/licenses/LICENSE-2.0                              *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/

/*! \file nmprkC.h
    \brief The header file for accessing NMPRK functions
*/

/*! \mainpage
	\section intro Introduction
	NMPRK provides a set of API functions that facilitate the use of IPMI and
	Node Manager commands in custom software.

	Quick Link to Functions:\n
	nmprkC.h

	IPMI Specification: https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html \n
	Node Manager Specification: https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html
 */

#ifndef _NMPRK_C_H_
#define _NMPRK_C_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the NMPRKC_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// NMPRKC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef WIN32
#ifdef NMPRKC_EXPORTS
#define NMPRKC_API __declspec(dllexport)
#else
#define NMPRKC_API __declspec(dllimport)
#endif
#else
#define NMPRKC_API
#endif

#ifdef WIN32
#include <Windows.h>
#else
#define IN
#define OUT
#endif

#include "nmprkTypes.h"

#if defined __cplusplus
extern "C" 
{
#endif

/*****************************************************************************/
// NMPRK Administrative Functions
/*****************************************************************************/
/*!
	Get the version of the NMPRK API

	\return C String that displays the version of the API
*/
NMPRKC_API
const char *
NMPRK_GetApiVersion();

/*!
	Start debug logging to the file that is specified.

	If file already exists, new log messages will be appended.

	\param[in] filename The path to the file to write debug messages to
	\return ::nmprk_status_t (NMPRK_SUCCESS if the function is successful)
*/
NMPRKC_API 
nmprk_status_t 
NMPRK_StartDebugLogging(
	const char *filename);

/*!
	Stop debug logging that was previously started

	\return ::nmprk_status_t (NMPRK_SUCCESS if the function is successful)
*/
NMPRKC_API 
nmprk_status_t 
NMPRK_StopDebugLogging();

/*!
	Specify a callback function to be executed when a new log message is
	available.

	This can be used to integrated NMPRK log messages into an already
	established logging mechanism.

	This can be used independently of ::NMPRK_StartDebugLogging

	\param[in] filename The path to the file to write debug messages to
	\return ::nmprk_status_t (NMPRK_SUCCESS if the function is successful)
*/
NMPRKC_API
nmprk_status_t
NMPRK_SetDebugCallback(nmprk_debug_callback callback);

/*****************************************************************************/
// NMPRK Connection Functions
/*****************************************************************************/

/*!
	Connect to the local server through one of the supported IPMI drivers

	Supported drivers are:\n
	Microsoft Generic IPMI\n
	Intel IMB

	\param[out] h The handle for the connection is returned here
	\return ::nmprk_status_t (NMPRK_SUCCESS if the function is successful)
*/
NMPRKC_API
nmprk_status_t
NMPRK_ConnectLocal(
	OUT nmprk_conn_handle_t *h);

/*!
	Connect to a remote server
	
	The caller provides the IP or hostname, username
	and password. Optionally, the caller can provide a cipher suite.

	\param[in] input A pointer to the input structure containing connection
				     parameters
	\param[out] h The handle for the connection is returned here
	\return ::nmprk_status_t (NMPRK_SUCCESS if the function is successful)
*/
NMPRKC_API
nmprk_status_t
NMPRK_ConnectRemote(
	IN nmprk_connect_remote_parameters_t *input, 
	OUT nmprk_conn_handle_t *h);

/*!
	Disconnect from local or remote connection

	\param[in] h The connection handle
	\return ::nmprk_status_t (NMPRK_SUCCESS if the function is successful)
*/
NMPRKC_API
nmprk_status_t
NMPRK_Disconnect(
	IN nmprk_conn_handle_t h);

/*****************************************************************************/
// NMPRK Basic Functions
/*****************************************************************************/

/*!
	Set the default bridging parameters for Node Manager commands. Any of the
	Node Manager commands in this API with use this channel and address to
	bridge the commands. Other functions (i.e. IPMI specific) do not use them.

	\param[in] h The connection handle
	\param[in] channel The bridging channel
	\param[in] address The bridging address
	\return ::nmprk_status_t (NMPRK_SUCCESS if the function is successful)
*/
NMPRKC_API
nmprk_status_t
NMPRK_SetDefaultNmCommandBridging(
	IN nmprk_conn_handle_t h,
	IN byte_t channel,
	IN byte_t address);

/*!
	Send a non-bridged command

	Sends a command directly to the BMC and performs no bridging even if
	the default bridging parameters have been set.

	\param[in] h The connection handle
	\param[in] req A pointer to the request data structure
	\param[out] rsp The response is stored in this data structure
	\return ::nmprk_status_t (NMPRK_SUCCESS if the function is successful)
*/
NMPRKC_API
nmprk_status_t
NMPRK_SendNonBridgedCommand(
	IN nmprk_conn_handle_t h,
	IN ipmi_req_t *req,
	OUT ipmi_rsp_t *rsp);

/*!
	Send a bridged command

	Sends a command using the specified bridging channel and address

	\param[in] h The connection handle
	\param[in] channel The bridging channel
	\param[in] address The bridging address
	\param[in] req A pointer to the request data structure
	\param[out] rsp The response is stored in this data structure
	\return ::nmprk_status_t (NMPRK_SUCCESS if the function is successful)
*/
NMPRKC_API
nmprk_status_t
NMPRK_SendBridgedCommand(
	IN nmprk_conn_handle_t h,
	IN byte_t channel,
	IN byte_t address,
	IN ipmi_req_t *req,
	OUT ipmi_rsp_t *rsp);

/*!
	Send a Node Manager command using the bridging parameters that
	were set with 

	Sends a command using the specified bridging channel and address
	that were set using ::NMPRK_SetDefaultNmCommandBridging

	\param[in] h The connection handle
	\param[in] req A pointer to the request data structure
	\param[out] rsp The response is stored in this data structure
	\return ::nmprk_status_t (NMPRK_SUCCESS if the function is successful)
*/
NMPRKC_API
nmprk_status_t
NMPRK_SendNmCommand(
	IN nmprk_conn_handle_t h,
	IN ipmi_req_t *req,
	OUT ipmi_rsp_t *rsp);

/*!
	Retrieve the last request/response exchange that was successful

	The API saves the last req/rsp. Commands that returned in error
	are not saved

	\param[in] h The connection handle
	\param[out] req The request data that was sent
	\param[out] rsp The response data that was received
	\return ::nmprk_status_t (NMPRK_SUCCESS if the function is successful)
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetLastRequestResponse(
	IN nmprk_conn_handle_t h,
	OUT ipmi_capture_req_t *req,
	OUT ipmi_capture_rsp_t *rsp);

/*!
	Get the string description for a specific error code

	\param[in] code The error code
	\param[out] str The string data, this buffer is provided by the caller
	\param[in] strsize The size of the string buffer supplied by the caller
	\return ::nmprk_status_t (NMPRK_SUCCESS if the function is successful)
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetErrorString(
	IN nmprk_status_t code, 
	IN OUT char_t *str,
	IN int_t strsize);

/*****************************************************************************/
// NMPRK Specification Functions (Commands from Specification)
/*****************************************************************************/
/*!
	Send the Enable/Disable Node Manager Policy Control command (C0h) and
	receive the response.

	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_EnableDisablePolicyControl(
	IN nmprk_conn_handle_t h,
	IN nm_enable_disable_policy_control_input_t *input);

/*!
	Send the Set Node Manager Policy command (C1h) and
	receive the response.

	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_SetPolicy(
	IN nmprk_conn_handle_t h,
	IN nm_set_policy_input_t *input);

/*!
	Send the Get Node Manager Policy command (C2h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\param[out] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetPolicy(
	IN nmprk_conn_handle_t h,
	IN nm_get_policy_input_t *input,
	OUT nm_get_policy_output_t *output);

/*!
	Send the Set Node Manager Policy Alert Thresholds command (C3h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_SetPolicyAlertThresholds(
	IN nmprk_conn_handle_t h,
	IN nm_set_policy_alert_thresholds_input_t *input);

/*!
	Send the Get Node Manager Policy Alert Thresholds command (C4h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\param[out] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetPolicyAlertThresholds(
	IN nmprk_conn_handle_t h,
	IN nm_get_policy_alert_thresholds_input_t *input,
	OUT nm_get_policy_alert_thresholds_output_t *output);

/*!
	Send the Set Node Manager Policy Suspend Periods command (C5h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_SetPolicySuspendPeriods(
	IN nmprk_conn_handle_t h,
	IN nm_set_policy_suspend_periods_input_t *input);

/*!
	Send the Get Node Manager Policy Suspend Periods command (C6h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\param[out] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetPolicySuspendPeriods(
	IN nmprk_conn_handle_t h,
	IN nm_get_policy_suspend_periods_input_t *input,
	OUT nm_get_policy_suspend_periods_output_t *output);

/*!
	Send the Reset Node Manager Statistics command (C7h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_ResetStatistics(
	IN nmprk_conn_handle_t h,
	IN nm_reset_statistics_input_t *input);

/*!
	Send the Get Node Manager Statistics command (C8h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\param[out] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetStatistics(
	IN nmprk_conn_handle_t h,
	IN nm_get_statistics_input_t *input,
	OUT nm_get_statistics_output_t *output);

/*!
	Send the Get Node Manager Capabilities command (C9h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\param[in] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetCapabilities(
	IN nmprk_conn_handle_t h,
	IN nm_get_capabilities_input_t *input,
	OUT nm_get_capabilities_output_t *output);

/*!
	Send the Get Node Manager Version command (CAh) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[out] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetVersion(
	IN nmprk_conn_handle_t h,
	OUT nm_get_version_output_t *output);

/*!
	Send the Set Node Manager Power Draw Range command (CBh) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_SetPowerDrawRange(
	IN nmprk_conn_handle_t h,
	IN nm_set_power_draw_range_input_t *input);

/*!
	Send the Set Node Manager Alert Destination command (CEh) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_SetAlertDestination(
	IN nmprk_conn_handle_t h,
	IN nm_set_alert_destination_input_t *input);

/*!
	Send the Get Node Manager Alert Destination command (CFh) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[out] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetAlertDestination(
	IN nmprk_conn_handle_t h,
	OUT nm_get_alert_destination_output_t *output);

/*!
	Send the Platform Characterization Launch Request (60h) and
	receive the response.

	Node Manager 3.0 or greater required
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_PlatformCharacterizationLaunchRequest(
	IN nmprk_conn_handle_t h,
	IN nm_platform_characterization_launch_req_input_t *input);

/*!
	Send the Get Node Manager Power Characterization Range (61h) and
	receive the response.

	Node Manager 3.0 or greater required
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\param[out] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetPowerCharacterizationRange(
	IN nmprk_conn_handle_t h,
	IN nm_get_nm_power_characterization_range_input_t *input,
	OUT nm_get_nm_power_characterization_range_output_t *output);

/*!
	Send the Get CUPS Capabilities command (64h) and
	receive the response.

	Node Manager 3.0 or greater required
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[out] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetCupsCapabilities(
	IN nmprk_conn_handle_t h,
	OUT nm_get_cups_capabilities_output_t *output);


/*!
	Send the Get CUPS Data command (65h) and
	receive the response.

	Node Manager 3.0 or greater required
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\param[out] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetCupsData(
	IN nmprk_conn_handle_t h,
	IN nm_get_cups_data_input_t *input,
	OUT nm_get_cups_data_output_t *output);

/*!
	Send the Set CUPS Configuration command (66h) and
	receive the response.

	Node Manager 3.0 or greater required
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\param[out] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_SetCupsConfiguration(
	IN nmprk_conn_handle_t h,
	IN nm_set_cups_configuration_input_t *input);

/*!
	Send the Get CUPS Configuration command (67h) and
	receive the response.

	Node Manager 3.0 or greater required
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[out] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetCupsConfiguration(
	IN nmprk_conn_handle_t h,
	OUT nm_get_cups_configuration_output_t *output);

/*!
	Send the Set CUPS Policies command (68h) and
	receive the response.

	Node Manager 3.0 or greater required
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_SetCupsPolicies(
	IN nmprk_conn_handle_t h,
	IN nm_set_cups_policies_input_t *input);

/*!
	Send the Get CUPS Policies command (69h) and
	receive the response.

	Node Manager 3.0 or greater required
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] input The input parameters for the command
	\param[out] output The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetCupsPolicies(
	IN nmprk_conn_handle_t h,
	IN nm_get_cups_policies_input_t *input,
	OUT nm_get_cups_policies_output_t *output);

/*****************************************************************************/
// NMPRK Advanced Functions
/*****************************************************************************/
/*!
	Get a list of all Node Manager policies on the server
	
	\param[in] h The connection handle
	\param[in,out] policies A pointer to a block of memory where the list of
							policies are to be stored
	\param[in,out] policiesSize The number of bytes of memory in the policies
								buffer
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API
nmprk_status_t
NMPRK_GetAllPolicies(
	IN nmprk_conn_handle_t h,
	IN OUT nm_policy_info_t *policies,
	IN OUT int_t *policiesSize);

/*!
	Get the Node Manager discovery parameters that are stored in the SDR 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?">Node Manager Specification</a> for more details.

	\param[in] h The connection handle
	\param[out] params The Node Manager parameters from the server
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t 
NMPRK_GetDiscoveryParameters(
	IN nmprk_conn_handle_t h, 
	OUT nm_discovery_parameters_t *params);

/*****************************************************************************/
// IPMI Functions
/*****************************************************************************/
/*!
	Send the Get Device ID command (NetFn: 06h Cmd: 01h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] devId The device ID info from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_GetDeviceId(
	IN nmprk_conn_handle_t h,
	OUT nm_ipmi_device_id_t *devId);

/*!
	Send the Get SDR Repository Info command (NetFn: 0Ah Cmd: 20h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[out] info The SDR info from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_GetSdrInfo(
	IN nmprk_conn_handle_t h,
	OUT nm_ipmi_repo_info_t *info);

/*!
	Send the Get SDR command (NetFn: 0Ah Cmd: 23h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] recordId The record ID
	\param[out] record The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_GetSdrRecord(
	IN nmprk_conn_handle_t h,
	IN ushort_t recordId,
	OUT ushort_t *nextRecord,
	OUT nm_ipmi_sdr_record_t *record);

/*!
	Send Add SDR command (NetFn: 0Ah Cmd: 24h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] record The record to add
	\param[out] recordId The record ID for the new record from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_AddSdrRecord(
	IN nmprk_conn_handle_t h,
	IN nm_ipmi_sdr_record_t *record,
	OUT ushort_t *recordId);

/*!
	Send the Delete SDR command (NetFn: 0Ah Cmd: 26h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] recordId The record to delete
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_DeleteSdrRecord(
	IN nmprk_conn_handle_t h,
	IN ushort_t recordId);

/*!
	Send the Clear SDR Repository command (NetFn: 0Ah Cmd: 27h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_ClearSdrRepository(
	IN nmprk_conn_handle_t h);

/*!
	Send the Get SEL Info command (NetFn: 0Ah Cmd: 40h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[out] info The SEL info from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_GetSelInfo(
	IN nmprk_conn_handle_t h,
	OUT nm_ipmi_repo_info_t *info);

/*!
	Send the Get SEL Entry command (NetFn: 0Ah Cmd: 43h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] entryId The Entry ID
	\param[out] entry The output parameters from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_GetSelEntry(
	IN nmprk_conn_handle_t h,
	IN ushort_t entryId,
	OUT ushort_t *nextRecord,
	OUT nm_ipmi_sel_entry_t *entry);

/*!
	Send the Delete SEL Entry command (NetFn: 0Ah Cmd: 46h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] entryId The entry to delete
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_DeleteSelEntry(
	IN nmprk_conn_handle_t h,
	IN ushort_t entryId);

/*!
	Send the Clear SEL command (NetFn: 0Ah Cmd: 47h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_ClearSel(
	IN nmprk_conn_handle_t h);

/*!
	Send the Get FRU Inventory Area Info command (NetFn: 0Ah Cmd: 10h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[out] info The FRU info from the command response
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_GetFruInfo(
	IN nmprk_conn_handle_t h,
	OUT nm_ipmi_fru_info_t *info);

/*!
	Send the Read FRU Data command (NetFn: 0Ah Cmd: 11h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] offset The offset into the inventory
	\param[in,out] length The number of bytes to read on input, the number
						  of bytes returned on output.
	\param[out] data The data buffer to store the results, must be greater
	                 than or equal to length
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_ReadFruData(
	IN nmprk_conn_handle_t h,
	IN int_t offset,
	IN OUT int_t *length,
	OUT byte_t *data);

/*!
	Send the Write FRU Data command (NetFn: 0Ah Cmd: 12h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[in] offset The offset into the inventory
	\param[in,out] length The number of bytes to write on input, the number
						  of bytes actually written on output.
	\param[in] data The data buffer that contains the buffer to write
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_WriteFruData(
	IN nmprk_conn_handle_t h,
	IN int_t offset,
	IN OUT int_t *length,
	IN byte_t *data);

/*!
	Send the Set ACPI Power State command (NetFn: 06h Cmd: 06h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[out] input The power state info to set
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_SetAcpiPowerState(
	IN nmprk_conn_handle_t h,
	IN ipmi_acpi_power_state_t *input);

/*!
	Send the Get ACPI Power State command (NetFn: 06h Cmd: 07h) and
	receive the response. 
	
	See the <a href="https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html">IPMI Specification</a> for more details.

	\param[in] h The connection handle
	\param[out] output The power state info returned
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
							 If a non-zero completion code is received that
							 completion code is the return value
*/
NMPRKC_API 
nmprk_status_t
NMPRK_IPMI_GetAcpiPowerState(
	IN nmprk_conn_handle_t h,
	OUT ipmi_acpi_power_state_t *output);

/*****************************************************************************/
// NMPRK Alert Functions
/*****************************************************************************/
/*!
	Register a handler to receive alerts 
	
	Note: This function is not implemented

	\param[in] h The connection handle
	\param[in] handler The handler function that will receive the alert
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
*/
NMPRKC_API 
nmprk_status_t
NMPRK_RegisterAlertHandler(
	IN nmprk_conn_handle_t h,
	IN alert_handler_t handler);

/*!
	Unregister a handler to receive alerts 
	
	Note: This function is not implemented

	\param[in] h The connection handle
	\param[in] handler The handler function to remove
	\return ::nmprk_status_t NMPRK_SUCCESS if the function is successful.
*/
NMPRKC_API 
nmprk_status_t
NMPRK_UnregisterAlertHandler(
	IN nmprk_conn_handle_t h,
	IN alert_handler_t handler);

#if defined __cplusplus
}
#endif

#endif //_NMPRK_C_H_

