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

/*! \file nmprkTypes.h
    \brief The header file for that contains data types, structures and
	       enumerations for the NMPRK API
*/

#ifndef _NMPRK_TYPES_H_
#define _NMPRK_TYPES_H_

#include <time.h>
#include "nmprkMutex.h"
#include "nmprkErrorCodes.h"

typedef int nmprk_status_t;
typedef int nmprk_conn_handle_t; // TODO: maybe make pointer to device?

typedef int bool_t;
typedef char char_t;
typedef unsigned char byte_t;
typedef unsigned short ushort_t;
typedef int int_t;
typedef unsigned int uint_t;
typedef unsigned long long ulong_t;
typedef byte_t nm_policy_id_t;

typedef void (*nmprk_debug_callback)(const char *);  
typedef void(*alert_handler_t)(nmprk_conn_handle_t,void *);

#define KCS_DEVICE_HANDLE 0
#define BMC_ADDRESS 0x20

/*****************************************************************************/
// IPMI Command Structures
/*****************************************************************************/
#define MAX_IPMI_SIZE 490

#ifdef PRAGMA_PACK
#pragma pack(1)
#endif
/*!
  The structure contains a raw IPMI request
*/
typedef struct ipmi_req_t
{
    byte_t netFun; /*!< Net Function */
    byte_t cmd; /*!< Command */
    byte_t data[MAX_IPMI_SIZE]; /*!< Data */
	size_t len; /*!< Data Length */
    byte_t rsAddr; /*!< Responder Address */
    byte_t rsLun; /*!< Responder LUN */
} ipmi_req_t;

/*!
  The structure contains a raw IPMI response
*/
typedef struct ipmi_rsp_t
{
    byte_t compCode; /*!< Completion Code */
    byte_t data[MAX_IPMI_SIZE]; /*!< Data */
    size_t len; /*!< Length */
} ipmi_rsp_t;
#ifdef PRAGMA_PACK
#pragma pack()
#endif
/*****************************************************************************/
// IPMI Capture Structures
/*****************************************************************************/
#ifdef PRAGMA_PACK
#pragma pack(1)
#endif
/*!
  The structure contains the data bytes of an IPMI request
*/
typedef struct ipmi_capture_req_t
{
	int count; /*!< Number of Bytes */
	byte_t data[MAX_IPMI_SIZE]; /*!< Data */
} ipmi_capture_req_t;

/*!
  The structure contains the data bytes of an IPMI response
*/
typedef struct ipmi_capture_rsp_t
{
	int count; /*!< Number of Bytes */
	byte_t data[MAX_IPMI_SIZE]; /*!< Data */
} ipmi_capture_rsp_t;
#ifdef PRAGMA_PACK
#pragma pack()
#endif

