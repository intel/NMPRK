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
#include "stdafx.h"

#include "nmprkC.h"
#include "nmprkUtilities.h"
#include "version.h"

//#include "transportInterfaceOriginal.h"
#include "transportInterfaceSlim.h"

std::fstream si_fsDebugLog;
si_debug_module_t si_debugModule;
si_debug_level_t si_debugLevel;
nmprk_debug_callback debugCallback = NULL;

#define SI_THIS_MODULE SI_DEBUG_MODULE_INTERFACE

#if defined __cplusplus
extern "C" 
{
#endif

//transportInterfaceOriginal transport;
transportInterfaceSlim transport;

NMPRKC_API const char *NMPRK_GetApiVersion()
{
	return PRODUCT_VERSION;
}

//Define the length of the Date,Time string
//#define MAX_DATE_STR_LEN 9

NMPRKC_API nmprk_status_t NMPRK_StartDebugLogging(
	const char *filename)
{
	char dateStr[MAX_DATE_STR_LEN];
	char timeStr[MAX_DATE_STR_LEN];

	if(si_fsDebugLog.is_open() == true)
		return NMPRK_FAILURE;

	try
	{
		si_debugModule = SI_DEBUG_MODULE_ALL;
		si_debugLevel = SI_DEBUG_LEVEL_ALL;
		si_fsDebugLog.open(filename, std::fstream::out | std::fstream::app);
		if(si_fsDebugLog.is_open() != true)
			return NMPRK_FAILURE;
	}
	catch (...)
	{
		return NMPRK_FAILURE;
	}

#if defined WIN32
	_strdate_s(dateStr, MAX_DATE_STR_LEN);
	_strtime_s(timeStr, MAX_DATE_STR_LEN);
#else
	time_t mytime = time(NULL);
	strftime(dateStr, 9, "%D", localtime(&mytime));
	strftime(timeStr, 9, "%T", localtime(&mytime));
#endif
		
	SI_DEBUG_INFO(SI_THIS_MODULE, "Debug Logging Started: %s %s", dateStr, timeStr);

	return NMPRK_SUCCESS;
}

NMPRKC_API nmprk_status_t NMPRK_StopDebugLogging()
{
	if(si_fsDebugLog.is_open() == true)
	{
		SI_DEBUG_INFO(SI_THIS_MODULE, "Debug Logging Stopped");
		si_fsDebugLog.close();
	}

	return NMPRK_SUCCESS;
}

NMPRKC_API nmprk_status_t NMPRK_SetDebugCallback(
	nmprk_debug_callback callback)
{
	si_debugModule = SI_DEBUG_MODULE_ALL;
	si_debugLevel = SI_DEBUG_LEVEL_ALL;
	debugCallback = callback;
	return NMPRK_SUCCESS;
}

NMPRKC_API nmprk_status_t NMPRK_ConnectLocal(
	OUT nmprk_conn_handle_t *h)
{
	return transport.ConnectLocal(h);
}

NMPRKC_API nmprk_status_t NMPRK_ConnectRemote(
	IN nmprk_connect_remote_parameters_t *input, 
	OUT nmprk_conn_handle_t *h)
{
	return transport.ConnectRemote(input, h);
}

NMPRKC_API nmprk_status_t NMPRK_Disconnect(
	IN nmprk_conn_handle_t h)
{
	return transport.Disconnect(h);
}

NMPRKC_API nmprk_status_t NMPRK_SetDefaultNmCommandBridging(
	IN nmprk_conn_handle_t h,
	IN byte_t channel,
	IN byte_t address)
{
	return transport.SetDefaultNmCommandBridging(h, channel, address);
}

NMPRKC_API nmprk_status_t NMPRK_SendNonBridgedCommand(
	IN nmprk_conn_handle_t h,
	IN ipmi_req_t *req,
	OUT ipmi_rsp_t *rsp)
{
	return transport.SendCommandInternal(h, false, req, rsp);
	//return NMPRK_SendCommandInternal(h, false, req, rsp);
}

NMPRKC_API
nmprk_status_t
NMPRK_SendBridgedCommand(
	IN nmprk_conn_handle_t h,
	IN byte_t channel,
	IN byte_t address,
	IN ipmi_req_t *req,
	OUT ipmi_rsp_t *rsp)
{
	return transport.SendBridgedCommand(h, channel, address, req, rsp);
}

NMPRKC_API nmprk_status_t NMPRK_SendNmCommand(
	IN nmprk_conn_handle_t h,
	IN ipmi_req_t *req,
	OUT ipmi_rsp_t *rsp)
{
	return transport.SendCommandInternal(h, true, req, rsp);
	//return NMPRK_SendCommandInternal(h, true, req, rsp);
}

NMPRKC_API
nmprk_status_t
NMPRK_GetLastRequestResponse(
	IN nmprk_conn_handle_t h,
	OUT ipmi_capture_req_t *req,
	OUT ipmi_capture_rsp_t *rsp)
{
	return transport.GetLastRequestResponse(h, req, rsp);
}

NMPRKC_API nmprk_status_t NMPRK_GetErrorString(
	IN nmprk_status_t code, 
	IN OUT char *str,
	IN int strsize)
{
	return transport.GetErrorString(code, str, strsize);
}

NMPRKC_API nmprk_status_t NMPRK_EnableDisablePolicyControl(
	IN nmprk_conn_handle_t h,
	IN nm_enable_disable_policy_control_input_t *input)
{
	if(input == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xc0);
	req.data[req.len++] = input->flags;
	req.data[req.len++] = input->domain;
	req.data[req.len++] = input->policy;

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
		return rsp.compCode;
	else
		return status;
}

NMPRKC_API nmprk_status_t NMPRK_SetPolicy(
	IN nmprk_conn_handle_t h,
	IN nm_set_policy_input_t *input)
{
	if(input == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xc1);
	req.data[req.len++] = input->info.domain | (input->info.policyEnabled << 4);
	req.data[req.len++] = input->info.policyId;
	req.data[req.len++] = input->info.policyTriggerType | ((!input->removePolicy) << 4) | (input->info.aggressiveCpuPowerCorrection << 5);
	req.data[req.len++] = input->info.policyExceptionActionSendAlert | (input->info.policyExceptionActionShutdown << 1) || (input->info.secondaryPowerDomain << 7);
	req.data[req.len++] = nmprkGetByte(input->info.policyTargetLimit, 0);
	req.data[req.len++] = nmprkGetByte(input->info.policyTargetLimit, 1);
	req.data[req.len++] = nmprkGetByte(input->info.correctionTimeLimit, 0);
	req.data[req.len++] = nmprkGetByte(input->info.correctionTimeLimit, 1);
	req.data[req.len++] = nmprkGetByte(input->info.correctionTimeLimit, 2);
	req.data[req.len++] = nmprkGetByte(input->info.correctionTimeLimit, 3);
	req.data[req.len++] = nmprkGetByte(input->info.policyTriggerLimit, 0);
	req.data[req.len++] = nmprkGetByte(input->info.policyTriggerLimit, 1);
	req.data[req.len++] = nmprkGetByte(input->info.statisticsReportingPeriod, 0);
	req.data[req.len++] = nmprkGetByte(input->info.statisticsReportingPeriod, 1);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
		return rsp.compCode;
	else
		return status;
}

NMPRKC_API nmprk_status_t NMPRK_GetPolicy(
	IN nmprk_conn_handle_t h,
	IN nm_get_policy_input_t *input,
	OUT nm_get_policy_output_t *output)
{
	if(input == NULL || output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xc2);
	req.data[req.len++] = input->domain & 0xF;
	req.data[req.len++] = input->policy;

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode == 0x00)
	{
		output->u.standard.info.domain = (nm_domain_id_t)(rsp.data[3] & 0xF);
		output->u.standard.info.policyEnabled = (rsp.data[3] >> 4) & 0x1;
		output->u.standard.perDomainPolicyControlEnabled = (rsp.data[3] >> 5) & 0x1;
		output->u.standard.globalPolicyControlEnabled = (rsp.data[3] >> 6) & 0x1;
		output->u.standard.policyCreatedByOtherClient = (rsp.data[3] >> 7) & 0x1;
		output->u.standard.info.policyTriggerType = (nm_policy_trigger_type_t)(rsp.data[4] & 0xF);
		output->u.standard.info.aggressiveCpuPowerCorrection = (nm_aggressive_cpu_power_correction_t)((rsp.data[4] >> 5) & 0x3);
		output->u.standard.info.policyExceptionActionSendAlert = rsp.data[5] & 0x1;
		output->u.standard.info.secondaryPowerDomain = (rsp.data[5] & 0x8) >> 7;
		output->u.standard.info.policyExceptionActionShutdown = (rsp.data[5] >> 1) & 0x1;
		output->u.standard.info.policyTargetLimit = rsp.data[6] | (rsp.data[7] << 8);
		output->u.standard.info.correctionTimeLimit = rsp.data[8] | (rsp.data[9] << 8) | (rsp.data[10] << 16) | (rsp.data[11] << 24);
		output->u.standard.info.policyTargetLimit = rsp.data[12] | (rsp.data[13] << 8);
		output->u.standard.info.statisticsReportingPeriod = rsp.data[14] | (rsp.data[15] << 8);
		
		// set the policyId to the policy that was requested
		output->u.standard.info.policyId = input->policy;
	}
	else if(rsp.compCode == 0x80)
	{
		if(rsp.len < 5)
			output->u.policy_id_invalid.valid = false;
		else
		{
			output->u.policy_id_invalid.valid = true;
			output->u.policy_id_invalid.nextValidPolicyId = rsp.data[3];
			output->u.policy_id_invalid.numberOfDefinedPoliciesForDomain = rsp.data[4];
		}
	}
	else if(rsp.compCode == 0x81)
	{
		if(rsp.len < 5)
			output->u.policy_id_invalid.valid = false;
		else
		{
			output->u.policy_id_invalid.valid = true;
			output->u.domain_id_invalid.nextValidDomainId = rsp.data[3];
			output->u.domain_id_invalid.numberOfAvailableDomains = rsp.data[4];
		}
	}

	return rsp.compCode;
}

