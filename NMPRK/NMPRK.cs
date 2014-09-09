using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace NMPRK
{
    #region Enums
    public enum ipmi_cipher_suite_t
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
    }

    public enum nm_policy_enable_disable_t
    {
        GLOBAL_DISABLE,
        GLOBAL_ENABLE,
        DOMAIN_DISABLE,
        DOMAIN_ENABLE,
        POLICY_DISABLE,
        POLICY_ENABLE
    }

    public enum nm_domain_id_t
    {
        PLATFORM,
        CPU,
        MEMORY
    }

    public enum nm_policy_trigger_type_t
    {
        NO_POLICY_TRIGGER,
        INLET_TEMPERATURE_TRIGGER,
        MISSING_POWER_READING_TIMEOUT_TRIGGER,
        TIME_AFTER_HOST_RESET_TRIGGER,
        BOOT_TIME_TRIGGER
    }

    public enum nm_aggressive_cpu_power_correction_t
    {
        DEFAULT,
        T_STATES_NOT_ALLOWED,
        T_STATES_ALLOWED
    }

    public enum nm_suspend_period_recurrence_pattern_t : int
    {
	    MONDAY = 0x01,
	    TUESDAY = 0x02,
	    WEDNESDAY = 0x04,
	    THURSDAY = 0x08,
	    FRIDAY = 0x10,
	    SATURDAY = 0x20,
	    SUNDAY = 0x40
    }

    public enum nm_reset_statistics_mode_t
    {
        RESET_GLOBAL_POWER_TEMP = 0x00,
        RESET_POLICY_POWER_TEMP = 0x01,
        RESET_GLOBAL_HOST_UNHANDLED_REQ = 0x1B,
        RESET_GLOBAL_HOST_RESPONSE_TIME = 0x1C,
        RESET_GLOBAL_CPU_THROTTLING = 0x1D,
        RESET_GLOBAL_MEMORY_THROTTLING = 0x1E,
        RESET_GLOBAL_HOST_COMMUNICATION_FAILURE = 0x1F
    }

    public enum nm_get_statistics_mode_t
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
    }

    public enum nm_ptu_launch_power_characterization_on_hw_change_t
    {
        DONT_LAUNCH_OR_CANCEL_PREV_LAUNCH = 0x00,
        LAUNCH_NODE_MANAGER_CHARACTERIZATION = 0x01,
        LAUNCH_NODE_MANAGER_CHARACTERIZATION_ON_HW_CHANGE = 0x02
    }

    public enum nm_ptu_bmc_table_config_phase_action_t
    {
	    BMC_TABLE_NO_ACTION = 0x00,
	    BMC_TABLE_CLEAR_ALL = 0x01,
	    BMC_TABLE_WRITE = 0x02
    }

    public enum nm_ptu_bmc_phase_state_machine_action_t
    {
	    BMC_PHASE_NO_ACTION = 0x00,
	    BMC_PHASE_RESTART = 0x01,
	    BMC_PHASE_SKIP_TO_THE_NEXT_TABLE_ENTRY = 0x02,
	    BMC_PHASE_EXIT = 0x03
    }

    public enum nm_ptu_power_domain_id_t
    {
	    POWER_DOMAIN_PLATFORM = 0x00,
	    POWER_DOMAIN_CPU_SUBSYSTEM = 0x01,
	    POWER_DOMAIN_MEMORY_SUBSYSTEM = 0x02
    }

    public enum nm_ptu_power_draw_characterization_point_t
    {
	    POWER_DRAW_CHARACTERIZATION_POINT_MAX = 0x00,
	    POWER_DRAW_CHARACTERIZATION_POINT_MIN = 0x01,
	    POWER_DRAW_CHARACTERIZATION_POINT_EFF = 0x02
    }

    public enum nm_get_cups_data_parameter_t
    {
        CUPS_PARAMETER_INDEX = 0x01,
        CUPS_PARAMETER_DYNAMIC = 0x02,
        CUPS_PARAMETER_BASE = 0x03,
        CUPS_PARAMETER_AGGREGATE = 0x04
    }

    public enum nm_cups_policy_id_t
    {
	    CUPS_CORE_DOMAIN = 0x01,
	    CUPS_MEMORY_DOMAIN = 0x02,
	    CUPS_IO_DOMAIN = 0x04
    }

    public enum nm_cups_target_identifier_t
    {
        CUPS_TARGET_BMC = 0x00,
        CUPS_TARGET_REMOTE_CONSOLE = 0x01
    }

    public enum ipmi_acpi_system_power_state_t
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
    }

    public enum ipmi_acpi_device_power_state_t
    {
        ACPI_POWER_STATE_DEV_D0 = 0x00,
        ACPI_POWER_STATE_DEV_D1 = 0x01,
        ACPI_POWER_STATE_DEV_D2 = 0x02,
        ACPI_POWER_STATE_DEV_D3 = 0x03,
        ACPI_POWER_STATE_DEV_UNKNOWN = 0x2a,
        ACPI_POWER_STATE_DEV_NO_CHANGE = 0x7f
    }
    #endregion

    #region Input/Output Structures
    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nmprk_connect_remote_parameters_t
    {
        public string ipOrHostname;
        public string username;
        public string password;
        public bool useCustomCipherSuite;
        public ipmi_cipher_suite_t customCipherSuite;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_enable_disable_policy_control_input_t
    {
        public nm_policy_enable_disable_t flags;
        public nm_domain_id_t domain;
        public byte policy;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_policy_info_t
    {
        public nm_domain_id_t domain;
        public byte policy;
        public bool policyEnabled;
        public nm_policy_trigger_type_t policyTriggerType;
        public nm_aggressive_cpu_power_correction_t aggressiveCpuPowerCorrection;
        public bool policyExceptionActionShutdown;
        public bool policyExceptionActionSendAlert;
        public bool secondardPowerDomain;
        public ushort policyTargetLimit;
        public uint correctionTimeLimit;
        public ushort policyTriggerLimit;
        public ushort statisticsReportingPeriod;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_set_policy_input_t
    {
        public nm_policy_info_t info;
        public bool removePolicy;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_policy_input_t
    {
        public nm_domain_id_t domain;
        public byte policy;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_policy_output_standard_t
    {
        public nm_policy_info_t info;
        public bool perDomainPolicyControlEnabled;
        public bool globalPolicyControlEnabled;
        public bool policyCreatedByOtherClient;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_policy_output_policy_id_invalid_t
    {
        public byte nextValidPolicyId;
        public byte numberOfDefinedPoliciesForDomain;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_policy_output_domain_id_invalid_t
    {
        public byte nextValidDomainId;
        public byte numberOfAvailableDomains;
    }

    [StructLayout(LayoutKind.Explicit)]
    public struct nm_get_policy_output_t
    {
        [FieldOffset(0)]
        public nm_get_policy_output_standard_t standard;
        [FieldOffset(0)]
        public nm_get_policy_output_policy_id_invalid_t policy_id_invalid;
        [FieldOffset(0)]
        public nm_get_policy_output_domain_id_invalid_t domain_id_invalid;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_set_policy_alert_thresholds_input_t
    {
        public nm_domain_id_t domain;
        public byte policy;
        public byte numberOfAlertThresholds;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3, ArraySubType = UnmanagedType.U2)]
        public ushort[] alertThresholdsArray;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_policy_alert_thresholds_input_t
    {
        public nm_domain_id_t domain;
        public byte policy;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_policy_alert_thresholds_output_t
    {
        public byte numberOfAlertThresholds;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3, ArraySubType = UnmanagedType.U2)]
        public ushort[] alertThresholdsArray;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_policy_suspend_period_t
    {
        public byte startTime;
        public byte stopTime;
        public nm_suspend_period_recurrence_pattern_t recurrencePattern;

        public nm_policy_suspend_period_t(byte start, byte stop, nm_suspend_period_recurrence_pattern_t pattern)
        {
            startTime = start;
            stopTime = stop;
            recurrencePattern = pattern;
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_set_policy_suspend_periods_input_t
    {
	    public nm_domain_id_t domain;
	    public byte policy;
	    public byte numberOfSuspendPeriods;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5, ArraySubType = UnmanagedType.Struct)]
	    public nm_policy_suspend_period_t[] suspendPeriodsArray;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_policy_suspend_periods_input_t
    {
	    public nm_domain_id_t domain;
	    public byte policy;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_policy_suspend_periods_output_t
    {
	    public byte numberOfSuspendPeriods;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5, ArraySubType = UnmanagedType.Struct)]
	    public nm_policy_suspend_period_t[] suspendPeriods;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_reset_statistics_input_t
    {
        public nm_reset_statistics_mode_t mode;
        public nm_domain_id_t domain;
        public byte policy;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_statistics_input_t
    {
        public nm_get_statistics_mode_t mode;
        public nm_domain_id_t domain;
        public byte policy;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_statistics_output_t
    {
        public ushort currentValue;
        public ushort minimumValue;
        public ushort maximumValue;
        public ushort averageValue;
        public uint timestamp;
        public uint statisticsReportingPeriod;
        public nm_domain_id_t domain;
        public bool policyGlobalAdministrativeState;
        public bool policyOperationalState;
        public bool measurementsState;
        public bool policyActivationState;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_capabilities_input_t
    {
        public nm_domain_id_t domain;
        public nm_policy_trigger_type_t policyTriggerType;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_capabilities_output_t
    {
        public byte maxConcurrentSettings;
        public ushort maxValue;
        public ushort minValue;
        public uint minCorrectionTime;
        public uint maxCorrectionTime;
        public ushort minStatisticsReportingPeriod;
        public ushort maxStatisticsReportingPeriod;
        public nm_domain_id_t domainLimitingScope;
        public bool limitingBasedOn;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_version_output_t
    {
        public byte version;
        public byte ipmiVersion;
        public byte patchVersion;
        public byte majorFirmwareRevision;
        public byte minorFirmwareRevision;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_set_power_draw_range_input_t
    {
	    public nm_domain_id_t domain;
	    public ushort minimumPowerDraw;
	    public ushort maximumPowerDraw;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_alert_destination_t
    {
	    public byte channelNumber;
	    public bool destinationInformationReceiver;
	    public byte i2cSlaveAddressDestSelector;
	    public byte alertStringSelector;
	    public bool sendAlertString;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_set_alert_destination_input_t
    {
	    public nm_alert_destination_t alertDestination;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_alert_destination_output_t
    {
        public nm_alert_destination_t alertDestination;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_platform_characterization_launch_req_input_t
    {
        public nm_ptu_launch_power_characterization_on_hw_change_t launchAction;
        public nm_ptu_bmc_table_config_phase_action_t bmcTableConfigPhaseAction;
        public nm_ptu_bmc_phase_state_machine_action_t bmcPhaseStateMachineAction;
        public nm_ptu_power_domain_id_t powerDomainId;
        public nm_ptu_power_draw_characterization_point_t powerDrawCharacterizationPoint;
        public uint delay;
        public uint timeToRun;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_power_characterization_range_input_t
    {
        public nm_ptu_power_domain_id_t domainId;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_power_characterization_range_output_t
    {
        public uint timestamp;
        public ushort maxPowerDraw;
        public ushort minPowerDraw;
        public ushort effPowerDraw;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_cups_capabilities_output_t
    {
        public bool cupsEnabled;
        public bool cupsPoliciesAvailable;
        public byte cupsVersion;
        public byte reserved;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_cups_data_input_t
    {
        public nm_get_cups_data_parameter_t parameter;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_cups_data_output_index_t
    {
        public ushort index;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_cups_data_output_dynamic_t
    {
        public ushort cpu;
        public ushort memory;
        public ushort io;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_cups_data_output_base_t
    {
        public ulong cpu;
        public ulong memory;
        public ulong io;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_cups_data_output_aggregate_t
    {
        public ulong cpu;
        public ulong memory;
        public ulong io;
    }

    [StructLayout(LayoutKind.Explicit)]
    public struct nm_get_cups_data_output_t
    {
        [FieldOffset(0)]
        public nm_get_cups_data_output_index_t index;
        [FieldOffset(0)]
        public nm_get_cups_data_output_dynamic_t dynamic;
        [FieldOffset(0)]
        public nm_get_cups_data_output_base_t baseData;
        [FieldOffset(0)]
        public nm_get_cups_data_output_aggregate_t aggregate;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_cups_configuration_t
    {
        public bool cupsEnabled;
        public bool loadFactorTypeToggle; /*!< Set = Toggle Switch (0=No Change, 1=Toggle between dynamic and static), Get = Type (0=Dynamic, 1=Static) */
        public ushort staticCoreLoadFactor;
        public ushort staticMemoryLoadFactor;
        public ushort staticIoLoadFactor;
        public byte sampleCount;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_set_cups_configuration_input_t
    {
        public bool setCoreLoadFactor;
        public bool setMemoryLoadFactor;
        public bool setIoLoadFactor;
        public nm_cups_configuration_t config;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_cups_configuration_output_t
    {
        public nm_cups_configuration_t config;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_cups_policy_t
    {
        public bool policyEnabled;
        public bool policyStorageVolatileMemory;
        public bool sendAlertEnabled;
        public byte cupsThreshold;
        public ushort averagingWindow;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_set_cups_policies_input_t
    {
        public nm_cups_policy_id_t policyId;
        public nm_cups_target_identifier_t target;
        public nm_cups_policy_t policy;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_cups_policies_input_t
    {
        public nm_cups_policy_id_t policyId;
        public nm_cups_target_identifier_t target;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_get_cups_policies_output_t
    {
        public nm_cups_policy_t policy;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_discovery_parameters_t
    {
        public byte channel;
        public byte address;
        public byte nmHealthEvSensor;
        public byte nmExceptionEvSensor;
        public byte nmOperationalCapSensor;
        public byte nmAlertThresExcdSensor;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_ipmi_device_id_t
    {
        public byte deviceId;            // the id of this device
        public uint deviceRev;           // this devices revision
        public bool deviceProvidesSdr;   // does this device provide SDR access
        public uint firmwareRev;         // firmware revision
        public bool devNormOp;           // device is in normal operation mode
        public uint firmwareRev2;        // firmware revision 2
        public byte ipmiVersion;
        public bool isSensorDev;         // this device is a sensor device
        public bool isSdrRepoDev;        // is a sdr repo device
        public bool isSelDev;
        public bool isFruInvDev;         // is a fru 
        public bool isIpmbRevDev;
        public bool isIpmbGenDev;        // is a ipmi device
        public bool isBridgeDev;         // device acts as bridge
        public bool isChassisDev;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3, ArraySubType = UnmanagedType.U1)]
        public byte[] manufId;           // manfuctor id
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 2, ArraySubType = UnmanagedType.U1)]
        public byte[] productId;         // product id
    };

    public struct nm_ipmi_sdr_record_t
    {
        public ushort recordId;
        public byte version;
        public byte type;
        public byte len;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 1024, ArraySubType = UnmanagedType.U1)]
        public byte[] data;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_ipmi_sel_entry_t
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 1024, ArraySubType = UnmanagedType.U1)]
        public byte[] data;
        public uint len;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct tm
    {
        public int tm_sec;     /* seconds after the minute - [0,59] */
        public int tm_min;     /* minutes after the hour - [0,59] */
        public int tm_hour;    /* hours since midnight - [0,23] */
        public int tm_mday;    /* day of the month - [1,31] */
        public int tm_mon;     /* months since January - [0,11] */
        public int tm_year;    /* years since 1900 */
        public int tm_wday;    /* days since Sunday - [0,6] */
        public int tm_yday;    /* days since January 1 - [0,365] */
        public int tm_isdst;   /* daylight savings time flag */
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_ipmi_repo_info_t
    {
        public byte repoVersion;        // Version of the SEL
        public uint repoEntries;        // get total entries in SEL
        public uint repoFreeSpace;      // the amount of space left in the SEL
        public tm mostRecentAddTS;      // Timestamp from the last add command
        public tm mostRecentDelTS;      // timestamp from the last delete command
        public bool getAllocInfoSup;    // does this SEL support getAllocInfo command
        public bool reserveSup;         // reserve sel supported
        public bool parAddSup;          // partial add sel support
        public bool delSup;             // sel supports delete 
        public bool nonmodalSupported;  // non-modal SDR Repository update supported
        public bool modalSupported;     // modal SDR Repository update supported
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct nm_ipmi_fru_info_t
    {
        public ushort fruSize;
        public bool accessByWord;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct ipmi_acpi_power_state_t
    {
        public bool setSystemState;
        public bool setDeviceState;
	    public ipmi_acpi_system_power_state_t systemState;
	    public ipmi_acpi_device_power_state_t deviceState;
    }
    #endregion

    #region IPMI Req/Rsp Structures
    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct ipmi_req_t
    {
        public const int MAX_DATA_LEN = 490;

        [MarshalAs(UnmanagedType.U1)]
        public byte netFun;
        [MarshalAs(UnmanagedType.U1)]
        public byte cmd;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = MAX_DATA_LEN, ArraySubType = UnmanagedType.U1)]
        public byte[] data;
        [MarshalAs(UnmanagedType.U4)]
        public uint len;
        [MarshalAs(UnmanagedType.U1)]
        public byte rsAddr;
        [MarshalAs(UnmanagedType.U1)]
        public byte rsLun;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct ipmi_rsp_t
    {
        public const int MAX_DATA_LEN = 490;

        [MarshalAs(UnmanagedType.U1)]
        public byte compCode;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = MAX_DATA_LEN, ArraySubType = UnmanagedType.U1)]
        public byte[] data;
        [MarshalAs(UnmanagedType.U4)]
        public uint len;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct ipmi_capture_req_t
    {
        [MarshalAs(UnmanagedType.U4)]
        public int count;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = MAX_DATA_LEN, ArraySubType = UnmanagedType.U1)]
        public byte[] data;

        public const int MAX_DATA_LEN = 490;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct ipmi_capture_rsp_t
    {
        [MarshalAs(UnmanagedType.U4)]
        public int count;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = MAX_DATA_LEN, ArraySubType = UnmanagedType.U1)]
        public byte[] data;

        public const int MAX_DATA_LEN = 490;
    }
    #endregion

    class NMPRK_API
    {
        public const int NMPRK_SUCCESS = 0;
        public delegate void nmprk_debug_callback();

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetApiVersion", SetLastError = true)]
        public static extern string GetApiVersion();

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_StartDebugLogging", SetLastError = true)]
        public static extern int StartDebugLogging(
            [In] string file);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_StopDebugLogging", SetLastError = true)]
        public static extern int StopDebugLogging();

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_SetDebugCallback", SetLastError = true)]
        public static extern int SetDebugCallback(
            [In] nmprk_debug_callback cb);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_ConnectLocal", SetLastError = true)]
        public static extern int ConnectLocal(
            [In, Out] ref int handle);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_ConnectRemote", SetLastError = true)]
        public static extern int ConnectRemote(
            [In] ref nmprk_connect_remote_parameters_t input,
            [In, Out] ref int handle);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_Disconnect", SetLastError = true)]
        public static extern int Disconnect(
            [In] int handle);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetDiscoveryParameters", SetLastError = true)]
        public static extern int GetDiscoveryParameters(
            [In] int h,
            [In, Out] ref nm_discovery_parameters_t parameters);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetAllPolicies", SetLastError = true)]
        public static extern int GetAllPolicies(
            [In] int h,
            [In, Out] nm_policy_info_t[] policies,
            [In, Out] ref int policiesSize);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_SetDefaultNmCommandBridging", SetLastError = true)]
        public static extern int SetDefaultNmCommandBridging(
            [In] int h,
            [In] byte channel,
            [In] byte address);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_SendNonBridgedCommand", SetLastError = true)]
        public static extern int SendNonBridgedCommand(
            [In] int h,
            [In] ref ipmi_req_t req,
            [In, Out] ref ipmi_rsp_t rsp);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_SendBridgedCommand", SetLastError = true)]
        public static extern int SendBridgedCommand(
            [In] int h,
            [In] byte channel,
	        [In] byte address,
            [In] ref ipmi_req_t req,
            [In, Out] ref ipmi_rsp_t rsp);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_SendNmCommand", SetLastError = true)]
        public static extern int SendNmCommand(
            [In] int h,
            [In] ref ipmi_req_t req,
            [In, Out] ref ipmi_rsp_t rsp);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetLastRequestResponse", SetLastError = true)]
        public static extern int GetLastRequestResponse(
            [In] int h,
            [In, Out] ref ipmi_capture_req_t req,
            [In, Out] ref ipmi_capture_rsp_t rsp);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetErrorString", SetLastError = true)]
        protected static extern int GetErrorString(int statusCode, StringBuilder str, int maxLength);

        public static string GetErrorString(int statusCode)
        {
            StringBuilder sb = new StringBuilder(1024);
            GetErrorString(statusCode, sb, 1024);
            return sb.ToString();
        }

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_EnableDisablePolicyControl", SetLastError = true)]
        public static extern int EnableDisablePolicyControl(
            [In] int h,
            [In] ref nm_enable_disable_policy_control_input_t input);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_SetPolicy", SetLastError = true)]
        public static extern int SetPolicy(
            [In] int h,
            [In] ref nm_set_policy_input_t input);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetPolicy", SetLastError = true)]
        public static extern int GetPolicy(
            [In] int h,
            [In] ref nm_get_policy_input_t input,
            [In, Out] ref nm_get_policy_output_t output);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_SetPolicyAlertThresholds", SetLastError = true)]
        public static extern int SetPolicyAlertThresholds(
            [In] int h,
            [In] ref nm_set_policy_alert_thresholds_input_t input);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetPolicyAlertThresholds", SetLastError = true)]
        public static extern int GetPolicyAlertThresholds(
            [In] int h,
            [In] ref nm_get_policy_alert_thresholds_input_t input,
            [In, Out] ref nm_get_policy_alert_thresholds_output_t output);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_SetPolicySuspendPeriods", SetLastError = true)]
        public static extern int SetPolicySuspendPeriods(
            [In] int h,
            [In] ref nm_set_policy_suspend_periods_input_t input);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetPolicySuspendPeriods", SetLastError = true)]
        public static extern int GetPolicySuspendPeriods(
            [In] int h,
            [In] ref nm_get_policy_suspend_periods_input_t input,
            [In, Out] ref nm_get_policy_suspend_periods_output_t output);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_ResetStatistics", SetLastError = true)]
        public static extern int ResetStatistics(
            [In] int h,
            [In] ref nm_reset_statistics_input_t input);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetStatistics", SetLastError = true)]
        public static extern int GetStatistics(
            [In] int h,
            [In] ref nm_get_statistics_input_t input,
            [In, Out] ref nm_get_statistics_output_t output);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetCapabilities", SetLastError = true)]
        public static extern int GetCapabilities(
            [In] int h,
            [In] ref nm_get_capabilities_input_t input,
            [In, Out] ref nm_get_capabilities_output_t output);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetVersion", SetLastError = true)]
        public static extern int GetVersion(
            [In] int h,
            [In, Out] ref nm_get_version_output_t output);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_SetPowerDrawRange", SetLastError = true)]
        public static extern int SetPowerDrawRange(
            [In] int h,
            [In] ref nm_set_power_draw_range_input_t input);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_SetAlertDestination", SetLastError = true)]
        public static extern int SetAlertDestination(
            [In] int h,
            [In] ref nm_set_alert_destination_input_t input);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetAlertDestination", SetLastError = true)]
        public static extern int GetAlertDestination(
            [In] int h,
            [In, Out] ref nm_get_alert_destination_output_t output);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_PlatformCharacterizationLaunchRequest", SetLastError = true)]
        public static extern int PlatformCharacterizationLaunchRequest(
            [In] int h,
            [In] ref nm_platform_characterization_launch_req_input_t input);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetPowerCharacterizationRange", SetLastError = true)]
        public static extern int GetPowerCharacterizationRange(
            [In] int h,
            [In] ref nm_get_power_characterization_range_input_t input,
            [In, Out] ref nm_get_power_characterization_range_output_t output);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetCupsCapabilities", SetLastError = true)]
        public static extern int GetCupsCapabilities(
            [In] int h,
            [In, Out] ref nm_get_cups_capabilities_output_t output);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetCupsData", SetLastError = true)]
        public static extern int GetCupsData(
            [In] int h,
            [In] ref nm_get_cups_data_input_t input,
            [In, Out] ref nm_get_cups_data_output_t output);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_SetCupsConfiguration", SetLastError = true)]
        public static extern int SetCupsConfiguration(
            [In] int h,
            [In] ref nm_set_cups_configuration_input_t input);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetCupsConfiguration", SetLastError = true)]
        public static extern int GetCupsConfiguration(
            [In] int h,
            [In, Out] ref nm_get_cups_configuration_output_t output);
        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_SetCupsPolicies", SetLastError = true)]
        public static extern int SetCupsPolicies(
            [In] int h,
            [In] ref nm_set_cups_policies_input_t input);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_GetCupsPolicies", SetLastError = true)]
        public static extern int GetCupsPolicies(
            [In] int h,
            [In] ref nm_get_cups_policies_input_t input,
            [In, Out] ref nm_get_cups_policies_output_t output);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_GetDeviceId", SetLastError = true)]
        public static extern int IPMI_GetDeviceId(
            [In] int h,
            [In, Out] ref nm_ipmi_device_id_t devId);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_GetSdrInfo", SetLastError = true)]
        public static extern int IPMI_GetSdrInfo(
            [In] int h,
            [In, Out] ref nm_ipmi_repo_info_t info);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_GetSdrRecord", SetLastError = true)]
        public static extern int IPMI_GetSdrRecord(
            [In] int h,
            [In] ushort recordId,
            [In, Out] ref ushort nextRecord,
            [In, Out] ref nm_ipmi_sdr_record_t record);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_AddSdrRecord", SetLastError = true)]
        public static extern int IPMI_AddSdrRecord(
            [In] int h,
            [In] ref nm_ipmi_sdr_record_t record,
            [In, Out] ref ushort recordId);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_DeleteSdrRecord", SetLastError = true)]
        public static extern int IPMI_DeleteSdrRecord(
            [In] int h,
            [In] ushort recordId);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_ClearSdrRepository", SetLastError = true)]
        public static extern int IPMI_ClearSdrRepository(
            [In] int h);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_GetSelInfo", SetLastError = true)]
        public static extern int IPMI_GetSelInfo(
            [In] int h,
            [In, Out] ref nm_ipmi_repo_info_t info);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_GetSelEntry", SetLastError = true)]
        public static extern int IPMI_GetSelEntry(
            [In] int h,
            [In] ushort entryId,
            [In, Out] ref ushort nextEntry,
            [In, Out] ref nm_ipmi_sel_entry_t entry);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_DeleteSelEntry", SetLastError = true)]
        public static extern int IPMI_DeleteSelEntry(
            [In] int h,
            [In] ushort entryId);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_ClearSel", SetLastError = true)]
        public static extern int IPMI_ClearSel(
            [In] int h);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_GetFruInfo", SetLastError = true)]
        public static extern int IPMI_GetFruInfo(
            [In] int h,
            [In, Out] ref nm_ipmi_fru_info_t info);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_ReadFruData", SetLastError = true)]
        public static extern int IPMI_ReadFruData(
            [In] int h,
            [In] int offset,
            [In, Out] ref int length,
            [In, Out, MarshalAs(UnmanagedType.LPArray)] byte[] data);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_WriteFruData", SetLastError = true)]
        public static extern int IPMI_WriteFruData(
            [In] int h,
            [In] int offset,
            [In, Out] ref int length,
            [In, MarshalAs(UnmanagedType.LPArray)] byte[] data);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_GetAcpiPowerState", SetLastError = true)]
        public static extern int IPMI_GetAcpiPowerState(
            [In] int h,
            [In, Out] ref ipmi_acpi_power_state_t state);

        [DllImport("nmprkC", CallingConvention = CallingConvention.Cdecl, EntryPoint = "NMPRK_IPMI_SetAcpiPowerState", SetLastError = true)]
        public static extern int IPMI_SetAcpiPowerState(
            [In] int h,
            [In] ref ipmi_acpi_power_state_t state);
    }
}