/*****************************************************************************/
// Node Manager Enumerations
/*****************************************************************************/
/*!
  Node Manager Domains

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_domain_id_t
{
	PLATFORM,
	CPU,
	MEMORY,
	HW_PROTECTION,
	HIGH_POWER_IO,
	LAST_DOMAIN = HIGH_POWER_IO
} nm_domain_id_t;

/*!
  Node Manager Policy Control Flags

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_policy_enable_disable_t
{
	GLOBAL_DISABLE,
	GLOBAL_ENABLE,
	DOMAIN_DISABLE,
	DOMAIN_ENABLE,
	POLICY_DISABLE,
	POLICY_ENABLE
} nm_policy_enable_disable_t;

/*!
  Node Manager Policy Trigger Types

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_policy_trigger_type_t
{
	NO_POLICY_TRIGGER,
	INLET_TEMPERATURE_TRIGGER,
	MISSING_POWER_READING_TIMEOUT_TRIGGER,
	TIME_AFTER_HOST_RESET_TRIGGER,
	BOOT_TIME_TRIGGER
} nm_policy_trigger_type_t;

/*!
  Node Manager Aggressive CPU Power Correction Mode

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_aggressive_cpu_power_correction_t
{
	DEFAULT,
	T_STATES_NOT_ALLOWED,
	T_STATES_ALLOWED
} nm_aggressive_cpu_power_correction_t;

/*!
  Node Manager Suspend Period Recurrence Patterns

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_suspend_period_recurrence_pattern_t
{
	MONDAY = 0x01,
	TUESDAY = 0x02,
	WEDNESDAY = 0x04,
	THURSDAY = 0x08,
	FRIDAY = 0x10,
	SATURDAY = 0x20,
	SUNDAY = 0x40
} nm_suspend_period_recurrence_pattern_t;

/*!
  Node Manager Reset Statistics Mode

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_reset_statistics_mode_t
{
	RESET_GLOBAL_POWER_TEMP = 0x00,
	RESET_POLICY_POWER_TEMP = 0x01,
	RESET_GLOBAL_HOST_UNHANDLED_REQ = 0x1B,
	RESET_GLOBAL_HOST_RESPONSE_TIME = 0x1C,
	RESET_GLOBAL_CPU_THROTTLING = 0x1D,
	RESET_GLOBAL_MEMORY_THROTTLING = 0x1E,
	RESET_GLOBAL_HOST_COMMUNICATION_FAILURE = 0x1F
} nm_reset_statistics_mode_t;

/*!
  Node Manager Get Statistics Mode

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_get_statistics_mode_t
{
	GET_GLOBAL_POWER = 0x01,
	GET_GLOBAL_TEMPERATURE = 0x02,
	GET_GLOBAL_THROTTLING = 0x03,
	GET_GLOBAL_VOLUMETRIC_AIRFLOW = 0x04,
	GET_GLOBAL_OUTLET_AIRFLOW_TEMPERATURE = 0x05,
	GET_GLOBAL_CHASSIS_POWER = 0x06,
	GET_POLICY_POWER = 0x11,
	GET_POLICY_TEMPERATURE = 0x12,
	GET_POLICY_THROTTLING = 0x13,
	GET_GLOBAL_HOST_UNHANDLED_REQ = 0x1B,
	GET_GLOBAL_HOST_RESPONSE_TIME = 0x1C,
	GET_GLOBAL_CPU_THROTTLING = 0x1D,
	GET_GLOBAL_MEMORY_THROTTLING = 0x1E,
	GET_GLOBAL_HOST_COMMUNICATION_FAILURE = 0x1F
} nm_get_statistics_mode_t;

/*!
  Node Manager PTU Launch Power Characterization Mode

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_ptu_launch_power_characterization_on_hw_change_t
{
	DONT_LAUNCH_OR_CANCEL_PREV_LAUNCH = 0x00,
	LAUNCH_NODE_MANAGER_CHARACTERIZATION = 0x01,
	LAUNCH_NODE_MANAGER_CHARACTERIZATION_ON_HW_CHANGE = 0x02
} nm_ptu_launch_power_characterization_on_hw_change_t;

/*!
  Node Manager PTU BMC Configuration Table Action

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_ptu_bmc_table_config_phase_action_t
{
	BMC_TABLE_NO_ACTION = 0x00,
	BMC_TABLE_CLEAR_ALL = 0x01,
	BMC_TABLE_WRITE = 0x02
} nm_ptu_bmc_table_config_phase_action_t;

/*!
  Node Manager PTU BMC Phase State Machine Action

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_ptu_bmc_phase_state_machine_action_t
{
	BMC_PHASE_NO_ACTION = 0x00,
	BMC_PHASE_RESTART = 0x01,
	BMC_PHASE_SKIP_TO_THE_NEXT_TABLE_ENTRY = 0x02,
	BMC_PHASE_EXIT = 0x03
} nm_ptu_bmc_phase_state_machine_action_t;

/*!
  Node Manager PTU Power Domain ID

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_ptu_power_domain_id_t
{
	POWER_DOMAIN_PLATFORM = 0x00,
	POWER_DOMAIN_CPU_SUBSYSTEM = 0x01,
	POWER_DOMAIN_MEMORY_SUBSYSTEM = 0x02
} nm_ptu_power_domain_id_t;

/*!
  Node Manager PTU Power Draw Characterization Point

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_ptu_power_draw_characterization_point_t
{
	POWER_DRAW_CHARACTERIZATION_POINT_MAX = 0x00,
	POWER_DRAW_CHARACTERIZATION_POINT_MIN = 0x01,
	POWER_DRAW_CHARACTERIZATION_POINT_EFF = 0x02
} nm_ptu_power_draw_characterization_point_t;

/*!
  Node Manager Get CUPS Data Parameter

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_get_cups_data_parameter_t
{
	CUPS_PARAMETER_INDEX = 0x01,
	CUPS_PARAMETER_DYNAMIC = 0x02,
	CUPS_PARAMETER_BASE = 0x03,
	CUPS_PARAMETER_AGGREGATE = 0x04
} nm_get_cups_data_parameter_t;

/*!
  Node Manager Get CUPS Policy ID

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_cups_policy_id_t
{
	CUPS_CORE_DOMAIN = 0x01,
	CUPS_MEMORY_DOMAIN = 0x02,
	CUPS_IO_DOMAIN = 0x04
} nm_cups_policy_id_t;

/*!
  Node Manager Get CUPS Target Identifier

  See the Node Manager Interface Specification for more details
*/
typedef enum nm_cups_target_identifier_t
{
	CUPS_TARGET_BMC = 0x00,
	CUPS_TARGET_REMOTE_CONSOLE = 0x01
} nm_cups_target_identifier_t;