NMPRKC_API nmprk_status_t NMPRK_SetPolicyAlertThresholds(
	IN nmprk_conn_handle_t h,
	IN nm_set_policy_alert_thresholds_input_t *input)
{
	if(input == NULL)
		return NMPRK_INVALID_ARGUMENT;

	if(input->numberOfAlertThresholds > 3)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xc3);
	req.data[req.len++] = input->domain & 0xF;
	req.data[req.len++] = input->policy;
	req.data[req.len++] = input->numberOfAlertThresholds;
	for(int i = 0; i < input->numberOfAlertThresholds; i++)
	{
		req.data[req.len++] = nmprkGetByte(input->alertThresholdsArray[i], 0);
		req.data[req.len++] = nmprkGetByte(input->alertThresholdsArray[i], 1);
	}

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
		return rsp.compCode;
	else
		return status;
}

NMPRKC_API nmprk_status_t NMPRK_GetPolicyAlertThresholds(
	IN nmprk_conn_handle_t h,
	IN nm_get_policy_alert_thresholds_input_t *input,
	OUT nm_get_policy_alert_thresholds_output_t *output)
{
	if(input == NULL || output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xc4);
	req.data[req.len++] = input->domain & 0xF;
	req.data[req.len++] = input->policy;

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode == 0x00)
	{
		if(rsp.len < 4)
			return NMPRK_INVALID_DATA_LENGTH;

		output->numberOfAlertThresholds = rsp.data[3];
		for(int i = 0; i < output->numberOfAlertThresholds && i < 3; i++)
		{
			output->alertThresholdsArray[i] = rsp.data[4 + (i * 2)] | (rsp.data[5 + (i * 2)] << 8);
		}
	}

	return rsp.compCode;
}

NMPRKC_API nmprk_status_t NMPRK_SetPolicySuspendPeriods(
	IN nmprk_conn_handle_t h,
	IN nm_set_policy_suspend_periods_input_t *input)
{
	if(input == NULL)
		return NMPRK_INVALID_ARGUMENT;

	if(input->numberOfSuspendPeriods > 5)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xc5);
	req.data[req.len++] = input->domain & 0xF;
	req.data[req.len++] = input->policy;
	req.data[req.len++] = input->numberOfSuspendPeriods;
	for(int i = 0; i < input->numberOfSuspendPeriods; i++)
	{
		req.data[req.len++] = input->suspendPeriods[i].startTime;
		req.data[req.len++] = input->suspendPeriods[i].stopTime;
		req.data[req.len++] = input->suspendPeriods[i].recurrencePattern;
	}

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
		return rsp.compCode;
	else
		return status;
}

NMPRKC_API nmprk_status_t NMPRK_GetPolicySuspendPeriods(
	IN nmprk_conn_handle_t h,
	IN nm_get_policy_suspend_periods_input_t *input,
	OUT nm_get_policy_suspend_periods_output_t *output)
{
	if(input == NULL || output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xc6);
	req.data[req.len++] = input->domain & 0xF;
	req.data[req.len++] = input->policy;

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode == 0x00)
	{
		if(rsp.len < 4)
			return NMPRK_INVALID_RSP_LENGTH;

		output->numberOfSuspendPeriods = rsp.data[3];
		for(int i = 0; i < output->numberOfSuspendPeriods && i < 5; i++)
		{
			output->suspendPeriods[i].startTime = rsp.data[4 + (i * 3)];
			output->suspendPeriods[i].stopTime = rsp.data[5 + (i * 3)];
			output->suspendPeriods[i].recurrencePattern = (nm_suspend_period_recurrence_pattern_t)rsp.data[6 + (i * 3)];
		}
	}

	return rsp.compCode;
}

