// testC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../nmprkC/nmprkC.h"

#define SKIP_CUPS

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int args, char **argv)
#endif
{
	nmprk_status_t status;
	nmprk_conn_handle_t h;
	const byte_t POLICY_ID = 1;

	NMPRK_StartDebugLogging("nmprk.log");

	//status = NMPRK_ConnectLocal(&h);
	nmprk_connect_remote_parameters_t connParams;
#if 0
	connParams.ipOrHostname = "10.20.0.41";
	connParams.username = "root";
	connParams.password = "password";
#else
	connParams.ipOrHostname = "10.20.0.81";
	connParams.username = "ipmitest";
	connParams.password = "password";
#endif
      
#if 1
	status = NMPRK_ConnectRemote(&connParams, &h);
        assert(status == NMPRK_SUCCESS);
	assert(h == 1);
#else
        status = NMPRK_ConnectLocal(&h);
        assert(status == NMPRK_SUCCESS);
	assert(h == 0);
#endif
	

	byte_t channel, address;
	nm_discovery_parameters_t params;
	status = NMPRK_GetDiscoveryParameters(h, &params);
	assert(status == NMPRK_SUCCESS);

	status = NMPRK_SetDefaultNmCommandBridging(h, params.channel, params.address);
	assert(status == NMPRK_SUCCESS);

	/****************************************************************************/
	// Get All Policies
	/****************************************************************************/
	nm_policy_info_t *policies = NULL;
	int policiesSize = 0;
	status = NMPRK_GetAllPolicies(h, NULL, &policiesSize);
	assert(status == NMPRK_SUCCESS);
	if(status == NMPRK_SUCCESS)
	{
		policies = (nm_policy_info_t *)malloc(policiesSize);
		status = NMPRK_GetAllPolicies(h, policies, &policiesSize);
		assert(status == NMPRK_SUCCESS);
		if(status == NMPRK_SUCCESS)
		{
		}
	}

	// Remove all policies
	for(int i = 0; i < policiesSize / sizeof(nm_policy_info_t); i++)
	{
		{
		nm_enable_disable_policy_control_input_t input;
		input.domain = policies[i].domain;
		if(input.domain == HW_PROTECTION)
			continue;
		input.policy = policies[i].policyId;
		input.flags = POLICY_DISABLE;
		status = NMPRK_EnableDisablePolicyControl(h, &input);
		if(status != 0xD4)
		{
			assert(status == NMPRK_SUCCESS);
			{
			nm_set_policy_input_t input;
			memset(&input, 0, sizeof(input));
			input.removePolicy = true;
			input.info.domain = policies[i].domain;
			input.info.policyId = policies[i].policyId;
			status = NMPRK_SetPolicy(h, &input);
			assert(status == NMPRK_SUCCESS);
			}
		}
		}
	}

	if(policies != NULL)
		free(policies);

	{
	nm_set_power_draw_range_input_t input;
	memset(&input, 0, sizeof(input));
	input.domain = PLATFORM;
	input.minimumPowerDraw = 0;
	input.maximumPowerDraw = 0;
	status = NMPRK_SetPowerDrawRange(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	/*ipmi_req_t req;
	ipmi_rsp_t rsp;

	req.netFun = 0x2E;
	req.cmd = 0xCA;
	req.len = 0;
	req.data[req.len++] = 0x57;
	req.data[req.len++] = 0x01;
	req.data[req.len++] = 0x00;

	status = NMPRK_SendNmCommand(h, &req, &rsp);
	assert(status == NMPRK_SUCCESS);*/

	/****************************************************************************/
	// Enable/Disable Policy Control
	/****************************************************************************/
	{
	nm_enable_disable_policy_control_input_t input;
	input.domain = PLATFORM;
	input.policy = 0;
	input.flags = GLOBAL_DISABLE;
	status = NMPRK_EnableDisablePolicyControl(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Set Policy
	/****************************************************************************/
	{
	nm_set_policy_input_t input;
	memset(&input, 0, sizeof(input));
	input.removePolicy = false;
	input.info.domain = PLATFORM;
	input.info.policyId = POLICY_ID;
	input.info.policyEnabled = false;
	input.info.policyTargetLimit = 300;
	input.info.correctionTimeLimit = 6000;
	input.info.statisticsReportingPeriod = 60;
	status = NMPRK_SetPolicy(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Get Policy
	/****************************************************************************/
	{
	nm_get_policy_input_t input;
	memset(&input, 0, sizeof(input));
	input.domain = PLATFORM;
	input.policy = POLICY_ID;
	nm_get_policy_output_t output;
	status = NMPRK_GetPolicy(h, &input, &output);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Set Policy Alert Thresholds
	/****************************************************************************/
	{
	nm_set_policy_alert_thresholds_input_t input;
	memset(&input, 0, sizeof(input));
	input.domain = PLATFORM;
	input.policy = POLICY_ID;
	input.numberOfAlertThresholds = 2;
	input.alertThresholdsArray[0] = 100;
	input.alertThresholdsArray[1] = 200;
	status = NMPRK_SetPolicyAlertThresholds(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Get Policy Alert Thresholds
	/****************************************************************************/
	{
	nm_get_policy_alert_thresholds_input_t input;
	nm_get_policy_alert_thresholds_output_t output;
	memset(&input, 0, sizeof(input));
	memset(&output, 0, sizeof(output));
	input.domain = PLATFORM;
	input.policy = POLICY_ID;
	status = NMPRK_GetPolicyAlertThresholds(h, &input, &output);
	assert(status == NMPRK_SUCCESS);
	assert(output.numberOfAlertThresholds == 2);
	assert(output.alertThresholdsArray[0] == 100);
	assert(output.alertThresholdsArray[1] == 200);
	}

	/****************************************************************************/
	// Set Policy Suspend Periods
	/****************************************************************************/
	{
	nm_set_policy_suspend_periods_input_t input;
	memset(&input, 0, sizeof(input));
	input.domain = PLATFORM;
	input.policy = POLICY_ID;
	input.numberOfSuspendPeriods = 2;
	input.suspendPeriods[0].startTime = 6;
	input.suspendPeriods[0].stopTime = 210;
	input.suspendPeriods[0].recurrencePattern = (nm_suspend_period_recurrence_pattern_t)(MONDAY|TUESDAY|THURSDAY);
	input.suspendPeriods[1].startTime = 34;
	input.suspendPeriods[1].stopTime = 36;
	input.suspendPeriods[1].recurrencePattern = (nm_suspend_period_recurrence_pattern_t)(FRIDAY|SUNDAY);
	status = NMPRK_SetPolicySuspendPeriods(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Get Policy Suspend Periods
	/****************************************************************************/
	{
	nm_get_policy_suspend_periods_input_t input;
	nm_get_policy_suspend_periods_output_t output;
	memset(&input, 0, sizeof(input));
	memset(&output, 0, sizeof(output));
	input.domain = PLATFORM;
	input.policy = POLICY_ID;
	status = NMPRK_GetPolicySuspendPeriods(h, &input, &output);
	assert(status == NMPRK_SUCCESS);
	assert(output.numberOfSuspendPeriods == 2);
	assert(output.suspendPeriods[0].startTime == 6);
	assert(output.suspendPeriods[0].stopTime == 210);
	assert(output.suspendPeriods[0].recurrencePattern == (nm_suspend_period_recurrence_pattern_t)(MONDAY|TUESDAY|THURSDAY));
	assert(output.suspendPeriods[1].startTime == 34);
	assert(output.suspendPeriods[1].stopTime == 36);
	assert(output.suspendPeriods[1].recurrencePattern == (nm_suspend_period_recurrence_pattern_t)(FRIDAY|SUNDAY));
	}

	/****************************************************************************/
	// Reset Statistics
	/****************************************************************************/
	{
	nm_reset_statistics_input_t input;
	memset(&input, 0, sizeof(input));
	input.domain = PLATFORM;
	input.policy = 0;
	input.mode = RESET_GLOBAL_POWER_TEMP;
	status = NMPRK_ResetStatistics(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Get Statistics
	/****************************************************************************/
	{
	nm_get_statistics_input_t input;
	nm_get_statistics_output_t output;
	memset(&input, 0, sizeof(input));
	memset(&output, 0, sizeof(output));
	input.domain = PLATFORM;
	input.policy = POLICY_ID;
	input.mode = GET_POLICY_POWER;
	status = NMPRK_GetStatistics(h, &input, &output);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Get Capabilities
	/****************************************************************************/
	{
	nm_get_capabilities_input_t input;
	nm_get_capabilities_output_t output;
	memset(&input, 0, sizeof(input));
	memset(&output, 0, sizeof(output));
	input.domain = PLATFORM;
	input.policyTriggerType = INLET_TEMPERATURE_TRIGGER;
	status = NMPRK_GetCapabilities(h, &input, &output);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Get Version
	/****************************************************************************/
	{
	nm_get_version_output_t output;
	memset(&output, 0, sizeof(output));
	status = NMPRK_GetVersion(h, &output);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Set Power Draw Range
	/****************************************************************************/
	{
	nm_set_power_draw_range_input_t input;
	memset(&input, 0, sizeof(input));
	input.domain = PLATFORM;
	input.minimumPowerDraw = 0;
	input.maximumPowerDraw = 0;
	status = NMPRK_SetPowerDrawRange(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Set Alert Destination
	/****************************************************************************/
	{
	nm_set_alert_destination_input_t input;
	memset(&input, 0, sizeof(input));
	input.alertDestination.channelNumber = 1;
	status = NMPRK_SetAlertDestination(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Get Alert Destination
	/****************************************************************************/
	{
	nm_get_alert_destination_output_t output;
	memset(&output, 0, sizeof(output));
	status = NMPRK_GetAlertDestination(h, &output);
	assert(status == NMPRK_SUCCESS);
	}
#ifndef SKIP_CUPS
	/****************************************************************************/
	// Get CUPS Capabilities
	/****************************************************************************/
	{
	printf("*** Get CUPS Capabilities\n");
	nm_get_cups_capabilities_output_t output;
	memset(&output, 0, sizeof(output));
	status = NMPRK_GetCupsCapabilities(h, &output);
	assert(status == NMPRK_SUCCESS);
	printf("CUPS Enabled = %d\n", output.cupsEnabled);
	assert(output.cupsEnabled == TRUE);
	printf("CUPS Policies Available = %d\n", output.cupsPoliciesAvailable);
	assert(output.cupsPoliciesAvailable == TRUE);
	printf("CUPS Version = %d\n", output.cupsVersion);
	assert(output.cupsVersion == 1);
	printf("CUPS Reserved = %d\n", output.reserved);
	assert(output.reserved == 0);
	}

	/****************************************************************************/
	// Get CUPS Data
	/****************************************************************************/
	{
	printf("*** Get CUPS Data\n");
	nm_get_cups_data_input_t input;
	nm_get_cups_data_output_t output;
	memset(&input, 0, sizeof(input));
	memset(&output, 0, sizeof(output));
	nm_get_cups_data_parameter_t paramArray[] = { CUPS_PARAMETER_INDEX, CUPS_PARAMETER_DYNAMIC, CUPS_PARAMETER_BASE, CUPS_PARAMETER_AGGREGATE };
	for(int i = 0; i < 4; i++)
	{
		input.parameter = paramArray[i];
		status = NMPRK_GetCupsData(h, &input, &output);
		assert(status == NMPRK_SUCCESS);
		if(input.parameter == CUPS_PARAMETER_INDEX)
		{
			printf("CUPS Index = %d\n", output.data.index);
		}
		else if(input.parameter == CUPS_PARAMETER_DYNAMIC)
		{
			printf("CUPS Dynamic CPU = %d\n", output.data.dynamic.cpu);
			printf("CUPS Dynamic Memory = %d\n", output.data.dynamic.memory);
			printf("CUPS Dynamic IO = %d\n", output.data.dynamic.io);
		}
		else if(input.parameter == CUPS_PARAMETER_BASE)
		{
			printf("CUPS Base CPU = %d\n", output.data.base.cpu);
			assert(output.data.base.cpu != 0);
			printf("CUPS Base Memory = %d\n", output.data.base.memory);
			assert(output.data.base.memory != 0);
			printf("CUPS Base IO = %d\n", output.data.base.io);
			//assert(output.data.base.io != 0);
		}
		else if(input.parameter == CUPS_PARAMETER_AGGREGATE)
		{
			printf("CUPS Aggregate CPU = %d\n", output.data.aggregate.cpu);
			assert(output.data.aggregate.cpu != 0);
			printf("CUPS Aggregate Memory = %d\n", output.data.aggregate.memory);
			assert(output.data.aggregate.memory != 0);
			printf("CUPS Aggregate IO = %d\n", output.data.aggregate.io);
			//assert(output.data.aggregate.io != 0);
		}
	}
	}

	/****************************************************************************/
	// Set CUPS Configuration
	/****************************************************************************/
	{
	printf("*** Set CUPS Configuration\n");
	nm_set_cups_configuration_input_t input;
	memset(&input, 0, sizeof(input));
	input.config.cupsEnabled = 1;
	input.config.loadFactorTypeToggle = 0;
	input.config.sampleCount = 10;
	status = NMPRK_SetCupsConfiguration(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Get CUPS Configuration
	/****************************************************************************/
	{
	printf("*** Get CUPS Configuration\n");
	nm_get_cups_configuration_output_t output;
	memset(&output, 0, sizeof(output));
	status = NMPRK_GetCupsConfiguration(h, &output);
	assert(status == NMPRK_SUCCESS);
	printf("CUPS Feature Enabled = %d\n", output.config.cupsEnabled);
	assert(output.config.cupsEnabled == TRUE);
	printf("Load Factor Type = %d\n", output.config.loadFactorTypeToggle);
	printf("Static Core Load Factor = %d\n", output.config.staticCoreLoadFactor);
	printf("Static Memory Load Factor = %d\n", output.config.staticMemoryLoadFactor);
	printf("Static IO Load Factor = %d\n", output.config.staticIoLoadFactor);
	// Sample Count does not exist in NM 3.0 v1.0, add in v1.0.1
	//printf("Sample Count = %d\n", output.config.sampleCount);
	//assert(output.config.sampleCount == 10);
	}

	/****************************************************************************/
	// Set CUPS Policies
	/****************************************************************************/
	{
	printf("*** Set CUPS Policies\n");
	nm_set_cups_policies_input_t input;
	memset(&input, 0, sizeof(input));
	input.policyId = CUPS_CORE_DOMAIN;
	input.target = CUPS_TARGET_BMC;
	input.policy.policyEnabled = TRUE;
	input.policy.policyStorageVolatileMemory = 0;
	input.policy.sendAlertEnabled = 0;
	input.policy.cupsThreshold = 10;
	input.policy.averagingWindow = 5;
	status = NMPRK_SetCupsPolicies(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Get CUPS Policies
	/****************************************************************************/
	{
	printf("*** Get CUPS Policies\n");
	nm_get_cups_policies_input_t input;
	nm_get_cups_policies_output_t output;
	memset(&input, 0, sizeof(input));
	memset(&output, 0, sizeof(output));
	input.policyId = CUPS_CORE_DOMAIN;
	input.target = CUPS_TARGET_BMC;
	status = NMPRK_GetCupsPolicies(h, &input, &output);
	assert(status == NMPRK_SUCCESS);
	printf("Policy Enabled = %d\n", output.policy.policyEnabled);
	assert(output.policy.policyEnabled == TRUE);
	printf("Policy Type = %d\n", output.policy.policyStorageVolatileMemory);
	assert(output.policy.policyStorageVolatileMemory == FALSE);
	printf("Alert Enabled = %d\n", output.policy.sendAlertEnabled);
	assert(output.policy.sendAlertEnabled == FALSE);
	printf("CUPS Threshold = %d\n", output.policy.cupsThreshold);
	assert(output.policy.cupsThreshold == 10);
	printf("Averaging Window = %d\n", output.policy.averagingWindow);
	assert(output.policy.averagingWindow == 5);
	}

	/****************************************************************************/
	// Platform Characterization Launch Request
	/****************************************************************************/
	{
	printf("*** Platform Characterization Launch Request\n");
	nm_platform_characterization_launch_req_input_t input;
	memset(&input, 0, sizeof(input));
	input.launchAction = LAUNCH_NODE_MANAGER_CHARACTERIZATION;
	input.bmcTableConfigPhaseAction = BMC_TABLE_NO_ACTION;
	input.bmcPhaseStateMachineAction = BMC_PHASE_NO_ACTION;
	status = NMPRK_PlatformCharacterizationLaunchRequest(h, &input);
	//assert(status == NMPRK_SUCCESS);
	}

	/****************************************************************************/
	// Get Node Manager Power Characterization Range
	/****************************************************************************/
	{
	printf("*** Get Node Manager Power Characterization Range\n");
	nm_get_nm_power_characterization_range_input_t input;
	nm_get_nm_power_characterization_range_output_t output;
	memset(&input, 0, sizeof(input));
	memset(&output, 0, sizeof(output));
	input.domainId = POWER_DOMAIN_PLATFORM;
	status = NMPRK_GetPowerCharacterizationRange(h, &input, &output);
	assert(status == NMPRK_SUCCESS);
	printf("Timestamp = %d\n", output.timestamp);
	printf("Max Power Draw = %d\n", output.maxPowerDraw);
	printf("Min Power Draw = %d\n", output.minPowerDraw);
	printf("Eff Power Draw = %d\n", output.effPowerDraw);
	//assert(output.timestamp != 0);
	}
#endif

	/****************************************************************************/
	// IPMI Functions
	/****************************************************************************/
	{
	nm_ipmi_device_id_t input;
	memset(&input, 0, sizeof(input));
	status = NMPRK_IPMI_GetDeviceId(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	{
	nm_ipmi_repo_info_t input;
	memset(&input, 0, sizeof(input));
	status = NMPRK_IPMI_GetSdrInfo(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	{
	nm_ipmi_sdr_record_t record;
	ushort_t nextRecord;
	memset(&record, 0, sizeof(record));
	status = NMPRK_IPMI_GetSdrRecord(h, 0, &nextRecord, &record);
	assert(status == NMPRK_SUCCESS);
	}

	{
	nm_ipmi_repo_info_t input;
	memset(&input, 0, sizeof(input));
	status = NMPRK_IPMI_GetSelInfo(h, &input);
	assert(status == NMPRK_SUCCESS);
	}

	{
	nm_ipmi_sel_entry_t entry;
	ushort_t nextRecord;
	memset(&entry, 0, sizeof(entry));
	status = NMPRK_IPMI_GetSelEntry(h, 00, &nextRecord, &entry);
	assert(status == NMPRK_SUCCESS);
	}

	{
	nm_ipmi_fru_info_t output;
	memset(&output, 0, sizeof(output));
	status = NMPRK_IPMI_GetFruInfo(h, &output);
	assert(status == NMPRK_SUCCESS);
	}

	{
	ipmi_acpi_power_state_t output;
	memset(&output, 0, sizeof(output));
	status = NMPRK_IPMI_GetAcpiPowerState(h, &output);
	assert(status == NMPRK_SUCCESS);
	}

	puts("Complete! Press the Enter key to exit");
    getchar();
	return 0;
}