/*****************************************************************************/
// IPMI Enumerations
/*****************************************************************************/
/*!
  IPMI Connection Cipher Suites

  See the IPMI Specification v2.0 for more information
*/
typedef enum ipmi_cipher_suite_t
{
	CIPHER_SUITE_0 = 0,
	CIPHER_SUITE_1 = 1,
	CIPHER_SUITE_2 = 2,
	CIPHER_SUITE_3 = 3,
	CIPHER_SUITE_6 = 6,
	CIPHER_SUITE_7 = 7,
	CIPHER_SUITE_8 = 8,
	CIPHER_SUITE_11 = 11,
	CIPHER_SUITE_12 = 12
	/* All others are not supported */
} ipmi_cipher_suite_t;

/*!
  IPMI ACPI System Power States

  See the IPMI Specification v2.0 for more information
*/
typedef enum ipmi_acpi_system_power_state_t
{
    ACPI_POWER_STATE_SYS_S0G0 = 0x00,
    ACPI_POWER_STATE_SYS_S1 = 0x01,
    ACPI_POWER_STATE_SYS_S2 = 0x02,
    ACPI_POWER_STATE_SYS_S3 = 0x03,
    ACPI_POWER_STATE_SYS_S4 = 0x04,
    ACPI_POWER_STATE_SYS_S5G2 = 0x05,
    ACPI_POWER_STATE_SYS_S4S5 = 0x06,
    ACPI_POWER_STATE_SYS_G3 = 0x07,
    ACPI_POWER_STATE_SYS_SLEEPING = 0x08,
    ACPI_POWER_STATE_SYS_G1_SLEEP = 0x09,
    ACPI_POWER_STATE_SYS_OVERRIDE = 0x0A,
    ACPI_POWER_STATE_SYS_LEGACY_ON = 0x20 ,
    ACPI_POWER_STATE_SYS_LEGACY_OFF = 0x21, 
    ACPI_POWER_STATE_SYS_UNKNOWN = 0x2A,
    ACPI_POWER_STATE_SYS_NO_CHANGE = 0x7f 
} ipmi_acpi_system_power_state_t;

/*!
  IPMI ACPI Device Power States

  See the IPMI Specification v2.0 for more information
*/
typedef enum ipmi_acpi_device_power_state_t
{
    ACPI_POWER_STATE_DEV_D0 = 0x00,
    ACPI_POWER_STATE_DEV_D1 = 0x01,
    ACPI_POWER_STATE_DEV_D2 = 0x02,
    ACPI_POWER_STATE_DEV_D3 = 0x03,
    ACPI_POWER_STATE_DEV_UNKNOWN = 0x2a,
    ACPI_POWER_STATE_DEV_NO_CHANGE = 0x7f
} ipmi_acpi_device_power_state_t;

/*****************************************************************************/
// NMPRK Function Input/Output structures
/*****************************************************************************/
#ifdef PRAGMA_PACK
#pragma pack(1)
#endif

/*!
  Parameters passed to the NMPRK_ConnectRemote function

  See the IPMI Specification v2.0 for more information
*/
typedef struct nmprk_connect_remote_parameters_t
{
	const char *ipOrHostname; /*!< IP Address or Hostname of the BMC */
	const char *username; /*!< BMC Username */
	const char *password; /*!< BMC Password */
	bool_t useCustomCipherSuite; /*!< False, use the default cipher suite (3).
								      True, use the customCipherSuite */
	ipmi_cipher_suite_t customCipherSuite; /*!< Cipher Suite to use instead 
										        of the default. 
												useCustomCipherSuite must be 
												true*/
} nmprk_connect_remote_parameters_t;