NMPRKC_API nmprk_status_t NMPRK_ResetStatistics(
	IN nmprk_conn_handle_t h,
	IN nm_reset_statistics_input_t *input)
{
	if(input == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xc7);
	req.data[req.len++] = input->mode;
	req.data[req.len++] = input->domain & 0xF;
	req.data[req.len++] = input->policy;

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
		return rsp.compCode;
	else
		return status;
}

NMPRKC_API nmprk_status_t NMPRK_GetStatistics(
	IN nmprk_conn_handle_t h,
	IN nm_get_statistics_input_t *input,
	OUT nm_get_statistics_output_t *output)
{
	if(input == NULL || output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xc8);
	req.data[req.len++] = input->mode;
	req.data[req.len++] = input->domain & 0xF;
	req.data[req.len++] = input->policy;

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode == 0x00)
	{
		if(rsp.len != 20)
			return NMPRK_INVALID_RSP_LENGTH;

		output->currentValue = rsp.data[3] | (rsp.data[4] << 8);
		output->minimumValue = rsp.data[5] | (rsp.data[6] << 8);
		output->maximumValue = rsp.data[7] | (rsp.data[8] << 8);
		output->averageValue = rsp.data[9] | (rsp.data[10] << 8);
		output->timestamp = rsp.data[11] | (rsp.data[12] << 8) | (rsp.data[13] << 16) | (rsp.data[14] << 24);
		output->statisticsReportingPeriod = rsp.data[15] | (rsp.data[16] << 8) | (rsp.data[17] << 16) | (rsp.data[18] << 24);
		output->domain = (nm_domain_id_t)(rsp.data[19] & 0xF);
		output->policyGlobalAdministrativeState = (rsp.data[19] & 0x10) >> 4;
		output->policyOperationalState = (rsp.data[19] & 0x20) >> 5;
		output->measurementsState = (rsp.data[19] & 0x40) >> 6;
		output->policyActivationState = (rsp.data[19] & 0x80) >> 7;
	}

	return rsp.compCode;
}

NMPRKC_API nmprk_status_t NMPRK_GetCapabilities(
	IN nmprk_conn_handle_t h,
	IN nm_get_capabilities_input_t *input,
	OUT nm_get_capabilities_output_t *output)
{
	if(input == NULL || output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xc9);
	req.data[req.len++] = input->domain & 0xF;
	req.data[req.len++] = input->policyTriggerType | 0x10;

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode == 0x00)
	{
		if(rsp.len != 21)
			return NMPRK_INVALID_RSP_LENGTH;

		output->maxConcurrentSettings = rsp.data[3];
		output->maxValue = rsp.data[4] | (rsp.data[5] << 8);
		output->minValue = rsp.data[6] | (rsp.data[7] << 8);
		output->minCorrectionTime = rsp.data[8] | (rsp.data[9] << 8) | (rsp.data[10] << 16) | (rsp.data[11] << 24);
		output->maxCorrectionTime = rsp.data[12] | (rsp.data[13] << 8) | (rsp.data[14] << 16) | (rsp.data[15] << 24);
		output->minStatisticsReportingPeriod = rsp.data[16] | (rsp.data[17] << 8);
		output->maxStatisticsReportingPeriod = rsp.data[18] | (rsp.data[19] << 8);
		output->domainLimitingScope = (nm_domain_id_t)(rsp.data[20] & 0xF);
		output->limitingBasedOn = (rsp.data[20] & 0x80) >> 7;
	}

	return rsp.compCode;
}

NMPRKC_API nmprk_status_t NMPRK_GetVersion(
	IN nmprk_conn_handle_t h,
	OUT nm_get_version_output_t *output)
{
	if(output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xca);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode == 0x00)
	{
		if(rsp.len != 8)
			return NMPRK_INVALID_RSP_LENGTH;

		output->version = rsp.data[3];
		output->ipmiVersion = rsp.data[4];
		output->patchVersion = rsp.data[5];
		output->majorFirmwareRevision = rsp.data[6];
		output->minorFirmwareRevision = rsp.data[7];
	}

	return rsp.compCode;
}

NMPRKC_API nmprk_status_t NMPRK_SetPowerDrawRange(
	IN nmprk_conn_handle_t h,
	IN nm_set_power_draw_range_input_t *input)
{
	if(input == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xcb);
	req.data[req.len++] = input->domain & 0xF;
	req.data[req.len++] = nmprkGetByte(input->minimumPowerDraw, 0);
	req.data[req.len++] = nmprkGetByte(input->minimumPowerDraw, 1);
	req.data[req.len++] = nmprkGetByte(input->maximumPowerDraw, 0);
	req.data[req.len++] = nmprkGetByte(input->maximumPowerDraw, 1);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
		return rsp.compCode;
	else
		return status;
}

NMPRKC_API nmprk_status_t NMPRK_SetAlertDestination(
	IN nmprk_conn_handle_t h,
	IN nm_set_alert_destination_input_t *input)
{
	if(input == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xce);
	req.data[req.len++] = input->alertDestination.channelNumber | (input->alertDestination.destinationInformationReceiver << 7);
	req.data[req.len++] = input->alertDestination.destinationInformation.destinationSelector;
	req.data[req.len++] = input->alertDestination.alertStringSelector | (input->alertDestination.sendAlertString << 7);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
		return rsp.compCode;
	else
		return status;
}

NMPRKC_API nmprk_status_t NMPRK_GetAlertDestination(
	IN nmprk_conn_handle_t h,
	OUT nm_get_alert_destination_output_t *output)
{
	if(output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0xcf);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode == 0x00)
	{
		if(rsp.len != 6)
			return NMPRK_INVALID_RSP_LENGTH;

		output->alertDestination.channelNumber = rsp.data[3] & 0xF;
		output->alertDestination.destinationInformationReceiver = (rsp.data[3] & 0x80) >> 7;
		output->alertDestination.destinationInformation.destinationSelector = rsp.data[4];
		output->alertDestination.alertStringSelector = rsp.data[5] & 0x7F;
		output->alertDestination.sendAlertString = (rsp.data[5] & 0x80) >> 7;
	}

	return rsp.compCode;
}

NMPRKC_API
nmprk_status_t
NMPRK_PlatformCharacterizationLaunchRequest(
	IN nmprk_conn_handle_t h,
	IN nm_platform_characterization_launch_req_input_t *input)
{
	if(input == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0x60);
	req.data[req.len++] = (input->launchAction & 0x03) | ((input->bmcTableConfigPhaseAction << 2) & 0x0C) | ((input->bmcPhaseStateMachineAction << 4) & 0x30);
	req.data[req.len++] = input->powerDomainId;
	req.data[req.len++] = input->powerDrawCharacterizationPoint;
	req.data[req.len++] = nmprkGetByte(input->delay, 0);
	req.data[req.len++] = nmprkGetByte(input->delay, 1);
	req.data[req.len++] = nmprkGetByte(input->delay, 2);
	req.data[req.len++] = nmprkGetByte(input->delay, 3);
	req.data[req.len++] = nmprkGetByte(input->timeToRun, 0);
	req.data[req.len++] = nmprkGetByte(input->timeToRun, 1);
	req.data[req.len++] = nmprkGetByte(input->timeToRun, 2);
	req.data[req.len++] = nmprkGetByte(input->timeToRun, 3);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
		return rsp.compCode;
	else
		return status;
}