/*!
  Parameters to pass to the ::NMPRK_EnableDisablePolicyControl function

  See the Node Manager Interface Specification for more details on these 
  parameters
*/
typedef struct nm_enable_disable_policy_control_input_t
{
	nm_policy_enable_disable_t flags;
	nm_domain_id_t domain;
	byte_t policy;
} nm_enable_disable_policy_control_input_t;

/*!
  Parameters that represent a Node Manager Policy.

  Used with the following functions:\n
  ::NMPRK_SetPolicy\n
  ::NMPRK_GetPolicy\n
  ::NMPRK_GetAllPolicies

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_policy_info_t
{
	nm_domain_id_t domain;
	byte_t policyId;
	bool_t policyEnabled;
	nm_policy_trigger_type_t policyTriggerType;
	nm_aggressive_cpu_power_correction_t aggressiveCpuPowerCorrection;
	bool_t policyExceptionActionShutdown;
	bool_t policyExceptionActionSendAlert;
	bool_t secondaryPowerDomain;
	ushort_t policyTargetLimit;
	uint_t  correctionTimeLimit;
	ushort_t policyTriggerLimit;
	ushort_t statisticsReportingPeriod;
} nm_policy_info_t;

/*!
  Parameters to pass to the ::NMPRK_SetPolicy function

  See the Node Manager Interface Specification for more details on these 
  parameters
*/
typedef struct nm_set_policy_input_t
{
	nm_policy_info_t info;
	bool_t removePolicy;
} nm_set_policy_input_t;

/*!
  Parameters to pass to the ::NMPRK_GetPolicy function

  See the Node Manager Interface Specification for more details on these 
  parameters
*/
typedef struct nm_get_policy_input_t
{
	nm_domain_id_t domain;
	byte_t policy;
} nm_get_policy_input_t;

/*!
  Parameters returned from the ::NMPRK_GetPolicy function

  Choose which struct inside the union to use based on the completion code.
  00h - standard
  80h - policy_id_invalid
  81h - domain_id_invalid

  See the Node Manager Interface Specification for more details on these 
  parameters
*/
typedef struct nm_get_policy_output_t
{
	union {
		struct {
			nm_policy_info_t info;
			bool_t perDomainPolicyControlEnabled;
			bool_t globalPolicyControlEnabled;
			bool_t policyCreatedByOtherClient;
		} standard;
		struct {
			bool_t valid;
			byte_t nextValidPolicyId;
			byte_t numberOfDefinedPoliciesForDomain;
		} policy_id_invalid;
		struct {
			bool_t valid;
			byte_t nextValidDomainId;
			byte_t numberOfAvailableDomains;
		} domain_id_invalid;
	} u;
} nm_get_policy_output_t;

/*!
  Parameters to pass to the ::NMPRK_SetPolicyAlertThresholds function

  See the Node Manager Interface Specification for more details on these 
  parameters
*/
typedef struct nm_set_policy_alert_thresholds_input_t
{
	nm_domain_id_t domain;
	byte_t policy;
	byte_t numberOfAlertThresholds;
	ushort_t alertThresholdsArray[3];
} nm_set_policy_alert_thresholds_input_t;

/*!
  Parameters to pass to the ::NMPRK_GetPolicyAlertThresholds function

  See the Node Manager Interface Specification for more details on these 
  parameters
*/
typedef struct nm_get_policy_alert_thresholds_input_t
{
	nm_domain_id_t domain;
	byte_t policy;
} nm_get_policy_alert_thresholds_input_t;

/*!
  Parameters returned from the ::NMPRK_GetPolicyAlertThresholds function

  See the Node Manager Interface Specification for more details on these 
  parameters
*/
typedef struct nm_get_policy_alert_thresholds_output_t
{
	byte_t numberOfAlertThresholds;
	ushort_t alertThresholdsArray[3];
} nm_get_policy_alert_thresholds_output_t;

/*!
  Represents a Node Manager Policy Suspend Period

  See the Node Manager Interface Specification for more details on these 
  parameters
*/
typedef struct nm_policy_suspend_period_t
{
	byte_t startTime;
	byte_t stopTime;
	nm_suspend_period_recurrence_pattern_t recurrencePattern;
} nm_policy_suspend_period_t;

/*!
  Parameters to pass to the ::NMPRK_SetPolicySuspendPeriods function

  See the Node Manager Interface Specification for more details on these 
  parameters
*/
typedef struct nm_set_policy_suspend_periods_input_t
{
	nm_domain_id_t domain;
	byte_t policy;
	byte_t numberOfSuspendPeriods;
	nm_policy_suspend_period_t suspendPeriods[5];
} nm_set_policy_suspend_periods_input_t;

/*!
  Parameters to pass to the ::NMPRK_GetPolicySuspendPeriods function

  See the Node Manager Interface Specification for more details on these 
  parameters
*/
typedef struct nm_get_policy_suspend_periods_input_t
{
	nm_domain_id_t domain;
	byte_t policy;
} nm_get_policy_suspend_periods_input_t;

/*!
  Parameters returned from the ::NMPRK_GetPolicySuspendPeriods function

  See the Node Manager Interface Specification for more details on these 
  parameters
*/
typedef struct nm_get_policy_suspend_periods_output_t
{
	byte_t numberOfSuspendPeriods;
	nm_policy_suspend_period_t suspendPeriods[5];
} nm_get_policy_suspend_periods_output_t;

/*!
  Parameters to pass to the ::NMPRK_ResetStatistics function

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_reset_statistics_input_t
{
	nm_reset_statistics_mode_t mode;
	nm_domain_id_t domain;
	byte_t policy;
} nm_reset_statistics_input_t;

/*!
  Parameters to pass to the ::NMPRK_GetStatistics function

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_get_statistics_input_t
{
	nm_get_statistics_mode_t mode;
	nm_domain_id_t domain;
	byte_t policy;
} nm_get_statistics_input_t;

/*!
  Parameters returned from the ::NMPRK_GetStatistics function

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_get_statistics_output_t
{
	ushort_t currentValue;
	ushort_t minimumValue;
	ushort_t maximumValue;
	ushort_t averageValue;
	uint_t timestamp;
	uint_t statisticsReportingPeriod;
	nm_domain_id_t domain;
	bool_t policyGlobalAdministrativeState;
	bool_t policyOperationalState;
	bool_t measurementsState;
	bool_t policyActivationState;
} nm_get_statistics_output_t;

/*!
  Parameters to pass to the ::NMPRK_GetCapabilities function

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_get_capabilities_input_t
{
	nm_domain_id_t domain;
	nm_policy_trigger_type_t policyTriggerType;
} nm_get_capabilities_input_t;

/*!
  Parameters returned from the ::NMPRK_GetCapabilities function

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_get_capabilities_output_t
{
	byte_t maxConcurrentSettings;
	ushort_t maxValue;
	ushort_t minValue;
	uint_t minCorrectionTime;
	uint_t maxCorrectionTime;
	ushort_t minStatisticsReportingPeriod;
	ushort_t maxStatisticsReportingPeriod;
	nm_domain_id_t domainLimitingScope;
	bool_t limitingBasedOn;
} nm_get_capabilities_output_t;

/*!
  Parameters returned from the ::NMPRK_GetVersion function

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_get_version_output_t
{
	byte_t version;
	byte_t ipmiVersion;
	byte_t patchVersion;
	byte_t majorFirmwareRevision;
	byte_t minorFirmwareRevision;
} nm_get_version_output_t;

/*!
  Parameters to pass to the ::NMPRK_SetPowerDrawRange function

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_set_power_draw_range_input_t
{
	nm_domain_id_t domain;
	ushort_t minimumPowerDraw;
	ushort_t maximumPowerDraw;
} nm_set_power_draw_range_input_t;

/*!
  Parameters for a Node Manager Alert Destination

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_alert_destination_t
{
	byte_t channelNumber;
	bool_t destinationInformationReceiver;
	union {
		byte_t i2cSlaveAddress;
		byte_t destinationSelector;
	} destinationInformation;
	byte_t alertStringSelector;
	bool_t sendAlertString;
} nm_alert_destination_t;

/*!
  Parameters to pass to the ::NMPRK_SetAlertDestination function

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_set_alert_destination_input_t
{
	nm_alert_destination_t alertDestination;
} nm_set_alert_destination_input_t;

/*!
  Parameters returned from the ::NMPRK_GetAlertDestination function

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_get_alert_destination_output_t
{
	nm_alert_destination_t alertDestination;
} nm_get_alert_destination_output_t;

/*!
  Parameters to pass to the ::NMPRK_PlatformCharacterizationLaunchRequest function

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_platform_characterization_launch_req_input_t
{
	nm_ptu_launch_power_characterization_on_hw_change_t launchAction;
	nm_ptu_bmc_table_config_phase_action_t bmcTableConfigPhaseAction;
	nm_ptu_bmc_phase_state_machine_action_t bmcPhaseStateMachineAction;
	nm_ptu_power_domain_id_t powerDomainId;
	nm_ptu_power_draw_characterization_point_t powerDrawCharacterizationPoint;
	uint_t delay;
	uint_t timeToRun;
} nm_platform_characterization_launch_req_input_t;

/*!
  Parameters to pass to the ::NMPRK_GetNodeManagerPowerCharacterizationRange function

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_get_nm_power_characterization_range_input_t
{
	nm_ptu_power_domain_id_t domainId;
} nm_get_nm_power_characterization_range_input_t;

/*!
  Parameters returned from the ::NMPRK_GetNodeManagerPowerCharacterizationRange function

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_get_nm_power_characterization_range_output_t
{
	uint_t timestamp;
	ushort_t maxPowerDraw;
	ushort_t minPowerDraw;
	ushort_t effPowerDraw;
} nm_get_nm_power_characterization_range_output_t;

/*!
  Parameters returned from the ::NMPRK_GetCupsCapabilities function

  See the Node Manager 3.0 Interface Specification for more details
*/
typedef struct nm_get_cups_capabilities_output_t
{
	bool_t cupsEnabled;
	bool_t cupsPoliciesAvailable;
	byte_t cupsVersion;
	byte_t reserved;
} nm_get_cups_capabilities_output_t;