NMPRKC_API
nmprk_status_t
NMPRK_GetPowerCharacterizationRange(
	IN nmprk_conn_handle_t h,
	IN nm_get_nm_power_characterization_range_input_t *input,
	OUT nm_get_nm_power_characterization_range_output_t *output)
{
	if(input == NULL || output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0x61);
	req.data[req.len++] = input->domainId;

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode == 0x00)
	{
		if(rsp.len != 13)
			return NMPRK_INVALID_RSP_LENGTH;

		output->timestamp = rsp.data[3] | ((ulong_t)rsp.data[4] << 8) | ((ulong_t)rsp.data[5] << 16) | ((ulong_t)rsp.data[6] << 24);
		output->maxPowerDraw = rsp.data[7] | ((ulong_t)rsp.data[8] << 8);
		output->minPowerDraw = rsp.data[9] | ((ulong_t)rsp.data[10] << 8);
		output->effPowerDraw = rsp.data[11] | ((ulong_t)rsp.data[12] << 8);
	}

	return rsp.compCode;
}

NMPRKC_API
nmprk_status_t
NMPRK_GetCupsCapabilities(
	IN nmprk_conn_handle_t h,
	OUT nm_get_cups_capabilities_output_t *output)
{
	if(output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0x64);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode == 0x00)
	{
		if(rsp.len != 7)
			return NMPRK_INVALID_RSP_LENGTH;

		output->cupsEnabled = rsp.data[3] & 0x01;
		output->cupsPoliciesAvailable = rsp.data[4] & 0x01;
		output->cupsVersion = rsp.data[5];
		output->reserved = rsp.data[6];
	}

	return rsp.compCode;
}

NMPRKC_API
nmprk_status_t
NMPRK_GetCupsData(
	IN nmprk_conn_handle_t h,
	IN nm_get_cups_data_input_t *input,
	OUT nm_get_cups_data_output_t *output)
{
	if(input == NULL || output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0x65);
	req.data[req.len++] = input->parameter;

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode == 0x00)
	{
		if((input->parameter == CUPS_PARAMETER_INDEX && rsp.len != 5) ||
			(input->parameter == CUPS_PARAMETER_DYNAMIC && rsp.len != 9) ||
			(input->parameter == CUPS_PARAMETER_BASE && rsp.len != 27) ||
			(input->parameter == CUPS_PARAMETER_AGGREGATE && rsp.len != 27))
			return NMPRK_INVALID_RSP_LENGTH;

		switch(input->parameter)
		{
		case CUPS_PARAMETER_INDEX:
			output->data.index.index = rsp.data[3] | (rsp.data[4] << 8);
			break;
		case CUPS_PARAMETER_DYNAMIC:
			output->data.dynamic.cpu = rsp.data[3] | (rsp.data[4] << 8);
			output->data.dynamic.memory = rsp.data[5] | (rsp.data[6] << 8);
			output->data.dynamic.io = rsp.data[7] | (rsp.data[8] << 8);
			break;
		case CUPS_PARAMETER_BASE:
			output->data.base.cpu = rsp.data[3] | ((ulong_t)rsp.data[4] << 8) | ((ulong_t)rsp.data[5] << 16) | ((ulong_t)rsp.data[6] << 24) | 
									((ulong_t)rsp.data[7] << 32) | ((ulong_t)rsp.data[8] << 40) | ((ulong_t)rsp.data[9] << 48) | ((ulong_t)rsp.data[10] << 56);
			output->data.base.memory = rsp.data[11] | ((ulong_t)rsp.data[12] << 8) | ((ulong_t)rsp.data[13] << 16) | ((ulong_t)rsp.data[14] << 24) | 
									((ulong_t)rsp.data[15] << 32) | ((ulong_t)rsp.data[16] << 40) | ((ulong_t)rsp.data[17] << 48) | ((ulong_t)rsp.data[18] << 56);
			output->data.base.io = rsp.data[19] | ((ulong_t)rsp.data[20] << 8) | ((ulong_t)rsp.data[21] << 16) | ((ulong_t)rsp.data[22] << 24) | 
									((ulong_t)rsp.data[23] << 32) | ((ulong_t)rsp.data[24] << 40) | ((ulong_t)rsp.data[25] << 48) | ((ulong_t)rsp.data[26] << 56);
			break;
		case CUPS_PARAMETER_AGGREGATE:
			output->data.aggregate.cpu = rsp.data[3] | ((ulong_t)rsp.data[4] << 8) | ((ulong_t)rsp.data[5] << 16) | ((ulong_t)rsp.data[6] << 24) | 
									((ulong_t)rsp.data[7] << 32) | ((ulong_t)rsp.data[8] << 40) | ((ulong_t)rsp.data[9] << 48) | ((ulong_t)rsp.data[10] << 56);
			output->data.aggregate.memory = rsp.data[11] | ((ulong_t)rsp.data[12] << 8) | ((ulong_t)rsp.data[13] << 16) | ((ulong_t)rsp.data[14] << 24) | 
									((ulong_t)rsp.data[15] << 32) | ((ulong_t)rsp.data[16] << 40) | ((ulong_t)rsp.data[17] << 48) | ((ulong_t)rsp.data[18] << 56);
			output->data.aggregate.io = rsp.data[19] | ((ulong_t)rsp.data[20] << 8) | ((ulong_t)rsp.data[21] << 16) | ((ulong_t)rsp.data[22] << 24) | 
									((ulong_t)rsp.data[23] << 32) | ((ulong_t)rsp.data[24] << 40) | ((ulong_t)rsp.data[25] << 48) | ((ulong_t)rsp.data[26] << 56);
			break;
		}
	}

	return rsp.compCode;
}

NMPRKC_API
nmprk_status_t
NMPRK_SetCupsConfiguration(
	IN nmprk_conn_handle_t h,
	IN nm_set_cups_configuration_input_t *input)
{
	if(input == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0x66);
	req.data[req.len++] = input->config.cupsEnabled & 0x01;
	req.data[req.len++] = (input->setCoreLoadFactor & 0x1) | ((input->setMemoryLoadFactor << 1) & 0x2) | ((input->setIoLoadFactor >> 2) & 0x4);
	req.data[req.len++] = nmprkGetByte(input->config.staticCoreLoadFactor, 0);
	req.data[req.len++] = nmprkGetByte(input->config.staticCoreLoadFactor, 1);
	req.data[req.len++] = nmprkGetByte(input->config.staticMemoryLoadFactor, 0);
	req.data[req.len++] = nmprkGetByte(input->config.staticMemoryLoadFactor, 1);
	req.data[req.len++] = nmprkGetByte(input->config.staticIoLoadFactor, 0);
	req.data[req.len++] = nmprkGetByte(input->config.staticIoLoadFactor, 1);
	req.data[req.len++] = input->config.sampleCount;

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
		return rsp.compCode;
	else
		return status;
}

NMPRKC_API
nmprk_status_t
NMPRK_GetCupsConfiguration(
	IN nmprk_conn_handle_t h,
	OUT nm_get_cups_configuration_output_t *output)
{
	if(output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0x67);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode == 0x00)
	{
		if(rsp.len != 11)
		// Sample Count does not exist in NM 3.0 v1.0, add in v1.0.1
		//if(rsp.len != 12)
			return NMPRK_INVALID_RSP_LENGTH;

		output->config.cupsEnabled = rsp.data[3] & 0x01;
		output->config.loadFactorTypeToggle = rsp.data[4] & 0x01;
		output->config.staticCoreLoadFactor = rsp.data[5] | (rsp.data[6] << 8);
		output->config.staticCoreLoadFactor = rsp.data[7] | (rsp.data[8] << 8);
		output->config.staticCoreLoadFactor = rsp.data[9] | (rsp.data[10] << 8);
		//output->config.sampleCount = rsp.data[11];
	}

	return rsp.compCode;
}

NMPRKC_API
nmprk_status_t
NMPRK_SetCupsPolicies(
	IN nmprk_conn_handle_t h,
	IN nm_set_cups_policies_input_t *input)
{
	if(input == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0x68);
	req.data[req.len++] = 0;
	req.data[req.len++] = (input->policyId & 0xF) | ((input->target << 4) & 0xF0);
	req.data[req.len++] = input->policy.policyEnabled & 0x01;
	req.data[req.len++] = (input->policy.policyStorageVolatileMemory << 7) & 0x80;
	req.data[req.len++] = input->policy.sendAlertEnabled & 0x01;
	req.data[req.len++] = input->policy.cupsThreshold;
	req.data[req.len++] = nmprkGetByte(input->policy.averagingWindow, 0);
	req.data[req.len++] = nmprkGetByte(input->policy.averagingWindow, 1);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
		return rsp.compCode;
	else
		return status;
}

NMPRKC_API
nmprk_status_t
NMPRK_GetCupsPolicies(
	IN nmprk_conn_handle_t h,
	IN nm_get_cups_policies_input_t *input,
	OUT nm_get_cups_policies_output_t *output)
{
	if(input == NULL || output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeRequest(req, 0x2e, 0x69);
	req.data[req.len++] = 0;
	req.data[req.len++] = (input->policyId & 0xF) | ((input->target << 4) & 0xF0);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNmCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode == 0x00)
	{
		if(rsp.len != 9)
			return NMPRK_INVALID_RSP_LENGTH;

		output->policy.policyEnabled = rsp.data[3] & 0x01;
		output->policy.policyStorageVolatileMemory = (rsp.data[4] >> 7) & 0x01;
		output->policy.sendAlertEnabled = rsp.data[5] & 0x01;
		output->policy.cupsThreshold = rsp.data[6];
		output->policy.averagingWindow = rsp.data[7] | (rsp.data[8] << 8);
	}

	return rsp.compCode;
}

NMPRKC_API
nmprk_status_t
NMPRK_GetAllPolicies(
	IN nmprk_conn_handle_t h,
	IN OUT nm_policy_info_t *policies,
	IN OUT int *policiesSize)
{
	nmprk_status_t status;
	nm_get_policy_input_t gpIn;
	nm_get_policy_output_t gpOut;
	bool writePolicies = true;
	int policiesCount = 0;

	memset(&gpIn, 0, sizeof(gpIn));
	memset(&gpOut, 0, sizeof(gpOut));

	if(policiesSize == NULL)
		return NMPRK_INVALID_ARGUMENT;

	if(policies == NULL || *policiesSize == 0)
		writePolicies = false;

	int numPoliciesAvailable = *policiesSize / sizeof(nm_policy_info_t);

	byte_t version = 0x00;
	status = nmprkGetNodeManagerVersion(h, &version);
	if(status != NMPRK_SUCCESS)
		return status;

	if (version <= 0x02) // NM 1.5
    {
        for (byte_t i = 0; i <= 0xF; i++) // search a max of 16 policies
        {
			gpIn.domain = PLATFORM;
			gpIn.policy = i;
			status = NMPRK_GetPolicy(h, &gpIn, &gpOut);
			if(status == 0x00)
			{
				if(policiesCount < numPoliciesAvailable && writePolicies)
					memcpy(&policies[policiesCount], &gpOut.u.standard.info, sizeof(nm_policy_info_t));
				policiesCount++;
			}
			else if(status == 0x80)
				continue;
			else
				return status;
        }
    }
    else // >= NM 2.0
    {
		nm_domain_id_t maxDomain = LAST_DOMAIN;
		switch(version)
		{
			// Add NM 2.5 and 3.0 here
		case 0x03:
			maxDomain = MEMORY;
			break;
		default:
			maxDomain = LAST_DOMAIN;
			break;
		}

		gpIn.domain = (nm_domain_id_t)0;
		gpIn.policy = 0;

		do
		{
			status = NMPRK_GetPolicy(h, &gpIn, &gpOut);
			if(status == 0x00)
			{
				if(policiesCount < numPoliciesAvailable && writePolicies)
					memcpy(&policies[policiesCount], &gpOut.u.standard.info, sizeof(nm_policy_info_t));
				policiesCount++;

				gpIn.policy++;
				if(gpIn.policy == 0)
					gpIn.domain = (nm_domain_id_t)(gpIn.domain + 1);
			}
			else if(status == 0x80) // invalid policy id
			{
				if(gpOut.u.policy_id_invalid.valid == true)
				{
					gpIn.policy = gpOut.u.policy_id_invalid.nextValidPolicyId;
					if(gpIn.policy == 0)
						gpIn.domain = (nm_domain_id_t)((int)gpIn.domain + 1);
				}
			}
			else if(status == 0x81) // invalid domain id
			{
				if(gpOut.u.domain_id_invalid.valid == true)
				{
					gpIn.domain = (nm_domain_id_t)gpOut.u.domain_id_invalid.nextValidDomainId;
					gpIn.policy = 0;
				}
			}
			else
			{
				return status;
			}
		}
        while (gpIn.domain <= maxDomain && (gpIn.domain > 0 || gpIn.policy > 0));
    }

	*policiesSize = policiesCount * sizeof(nm_policy_info_t);
	return NMPRK_SUCCESS;
}