/*!
  Parameters to pass to the ::NMPRK_GetCupsData function

  See the Node Manager 3.0 Interface Specification for more details
*/
typedef struct nm_get_cups_data_input_t
{
	nm_get_cups_data_parameter_t parameter;
} nm_get_cups_data_input_t;

/*!
  Parameters returned from the ::NMPRK_GetCupsData function

  See the Node Manager 3.0 Interface Specification for more details
*/
typedef struct nm_get_cups_data_output_t
{
	union {
		struct {
			ushort_t index;
		} index;
		struct {
			ushort_t cpu;
			ushort_t memory;
			ushort_t io;
		} dynamic;
		struct {
			ulong_t cpu;
			ulong_t memory;
			ulong_t io;
		} base;
		struct {
			ulong_t cpu;
			ulong_t memory;
			ulong_t io;
		} aggregate;
	} data;
} nm_get_cups_data_output_t;

/*!
  Parameters for a CUPS Configuration

  See the Node Manager 3.0 Interface Specification for more details
*/
typedef struct nm_cups_configuration_t
{
	bool_t cupsEnabled;
	bool_t loadFactorTypeToggle; /*!< Set = Toggle Switch (0=No Change, 1=Toggle between dynamic and static), Get = Type (0=Dynamic, 1=Static) */ 
	ushort_t staticCoreLoadFactor;
	ushort_t staticMemoryLoadFactor;
	ushort_t staticIoLoadFactor;
	byte_t sampleCount;
} nm_cups_configuration_t;

/*!
  Parameters to pass to the ::NMPRK_SetCupsConfiguration function

  See the Node Manager 3.0 Interface Specification for more details
*/
typedef struct nm_set_cups_configuration_input_t
{
	bool_t setCoreLoadFactor;
	bool_t setMemoryLoadFactor;
	bool_t setIoLoadFactor;
	nm_cups_configuration_t config;
} nm_set_cups_configuration_input_t;

/*!
  Parameters returned from the ::NMPRK_GetCupsConfiguration function

  See the Node Manager 3.0 Interface Specification for more details
*/
typedef struct nm_get_cups_configuration_output_t
{
	nm_cups_configuration_t config;
} nm_get_cups_configuration_output_t;

/*!
  Parameters for a CUPS Policy

  See the Node Manager 3.0 Interface Specification for more details
*/
typedef struct nm_cups_policy_t
{
	bool_t policyEnabled;
	bool_t policyStorageVolatileMemory;
	bool_t sendAlertEnabled;
	byte_t cupsThreshold;
	ushort_t averagingWindow;
} nm_cups_policy_t;

/*!
  Parameters to pass to the ::NMPRK_SetCupsPolicies function

  See the Node Manager 3.0 Interface Specification for more details
*/
typedef struct nm_set_cups_policies_input_t
{
	nm_cups_policy_id_t policyId;
	nm_cups_target_identifier_t target;
	nm_cups_policy_t policy;
} nm_set_cups_policies_input_t;

/*!
  Parameters to pass to the ::NMPRK_GetCupsPolicies function

  See the Node Manager 3.0 Interface Specification for more details
*/
typedef struct nm_get_cups_policies_input_t
{
	nm_cups_policy_id_t policyId;
	nm_cups_target_identifier_t target;
} nm_get_cups_policies_input_t;

/*!
  Parameters returned from the ::NMPRK_GetCupsPolicies function

  See the Node Manager 3.0 Interface Specification for more details
*/
typedef struct nm_get_cups_policies_output_t
{
	nm_cups_policy_t policy;
} nm_get_cups_policies_output_t;

/*!
  Details contained in the Node Manager Discovery record in the SDR

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_discovery_parameters_t
{
	byte_t channel;
	byte_t address;
	byte_t nmHealthEvSensor;
	byte_t nmExceptionEvSensor;
	byte_t nmOperationalCapSensor;
	byte_t nmAlertThresExcdSensor;
} nm_discovery_parameters_t;

/*!
  Information returned from the ::NMPRK_IPMI_GetDeviceId

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_ipmi_device_id_t {
    byte_t deviceId;
    uint_t deviceRev;
    bool_t deviceProvidesSdr;
    uint_t firmwareRev;
    bool_t devNormOp;
    uint_t firmwareRev2;
	byte_t ipmiVersion;
    bool_t isSensorDev;
    bool_t isSdrRepoDev;
	bool_t isSelDev;
    bool_t isFruInvDev; 
    bool_t isIpmbRevDev;         
    bool_t isIpmbGenDev;
    bool_t isBridgeDev;
    bool_t isChassisDev;   
    byte_t manufId[3];
    byte_t productId[2];
} nm_ipmi_device_id_t;


/*!
  Information in an SDR Record.

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_ipmi_sdr_record_t
{
	ushort_t recordId;
	byte_t version;
	byte_t type;
	byte_t len;
    byte_t data[1024];
} nm_ipmi_sdr_record_t;

/*!
  Information in an SEL Entry.

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_ipmi_sel_entry_t
{
    byte_t data[1024];
    uint_t len; 
} nm_ipmi_sel_entry_t;

/*!
  Information about the SDR or SEL repository

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_ipmi_repo_info_t {
    byte_t repoVersion;       // Version of the SEL
    uint_t repoEntries;       // get total entries in SEL
    uint_t repoFreeSpace;     // the amount of space left in the SEL
    tm mostRecentAddTS;       // Timestamp from the last add command
    tm mostRecentDelTS;       // timestamp from the last delete command
    bool_t getAllocInfoSup;   // does this SEL support getAllocInfo command
    bool_t reserveSup;        // reserve sel supported
    bool_t parAddSup;         // partial add sel support
    bool_t delSup;            // sel supports delete 
    bool_t nonmodalSupported; // non-modal SDR Repository update supported
    bool_t modalSupported;    // modal SDR Repository update supported
} nm_ipmi_repo_info_t;

/*!
  Information about the FRU repository

  See the Node Manager Interface Specification for more details
*/
typedef struct nm_ipmi_fru_info_t
{
      ushort_t fruSize;
      bool_t accessByWord;
} nm_ipmi_fru_info_t;

/*!
  Information about the ACPI Power State

  See the Node Manager Interface Specification for more details
*/
typedef struct ipmi_acpi_power_state_t
{
	bool_t setSystemState;
	bool_t setDeviceState;
	ipmi_acpi_system_power_state_t systemState;
	ipmi_acpi_device_power_state_t deviceState;
} ipmi_acpi_power_state_t;

#ifdef PRAGMA_PACK
#pragma pack()
#endif

#endif //_NMPRK_TYPES_H_