NMPRKC_API nmprk_status_t NMPRK_GetDiscoveryParameters(
	IN nmprk_conn_handle_t h, 
	OUT nm_discovery_parameters_t *params)
{
	nmprk_status_t status;
	bool foundDiscovery = false;
	ushort_t nextRecord = 0;

	if(params == NULL)
		return NMPRK_INVALID_ARGUMENT;

	do
	{
		nm_ipmi_sdr_record_t record;
		ushort_t recordId = nextRecord;

		status = NMPRK_IPMI_GetSdrRecord(h, recordId, &nextRecord, &record);
		if(status != NMPRK_SUCCESS)
			return status;

		if (record.type == 0xC0) // OEM record
		{
			// first 3 bytes are Intel ID 0x570100 (0,1,2)
			byte_t recordSubtype = record.data[3];

			SI_DEBUG_INFO(SI_THIS_MODULE, "Record SubType: 0x%x", (int) recordSubtype);

			// Node Manager Discovery SubType
			if (recordSubtype == 0x0D)
			{
				// version - 1 byte
				// I2C Slave address of NM
				params->address = record.data[5];
				params->channel = record.data[6] >> 4;
				params->nmHealthEvSensor = record.data[7];
				params->nmExceptionEvSensor = record.data[8];
				params->nmOperationalCapSensor = record.data[9];
				params->nmAlertThresExcdSensor = record.data[10];

				SI_DEBUG_INFO(SI_THIS_MODULE, "NM Address: 0x%x", (int) params->address);
				SI_DEBUG_INFO(SI_THIS_MODULE, "NM Channel: 0x%x", (int) params->channel);
				foundDiscovery = true;

				// this is the only record we want right now, break out of loop
				nextRecord = 0xFFFF;
			}
		}
	}
	while (nextRecord != 0xFFFF);

	if(foundDiscovery == true)
		return NMPRK_SUCCESS;
	else
		return NMPRK_NOT_FOUND;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_GetDeviceId(
	IN nmprk_conn_handle_t h,
	OUT nm_ipmi_device_id_t *devId)
{
	if(devId == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeIpmiRequest(req, 0x06, 0x01);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
	{
		devId->deviceId = rsp.data[0];
		devId->deviceProvidesSdr = ((rsp.data[1] & 0x80) >> 7);
		devId->deviceRev = rsp.data[1] & 0x0F;
		devId->devNormOp = ((rsp.data[2] & 0x80) >> 7);
		devId->firmwareRev = rsp.data[2] & 0x3F;
		devId->firmwareRev2 = rsp.data[3];
		devId->ipmiVersion = rsp.data[4];
		devId->isChassisDev = ((rsp.data[5] & 0x80) >> 7);
		devId->isBridgeDev = ((rsp.data[5] & 0x40) >> 6);
		devId->isIpmbGenDev = ((rsp.data[5] & 0x20) >> 5);
		devId->isIpmbRevDev = ((rsp.data[5] & 0x10) >> 4);
		devId->isFruInvDev = ((rsp.data[5] & 0x08) >> 3);
		devId->isSelDev = ((rsp.data[5] & 0x04) >> 2);
		devId->isSdrRepoDev = ((rsp.data[5] & 0x02) >> 1);
		devId->isSensorDev = rsp.data[5] & 0x01;
		devId->manufId[0] = rsp.data[6];
		devId->manufId[1] = rsp.data[7];
		devId->manufId[2] = rsp.data[8];
		devId->productId[0] = rsp.data[9];
		devId->productId[1] = rsp.data[10];
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_GetSdrInfo(
	IN nmprk_conn_handle_t h,
	OUT nm_ipmi_repo_info_t *info)
{
	struct tm* ti = NULL;
	time_t tt;
	time_t* T = new time_t();

	if(info == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeIpmiRequest(req, 0x0A, 0x20);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
	{
		info->repoVersion = rsp.data[0];
		info->repoEntries = rsp.data[1] | (rsp.data[2] << 8);
		info->repoFreeSpace = rsp.data[3] | (rsp.data[4] << 8);

		tt = (time_t)(rsp.data[5] | (rsp.data[6] << 8) | (rsp.data[7] << 16) | (rsp.data[8] << 24));
		*T = tt;
		ti = localtime(T);
		info->mostRecentAddTS = *ti;

		tt = (time_t)(rsp.data[9] | (rsp.data[10] << 8) | (rsp.data[11] << 16) | (rsp.data[12] << 24));
		*T = tt;
		ti = localtime(T);
		info->mostRecentDelTS = *ti;

		info->modalSupported = (rsp.data[13] & 0x40) >> 6;
		info->nonmodalSupported = (rsp.data[13] & 0x20) >> 5;
		info->delSup = (rsp.data[13] & 0x08) >> 3;
		info->parAddSup = (rsp.data[13] & 0x04) >> 2;
		info->reserveSup = (rsp.data[13] & 0x02) >> 1;
		info->getAllocInfoSup = rsp.data[13] & 0x01;
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_GetSdrRecordHeader(
	IN nmprk_conn_handle_t h,
	IN ushort_t recordId,
	OUT ushort_t *nextRecord,
	OUT nm_ipmi_sdr_record_t *record)
{
	ipmi_req_t req;
	ipmi_rsp_t rsp;

	if(nextRecord == NULL || record == NULL)
		return NMPRK_INVALID_ARGUMENT;

	req.netFun = 0x0A; 
	req.cmd = 0x23;
	req.rsAddr = BMC_ADDRESS;
	req.rsLun = 0;
	req.len = 0;
	req.data[req.len++] = 0x00; // reservation id ls
    req.data[req.len++] = 0x00; // reservation id ms
    req.data[req.len++] = recordId & 0x00FF; // record id ls
    req.data[req.len++] = (recordId & 0xFF00) >> 8; // record id ms
    req.data[req.len++] = 0x00; // offset
    req.data[req.len++] = 0x05; // bytes to read, read the record header

	nmprk_status_t status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode != 0x00)
		return rsp.compCode;

	if(rsp.len < 2)
		return NMPRK_INVALID_RSP_LENGTH;

	*nextRecord = rsp.data[0] | (rsp.data[1] << 8);
	if(rsp.len >= 4)
		record->recordId = rsp.data[2] | (rsp.data[3] << 8);
	if(rsp.len >= 5)
		record->version = rsp.data[4];
	if(rsp.len >= 6)
		record->type = rsp.data[5];
	if(rsp.len >= 7)
		record->len = rsp.data[6];
	
	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_GetSdrRecord(
	IN nmprk_conn_handle_t h,
	IN ushort_t recordId,
	OUT ushort_t *nextRecord,
	OUT nm_ipmi_sdr_record_t *record)
{
	ipmi_req_t req;
	ipmi_rsp_t rsp;

	if(nextRecord == NULL || record == NULL)
		return NMPRK_INVALID_ARGUMENT;

	req.netFun = 0x0A; 
	req.cmd = 0x23;
	req.rsAddr = BMC_ADDRESS;
	req.rsLun = 0;
	req.len = 0;
	req.data[req.len++] = 0x00; // reservation id ls
    req.data[req.len++] = 0x00; // reservation id ms
    req.data[req.len++] = recordId & 0x00FF; // record id ls
    req.data[req.len++] = (recordId & 0xFF00) >> 8; // record id ms
    req.data[req.len++] = 0x00; // offset
    req.data[req.len++] = 0xFF; // bytes to read, read the whole thing, partial reads not supported yet

	nmprk_status_t status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status != NMPRK_SUCCESS)
		return status;

	if(rsp.compCode != 0x00)
		return rsp.compCode;

	if(rsp.len < 2)
		return NMPRK_INVALID_RSP_LENGTH;

	*nextRecord = rsp.data[0] | (rsp.data[1] << 8);
	if(rsp.len >= 4)
		record->recordId = rsp.data[2] | (rsp.data[3] << 8);
	if(rsp.len >= 5)
		record->version = rsp.data[4];
	if(rsp.len >= 6)
		record->type = rsp.data[5];
	if(rsp.len >= 7)
		record->len = rsp.data[6];
	if(record->len > 0)
		memcpy(&record->data[0], &rsp.data[7], rsp.len - 7);

	return NMPRK_SUCCESS;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_AddSdrRecord(
	IN nmprk_conn_handle_t h,
	IN nm_ipmi_sdr_record_t *record,
	OUT ushort_t *recordId)
{
	nmprk_status_t status = NMPRK_FAILURE;
	ipmi_req_t req;
	ipmi_rsp_t rsp;

	if(record == NULL || recordId == NULL)
		return NMPRK_INVALID_ARGUMENT;

	nmprkInitializeIpmiRequest(req, 0x0A, 0x24);
	req.data[req.len++] = record->version;
	req.data[req.len++] = record->type;
	req.data[req.len++] = record->len;
	for(int i = 0; i < record->len; i++)
		req.data[req.len++] = record->data[i];

	status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
	{
		if(rsp.len != 2)
			return NMPRK_INVALID_RSP_LENGTH;

		*recordId = rsp.data[0] | (rsp.data[1] << 8);
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_ReserveSdr(
	IN nmprk_conn_handle_t h,
	OUT ushort_t *resId)
{
	nmprk_status_t status = NMPRK_FAILURE;
	ipmi_req_t req;
	ipmi_rsp_t rsp;

	if(resId == NULL)
		return NMPRK_INVALID_ARGUMENT;

	nmprkInitializeIpmiRequest(req, 0x0A, 0x22);

	status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
	{
		if(rsp.len != 2)
			status = NMPRK_INVALID_RSP_LENGTH;
		else 
			*resId = rsp.data[0] && (rsp.data[1] << 8);
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_DeleteSdrRecord(
	IN nmprk_conn_handle_t h,
	IN ushort_t recordId)
{
	nmprk_status_t status = NMPRK_FAILURE;
	ipmi_req_t req;
	ipmi_rsp_t rsp;
	ushort_t resId;

	status = NMPRK_IPMI_ReserveSdr(h, &resId);
	if(status == NMPRK_SUCCESS)
	{
		nmprkInitializeIpmiRequest(req, 0x0A, 0x26);
		req.data[req.len++] = nmprkGetByte(resId, 0);
		req.data[req.len++] = nmprkGetByte(resId, 1);
		req.data[req.len++] = nmprkGetByte(recordId, 0);
		req.data[req.len++] = nmprkGetByte(recordId, 1);

		status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_ClearSdrRepository(
	IN nmprk_conn_handle_t h)
{
	ipmi_req_t req;
	ipmi_rsp_t rsp;

	req.netFun = 0x0A; 
	req.cmd = 0x27;
	req.rsAddr = BMC_ADDRESS;
	req.rsLun = 0;
	req.len = 0;
	req.data[req.len++] = 0x00; // reservation id ls
    req.data[req.len++] = 0x00; // reservation id ms
    req.data[req.len++] = 0x43;
    req.data[req.len++] = 0x4C;
    req.data[req.len++] = 0x52;
    req.data[req.len++] = 0xAA;

	return NMPRK_SendNonBridgedCommand(h, &req, &rsp);
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_GetSelInfo(
	IN nmprk_conn_handle_t h,
	OUT nm_ipmi_repo_info_t *info)
{
	struct tm* ti = NULL;
	time_t tt;
	time_t* T = new time_t();

	if(info == NULL)
		return NMPRK_INVALID_ARGUMENT;

	ipmi_req_t req;
	nmprkInitializeIpmiRequest(req, 0x0A, 0x40);

	ipmi_rsp_t rsp;
	nmprk_status_t status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
	{
		if(rsp.len < 14)
			return NMPRK_INVALID_RSP_LENGTH;

		info->repoVersion = rsp.data[0];
		info->repoEntries = rsp.data[1] | (rsp.data[2] << 8);
		info->repoFreeSpace = rsp.data[3] | (rsp.data[4] << 8);

		tt = (time_t)(rsp.data[5] | (rsp.data[6] << 8) | (rsp.data[7] << 16) | (rsp.data[8] << 24));
		*T = tt;
		ti = localtime(T);
		info->mostRecentAddTS = *ti;

		tt = (time_t)(rsp.data[9] | (rsp.data[10] << 8) | (rsp.data[11] << 16) | (rsp.data[12] << 24));
		*T = tt;
		ti = localtime(T);
		info->mostRecentDelTS = *ti;

		info->modalSupported = (rsp.data[13] & 0x40) >> 6;
		info->nonmodalSupported = (rsp.data[13] & 0x20) >> 5;
		info->delSup = (rsp.data[13] & 0x08) >> 3;
		info->parAddSup = (rsp.data[13] & 0x04) >> 2;
		info->reserveSup = (rsp.data[13] & 0x02) >> 1;
		info->getAllocInfoSup = rsp.data[13] & 0x01;
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_ReserveSel(
	IN nmprk_conn_handle_t h,
	OUT ushort_t *resId)
{
	nmprk_status_t status = NMPRK_FAILURE;
	ipmi_req_t req;
	ipmi_rsp_t rsp;

	if(resId == NULL)
		return NMPRK_INVALID_ARGUMENT;

	nmprkInitializeIpmiRequest(req, 0x0A, 0x42);

	status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
	{
		if(rsp.len < 2)
			status = NMPRK_INVALID_RSP_LENGTH;
		else 
			*resId = rsp.data[0] && (rsp.data[1] << 8);
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_GetSelEntry(
	IN nmprk_conn_handle_t h,
	IN ushort_t recordId,
	OUT ushort_t *nextRecord,
	OUT nm_ipmi_sel_entry_t *record)
{
	nmprk_status_t status = NMPRK_FAILURE;
	ipmi_req_t req;
	ipmi_rsp_t rsp;

	if(nextRecord == NULL || record == NULL)
		return NMPRK_INVALID_ARGUMENT;

	nmprkInitializeIpmiRequest(req, 0x0A, 0x43);
	req.data[req.len++] = 0; // reservation id ls
	req.data[req.len++] = 0; // reservation id ms
	req.data[req.len++] = nmprkGetByte(recordId, 0); // record id ls
	req.data[req.len++] = nmprkGetByte(recordId, 1); // record id ms
	req.data[req.len++] = 0;
	req.data[req.len++] = 0xFF;

	status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
	{
		if(rsp.len < 2)
			return NMPRK_INVALID_RSP_LENGTH;

		*nextRecord = rsp.data[0] | (rsp.data[1] << 8);
		memcpy(&record->data[0], &rsp.data[2], rsp.len - 2);
		record->len = rsp.len - 2;
	}
	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_DeleteSelEntry(
	IN nmprk_conn_handle_t h,
	IN ushort_t recordId)
{
	nmprk_status_t status = NMPRK_FAILURE;
	ipmi_req_t req;
	ipmi_rsp_t rsp;
	ushort_t resId;

	status = NMPRK_IPMI_ReserveSel(h, &resId);
	if(status == NMPRK_SUCCESS)
	{
		nmprkInitializeIpmiRequest(req, 0x0A, 0x46);
		req.data[req.len++] = nmprkGetByte(resId, 0); // reservation id ls
		req.data[req.len++] = nmprkGetByte(resId, 1); // reservation id ms
		req.data[req.len++] = nmprkGetByte(recordId, 0); // record id ls
		req.data[req.len++] = nmprkGetByte(recordId, 1); // record id ms

		status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_ClearSel(
	IN nmprk_conn_handle_t h)
{
	nmprk_status_t status = NMPRK_FAILURE;
	ipmi_req_t req;
	ipmi_rsp_t rsp;
	ushort_t resId;

	status = NMPRK_IPMI_ReserveSel(h, &resId);
	if(status == NMPRK_SUCCESS)
	{
		nmprkInitializeIpmiRequest(req, 0x0A, 0x46);
		req.data[req.len++] = nmprkGetByte(resId, 0); // reservation id ls
		req.data[req.len++] = nmprkGetByte(resId, 1); // reservation id ms
		req.data[req.len++] = 0x43;
		req.data[req.len++] = 0x4C;
		req.data[req.len++] = 0x52;
		req.data[req.len++] = 0xAA;

		status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_GetFruInfo(
	IN nmprk_conn_handle_t h,
	OUT nm_ipmi_fru_info_t *output)
{
	nmprk_status_t status = NMPRK_FAILURE;
	ipmi_req_t req;
	ipmi_rsp_t rsp;

	if(output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	nmprkInitializeIpmiRequest(req, 0x0A, 0x10);
	req.data[req.len++] = 0;

	status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
	{
		if(rsp.len != 3)
			return NMPRK_INVALID_RSP_LENGTH;

		output->fruSize = rsp.data[0] | (rsp.data[1] << 8);
		output->accessByWord = rsp.data[2] & 0x01;
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_ReadFruData(
	IN nmprk_conn_handle_t h,
	IN int_t offset,
	IN OUT int_t *length,
	OUT byte_t *data)
{
	nmprk_status_t status = NMPRK_FAILURE;
	ipmi_req_t req;
	ipmi_rsp_t rsp;

	if(length == NULL || data == NULL)
		return NMPRK_INVALID_ARGUMENT;

	nmprkInitializeIpmiRequest(req, 0x0A, 0x11);
	req.data[req.len++] = 0;
	req.data[req.len++] = nmprkGetByte(offset, 0);
	req.data[req.len++] = nmprkGetByte(offset, 1);
	req.data[req.len++] = *length;

	status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
	{
		if(rsp.len == 0)
			return NMPRK_INVALID_RSP_LENGTH;

		if(rsp.len != *length + 1)
			return NMPRK_INVALID_RSP_LENGTH;

		int copyLen = *length;
		if(rsp.data[0] < copyLen)
			copyLen = rsp.data[0];
		memcpy(data, &rsp.data[1], copyLen);
		*length = copyLen;
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_WriteFruData(
	IN nmprk_conn_handle_t h,
	IN int_t offset,
	IN OUT int_t *length,
	IN byte_t *data)
{
	nmprk_status_t status = NMPRK_FAILURE;
	ipmi_req_t req;
	ipmi_rsp_t rsp;

	if(length == NULL || data == NULL)
		return NMPRK_INVALID_ARGUMENT;

	nmprkInitializeIpmiRequest(req, 0x0A, 0x12);
	req.data[req.len++] = 0;
	req.data[req.len++] = nmprkGetByte(offset, 0);
	req.data[req.len++] = nmprkGetByte(offset, 1);
	for(int i = 0; i < *length; i++)
		req.data[req.len++] = data[i];

	status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
	{
		if(rsp.len == 0)
			return NMPRK_INVALID_RSP_LENGTH;

		*length = rsp.data[0];
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_SetAcpiPowerState(
	IN nmprk_conn_handle_t h,
	IN ipmi_acpi_power_state_t *input)
{
	nmprk_status_t status = NMPRK_FAILURE;
	ipmi_req_t req;
	ipmi_rsp_t rsp;

	if(input == NULL)
		return NMPRK_INVALID_ARGUMENT;

	nmprkInitializeIpmiRequest(req, 0x06, 0x06);
	req.data[req.len++] = ((input->setSystemState << 7) & 0x80) | (input->systemState & 0x7F);
	req.data[req.len++] = ((input->setDeviceState << 7) & 0x80) | (input->deviceState & 0x7F);

	return NMPRK_SendNonBridgedCommand(h, &req, &rsp);
}

NMPRKC_API nmprk_status_t NMPRK_IPMI_GetAcpiPowerState(
	IN nmprk_conn_handle_t h,
	OUT ipmi_acpi_power_state_t *output)
{
	nmprk_status_t status = NMPRK_FAILURE;
	ipmi_req_t req;
	ipmi_rsp_t rsp;

	if(output == NULL)
		return NMPRK_INVALID_ARGUMENT;

	nmprkInitializeIpmiRequest(req, 0x06, 0x07);

	status = NMPRK_SendNonBridgedCommand(h, &req, &rsp);
	if(status == NMPRK_SUCCESS)
	{
		if(rsp.len != 2)
			return NMPRK_INVALID_RSP_LENGTH;

		output->setSystemState = 0;
		output->setDeviceState = 0;
		output->systemState = (ipmi_acpi_system_power_state_t)rsp.data[0];
		output->deviceState = (ipmi_acpi_device_power_state_t)rsp.data[1];
	}

	return status;
}

NMPRKC_API nmprk_status_t NMPRK_RegisterAlertHandler(
	IN nmprk_conn_handle_t h,
	IN alert_handler_t handler)
{
	return NMPRK_FAILURE;
}

NMPRKC_API nmprk_status_t NMPRK_UnregisterAlertHandler(
	IN nmprk_conn_handle_t h,
	IN alert_handler_t handler)
{
	return NMPRK_FAILURE;
}

#if defined __cplusplus
}
#endif
