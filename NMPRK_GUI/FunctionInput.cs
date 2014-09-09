using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using System.Windows.Documents;
using NMPRK;

namespace NMPRK_GUI
{
    public abstract class FunctionInput : IEnumerable<NMPRK_Types.Type>
    {
        public List<NMPRK_Types.Type> TList { get; private set; }

        private Function _func = null;

        public FunctionInput(Function func)
        {
            _func = func;
            TList = new List<NMPRK_Types.Type>();

            FieldInfo[] fields = this.GetType().GetFields();
            foreach (FieldInfo f in fields)
            {
                object o = f.GetValue(this);
                if (o is NMPRK_Types.Type)
                    Add((NMPRK_Types.Type)o);
            }
        }

        public void Add(NMPRK_Types.Type item)
        {
            TList.Add(item);
        }

        public IEnumerator<NMPRK_Types.Type> GetEnumerator()
        {
            return TList.GetEnumerator();
        }

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public string Structure
        {
            get
            {
                string stru = string.Empty;
                if (_func != null)
                {
                    NMPRKReader.FunctionInputOutput fio = NMPRKReader.GetFunctionInputOutput(_func.Name);
                    stru = fio.Input;
                }
                return stru.Trim();
            }
        }

        public FlowDocument StructureFancy
        {
            get
            {
                return Function.ParseSyntax(Structure);
            }
        }
    }

    #region Node Manager Basic
    public class EnableDisablePolicyControlFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_enable_disable_policy_control_input_t\n{\n\tnm_policy_enable_disable_t flags;\n\tnm_domain_id_t domain;\n\tbyte_t policy;\n} nm_enable_disable_policy_control_input_t;"; } }

        public NMPRK_Types.Enum<nm_policy_enable_disable_t> Flags = new NMPRK_Types.Enum<nm_policy_enable_disable_t>("Flags", nm_policy_enable_disable_t.GLOBAL_ENABLE);
        public NMPRK_Types.U8 Policy = new NMPRK_Types.U8("Policy", 0);
        public NMPRK_Types.Enum<nm_domain_id_t> Domain = new NMPRK_Types.Enum<nm_domain_id_t>("Domain", nm_domain_id_t.PLATFORM);

        public EnableDisablePolicyControlFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_enable_disable_policy_control_input_t ToStruct()
        {
            NMPRK.nm_enable_disable_policy_control_input_t input = new nm_enable_disable_policy_control_input_t();
            input.flags = Flags.Value;
            input.domain = Domain.Value;
            input.policy = Policy.Value;
            return input;
        }
    }

    public class SetPolicyFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_set_policy_input_t\n{\n\tnm_policy_info_t info;\n\tbool_t removePolicy;\n} nm_set_policy_input_t;"; } }

        public NMPRK_Types.Enum<nm_domain_id_t> Domain = new NMPRK_Types.Enum<nm_domain_id_t>("Domain", nm_domain_id_t.PLATFORM);
        public NMPRK_Types.Bool Enabled = new NMPRK_Types.Bool("Policy Enabled", true);
        public NMPRK_Types.U8 Policy = new NMPRK_Types.U8("Policy", 0);
        public NMPRK_Types.Enum<nm_policy_trigger_type_t> TriggerType = new NMPRK_Types.Enum<nm_policy_trigger_type_t>("Policy Trigger Type", nm_policy_trigger_type_t.NO_POLICY_TRIGGER);
        public NMPRK_Types.Bool RemovePolicy = new NMPRK_Types.Bool("Remove Policy", false);
        public NMPRK_Types.Enum<nm_aggressive_cpu_power_correction_t> AggressiveCpuPowerCorrection = new NMPRK_Types.Enum<nm_aggressive_cpu_power_correction_t>("Aggressive CPU Power Correction", nm_aggressive_cpu_power_correction_t.DEFAULT);
        public NMPRK_Types.Bool ExceptionActionSendAlert = new NMPRK_Types.Bool("Exception Action - Send Alert", false);
        public NMPRK_Types.Bool ExceptionActionShutdown = new NMPRK_Types.Bool("Exception Action - Shutdown", false);
        public NMPRK_Types.U16 TargetLimit = new NMPRK_Types.U16("Policy Target Limit", 0, false);
        public NMPRK_Types.U32 CorrectionTimeLimit = new NMPRK_Types.U32("Correction Time Limit", 0, false);
        public NMPRK_Types.U16 TriggerLimit = new NMPRK_Types.U16("Policy Trigger Limit", 0, false);
        public NMPRK_Types.U16 StatisticsReportingPeriod = new NMPRK_Types.U16("Statistics Reporting Period", 0, false);

        public SetPolicyFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_set_policy_input_t ToStruct()
        {
            NMPRK.nm_set_policy_input_t input = new nm_set_policy_input_t();
            input.info = new nm_policy_info_t();
            input.info.domain = Domain.Value;
            input.info.policyEnabled = Enabled.Value;
            input.info.policy = Policy.Value;
            input.info.policyTriggerType = TriggerType.Value;
            input.removePolicy = RemovePolicy.Value;
            input.info.aggressiveCpuPowerCorrection = AggressiveCpuPowerCorrection.Value;
            input.info.policyExceptionActionSendAlert = ExceptionActionSendAlert.Value;
            input.info.policyExceptionActionShutdown = ExceptionActionShutdown.Value;
            input.info.policyTargetLimit = TargetLimit.Value;
            input.info.correctionTimeLimit = CorrectionTimeLimit.Value;
            input.info.policyTriggerLimit = TriggerLimit.Value;
            input.info.statisticsReportingPeriod = StatisticsReportingPeriod.Value;
            return input;
        }
    }

    public class GetPolicyFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_get_policy_input_t\n{\n\tnm_domain_id_t domain;\n\tbyte_t policy;\n} nm_get_policy_input_t;"; } }

        public NMPRK_Types.Enum<nm_domain_id_t> Domain = new NMPRK_Types.Enum<nm_domain_id_t>("Domain", nm_domain_id_t.PLATFORM);
        public NMPRK_Types.U8 Policy = new NMPRK_Types.U8("Policy", 0);

        public GetPolicyFunctionInput(Function func)
            : base(func)
        { 
        }

        public NMPRK.nm_get_policy_input_t ToStruct()
        {
            NMPRK.nm_get_policy_input_t input = new nm_get_policy_input_t();
            input.domain = Domain.Value;
            input.policy = Policy.Value;
            return input;
        }
    }

    public class SetPolicyAlertThresholdsFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_set_policy_alert_thresholds_input_t\n{\n\tnm_domain_id_t domain;\n\tbyte_t policy;\n\tbyte_t numberOfAlertThresholds;\n\tushort_t alertThresholdsArray[3];\n} nm_set_policy_alert_thresholds_input_t;"; } }

        public NMPRK_Types.Enum<nm_domain_id_t> Domain = new NMPRK_Types.Enum<nm_domain_id_t>("Domain", nm_domain_id_t.PLATFORM);
        public NMPRK_Types.U8 Policy = new NMPRK_Types.U8("Policy", 0);
        public NMPRK_Types.U8 AlertThresholdCount = new NMPRK_Types.U8("Number of Alert Thresholds", 0, false);
        public NMPRK_Types.U16 AlertThreshold1 = new NMPRK_Types.U16("Threshold 1", 0, false);
        public NMPRK_Types.U16 AlertThreshold2 = new NMPRK_Types.U16("Threshold 2", 0, false);
        public NMPRK_Types.U16 AlertThreshold3 = new NMPRK_Types.U16("Threshold 3", 0, false);

        public SetPolicyAlertThresholdsFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_set_policy_alert_thresholds_input_t ToStruct()
        {
            NMPRK.nm_set_policy_alert_thresholds_input_t input = new nm_set_policy_alert_thresholds_input_t();
            input.domain = Domain.Value;
            input.policy = Policy.Value;
            input.numberOfAlertThresholds = AlertThresholdCount.Value;
            input.alertThresholdsArray = new ushort[3];
            input.alertThresholdsArray[0] = AlertThreshold1.Value;
            input.alertThresholdsArray[1] = AlertThreshold2.Value;
            input.alertThresholdsArray[2] = AlertThreshold3.Value;
            return input;
        }
    }

    public class GetPolicyAlertThresholdsFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_get_policy_alert_thresholds_input_t\n{\n\tnm_domain_id_t domain;\n\tbyte_t policy;\n} nm_get_policy_alert_thresholds_input_t;"; } }

        public NMPRK_Types.Enum<nm_domain_id_t> Domain = new NMPRK_Types.Enum<nm_domain_id_t>("Domain", nm_domain_id_t.PLATFORM);
        public NMPRK_Types.U8 Policy = new NMPRK_Types.U8("Policy", 0);

        public GetPolicyAlertThresholdsFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_get_policy_alert_thresholds_input_t ToStruct()
        {
            NMPRK.nm_get_policy_alert_thresholds_input_t input = new nm_get_policy_alert_thresholds_input_t();
            input.domain = Domain.Value;
            input.policy = Policy.Value;
            return input;
        }
    }

    public class SetPolicySuspendPeriodsFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_set_policy_suspend_periods_input_t\n{\n\tnm_domain_id_t domain;\n\tbyte_t policy;\n\tbyte_t numberOfSuspendPeriods;\n\tnm_policy_suspend_period_t suspendPeriods[5];\n} nm_set_policy_suspend_periods_input_t;"; } }

        public NMPRK_Types.Enum<nm_domain_id_t> Domain = new NMPRK_Types.Enum<nm_domain_id_t>("Domain", nm_domain_id_t.PLATFORM);
        public NMPRK_Types.U8 Policy = new NMPRK_Types.U8("Policy", 0);
        public NMPRK_Types.U8 SuspendPeriodCount = new NMPRK_Types.U8("Number of Suspend Periods", 0, false);
        public NMPRK_Types.Struct_nm_policy_suspend_period_t SuspendPeriod1 = new NMPRK_Types.Struct_nm_policy_suspend_period_t("Suspend Period 1", new nm_policy_suspend_period_t());
        public NMPRK_Types.Struct_nm_policy_suspend_period_t SuspendPeriod2 = new NMPRK_Types.Struct_nm_policy_suspend_period_t("Suspend Period 2", new nm_policy_suspend_period_t());
        public NMPRK_Types.Struct_nm_policy_suspend_period_t SuspendPeriod3 = new NMPRK_Types.Struct_nm_policy_suspend_period_t("Suspend Period 3", new nm_policy_suspend_period_t());
        public NMPRK_Types.Struct_nm_policy_suspend_period_t SuspendPeriod4 = new NMPRK_Types.Struct_nm_policy_suspend_period_t("Suspend Period 4", new nm_policy_suspend_period_t());
        public NMPRK_Types.Struct_nm_policy_suspend_period_t SuspendPeriod5 = new NMPRK_Types.Struct_nm_policy_suspend_period_t("Suspend Period 5", new nm_policy_suspend_period_t());

        public SetPolicySuspendPeriodsFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_set_policy_suspend_periods_input_t ToStruct()
        {
            NMPRK.nm_set_policy_suspend_periods_input_t input = new nm_set_policy_suspend_periods_input_t();
            input.domain = Domain.Value;
            input.policy = Policy.Value;
            input.numberOfSuspendPeriods = SuspendPeriodCount.Value;
            input.suspendPeriodsArray = new nm_policy_suspend_period_t[5];
            input.suspendPeriodsArray[0] = SuspendPeriod1.Value;
            input.suspendPeriodsArray[1] = SuspendPeriod2.Value;
            input.suspendPeriodsArray[2] = SuspendPeriod3.Value;
            input.suspendPeriodsArray[3] = SuspendPeriod4.Value;
            input.suspendPeriodsArray[4] = SuspendPeriod5.Value;
            return input;
        }
    }

    public class GetPolicySuspendPeriodsFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_get_policy_suspend_periods_input_t\n{\n\tnm_domain_id_t domain;\n\tbyte_t policy;\n} nm_get_policy_suspend_periods_input_t;"; } }

        public NMPRK_Types.Enum<nm_domain_id_t> Domain = new NMPRK_Types.Enum<nm_domain_id_t>("Domain", nm_domain_id_t.PLATFORM);
        public NMPRK_Types.U8 Policy = new NMPRK_Types.U8("Policy", 0);

        public GetPolicySuspendPeriodsFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_get_policy_suspend_periods_input_t ToStruct()
        {
            NMPRK.nm_get_policy_suspend_periods_input_t input = new nm_get_policy_suspend_periods_input_t();
            input.domain = Domain.Value;
            input.policy = Policy.Value;
            return input;
        }
    }

    public class ResetStatisticsFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_reset_statistics_input_t\n{\n\tnm_reset_statistics_mode_t mode;\n\tnm_domain_id_t domain;\n\tbyte_t policy;\n} nm_reset_statistics_input_t;"; } }

        public NMPRK_Types.Enum<nm_reset_statistics_mode_t> Mode = new NMPRK_Types.Enum<nm_reset_statistics_mode_t>("Mode", nm_reset_statistics_mode_t.RESET_GLOBAL_POWER_TEMP);
        public NMPRK_Types.Enum<nm_domain_id_t> Domain = new NMPRK_Types.Enum<nm_domain_id_t>("Domain", nm_domain_id_t.PLATFORM);
        public NMPRK_Types.U8 Policy = new NMPRK_Types.U8("Policy", 0);

        public ResetStatisticsFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_reset_statistics_input_t ToStruct()
        {
            NMPRK.nm_reset_statistics_input_t input = new nm_reset_statistics_input_t();
            input.mode = Mode.Value;
            input.domain = Domain.Value;
            input.policy = Policy.Value;
            return input;
        }
    }

    public class GetStatisticsFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_get_statistics_input_t\n{\n\tnm_get_statistics_mode_t mode;\n\tnm_domain_id_t domain;\n\tbyte_t policy;\n} nm_get_statistics_input_t;"; } }

        public NMPRK_Types.Enum<nm_get_statistics_mode_t> Mode = new NMPRK_Types.Enum<nm_get_statistics_mode_t>("Mode", nm_get_statistics_mode_t.GET_GLOBAL_POWER);
        public NMPRK_Types.Enum<nm_domain_id_t> Domain = new NMPRK_Types.Enum<nm_domain_id_t>("Domain", nm_domain_id_t.PLATFORM);
        public NMPRK_Types.U8 Policy = new NMPRK_Types.U8("Policy", 0);

        public GetStatisticsFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_get_statistics_input_t ToStruct()
        {
            NMPRK.nm_get_statistics_input_t input = new nm_get_statistics_input_t();
            input.mode = Mode.Value;
            input.domain = Domain.Value;
            input.policy = Policy.Value;
            return input;
        }
    }

    public class GetCapabilitiesFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_get_capabilities_input_t\n{\n\tnm_domain_id_t domain;\n\tnm_policy_trigger_type_t policyTriggerType;\n} nm_get_capabilities_input_t;"; } }

        public NMPRK_Types.Enum<nm_domain_id_t> Domain = new NMPRK_Types.Enum<nm_domain_id_t>("Domain", nm_domain_id_t.PLATFORM);
        public NMPRK_Types.Enum<nm_policy_trigger_type_t> PolicyTriggerType = new NMPRK_Types.Enum<nm_policy_trigger_type_t>("Policy Trigger Type", 0);

        public GetCapabilitiesFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_get_capabilities_input_t ToStruct()
        {
            NMPRK.nm_get_capabilities_input_t input = new nm_get_capabilities_input_t();
            input.domain = Domain.Value;
            input.policyTriggerType = PolicyTriggerType.Value;
            return input;
        }
    }

    public class SetPowerDrawRangeFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_set_power_draw_range_input_t\n{\n\tnm_domain_id_t domain;\n\tushort_t minimumPowerDraw;\n\tushort_t maximumPowerDraw;\n} nm_set_power_draw_range_input_t;"; } }

        public NMPRK_Types.Enum<nm_domain_id_t> Domain = new NMPRK_Types.Enum<nm_domain_id_t>("Domain", nm_domain_id_t.PLATFORM);
        public NMPRK_Types.U16 MinPowerDraw = new NMPRK_Types.U16("Minimum Power Draw", 0, false);
        public NMPRK_Types.U16 MaxPowerDraw = new NMPRK_Types.U16("Maximum Power Draw", 0, false);

        public SetPowerDrawRangeFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_set_power_draw_range_input_t ToStruct()
        {
            NMPRK.nm_set_power_draw_range_input_t input = new nm_set_power_draw_range_input_t();
            input.domain = Domain.Value;
            input.minimumPowerDraw = MinPowerDraw.Value;
            input.maximumPowerDraw = MaxPowerDraw.Value;
            return input;
        }
    }

    public class SetAlertDestinationFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_set_alert_destination_input_t\n{\n\tnm_alert_destination_t alertDestination;\n} nm_set_alert_destination_input_t;"; } }

        public NMPRK_Types.U8 ChannelNumber = new NMPRK_Types.U8("Channel Number", 0);
        public NMPRK_Types.Bool UnregisterAlertReceiver = new NMPRK_Types.Bool("Unregister Alert Receiver", false);
        public NMPRK_Types.U8 DestinationSelector = new NMPRK_Types.U8("Destination Selector", 0);
        public NMPRK_Types.U8 AlertStringSelector = new NMPRK_Types.U8("Alert String Selector", 0);
        public NMPRK_Types.Bool SendAlertString = new NMPRK_Types.Bool("Send Alert String", false);

        public SetAlertDestinationFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_set_alert_destination_input_t ToStruct()
        {
            NMPRK.nm_set_alert_destination_input_t input = new nm_set_alert_destination_input_t();
            input.alertDestination.channelNumber = ChannelNumber.Value;
            input.alertDestination.destinationInformationReceiver = UnregisterAlertReceiver.Value;
            input.alertDestination.i2cSlaveAddressDestSelector = DestinationSelector.Value;
            input.alertDestination.alertStringSelector = AlertStringSelector.Value;
            input.alertDestination.sendAlertString = SendAlertString.Value;
            return input;
        }
    }
    #endregion

    public class PlatformCharacterizationLaunchRequestFunctionInput : FunctionInput
    {
        public NMPRK_Types.Enum<nm_ptu_launch_power_characterization_on_hw_change_t> LaunchAction = new NMPRK_Types.Enum<nm_ptu_launch_power_characterization_on_hw_change_t>("Launch Action", nm_ptu_launch_power_characterization_on_hw_change_t.DONT_LAUNCH_OR_CANCEL_PREV_LAUNCH);
        public NMPRK_Types.Enum<nm_ptu_bmc_table_config_phase_action_t> BmcTableConfigPhaseAction = new NMPRK_Types.Enum<nm_ptu_bmc_table_config_phase_action_t>("BMC Table Config Phase Action", nm_ptu_bmc_table_config_phase_action_t.BMC_TABLE_NO_ACTION);
        public NMPRK_Types.Enum<nm_ptu_bmc_phase_state_machine_action_t> BmcPhaseStateMachineAction = new NMPRK_Types.Enum<nm_ptu_bmc_phase_state_machine_action_t>("BMC Phase State Machine Action", nm_ptu_bmc_phase_state_machine_action_t.BMC_PHASE_NO_ACTION);
        public NMPRK_Types.Enum<nm_ptu_power_domain_id_t> PowerDomainId = new NMPRK_Types.Enum<nm_ptu_power_domain_id_t>("Power Domain Id", nm_ptu_power_domain_id_t.POWER_DOMAIN_PLATFORM);
        public NMPRK_Types.Enum<nm_ptu_power_draw_characterization_point_t> PowerDrawCharacterizationPoint = new NMPRK_Types.Enum<nm_ptu_power_draw_characterization_point_t>("Power Draw Characterization Point", nm_ptu_power_draw_characterization_point_t.POWER_DRAW_CHARACTERIZATION_POINT_MAX);
        public NMPRK_Types.U32 Delay = new NMPRK_Types.U32("Delay", 5000, false);
        public NMPRK_Types.U32 TimeToRun = new NMPRK_Types.U32("Time To Run", 0, false);

        public PlatformCharacterizationLaunchRequestFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_platform_characterization_launch_req_input_t ToStruct()
        {
            NMPRK.nm_platform_characterization_launch_req_input_t input = new nm_platform_characterization_launch_req_input_t();
            input.launchAction = LaunchAction.Value;
            input.bmcTableConfigPhaseAction = BmcTableConfigPhaseAction.Value;
            input.bmcPhaseStateMachineAction = BmcPhaseStateMachineAction.Value;
            input.powerDomainId = PowerDomainId.Value;
            input.powerDrawCharacterizationPoint = PowerDrawCharacterizationPoint.Value;
            input.delay = Delay.Value;
            input.timeToRun = TimeToRun.Value;
            return input;
        }
    }

    public class GetPowerCharacterizationRangeFunctionInput : FunctionInput
    {
        public NMPRK_Types.Enum<nm_ptu_power_domain_id_t> LaunchAction = new NMPRK_Types.Enum<nm_ptu_power_domain_id_t>("Domain ID", nm_ptu_power_domain_id_t.POWER_DOMAIN_PLATFORM);

        public GetPowerCharacterizationRangeFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_get_power_characterization_range_input_t ToStruct()
        {
            NMPRK.nm_get_power_characterization_range_input_t input = new nm_get_power_characterization_range_input_t();
            input.domainId = LaunchAction.Value;
            return input;
        }
    }

    public class GetCupsDataFunctionInput : FunctionInput
    {
        public NMPRK_Types.Enum<nm_get_cups_data_parameter_t> Parameter = new NMPRK_Types.Enum<nm_get_cups_data_parameter_t>("Parameter", nm_get_cups_data_parameter_t.CUPS_PARAMETER_INDEX);

        public GetCupsDataFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_get_cups_data_input_t ToStruct()
        {
            NMPRK.nm_get_cups_data_input_t input = new nm_get_cups_data_input_t();
            input.parameter = Parameter.Value;
            return input;
        }
    }

    public class SetCupsConfigurationFunctionInput : FunctionInput
    {
        public NMPRK_Types.Bool EnableCUPS = new NMPRK_Types.Bool("Enable CUPS", false);
        public NMPRK_Types.Bool SetCoreLoadFactor = new NMPRK_Types.Bool("Set Core Load Factor", false);
        public NMPRK_Types.Bool SetMemoryLoadFactor = new NMPRK_Types.Bool("Set Memory Load Factor", false);
        public NMPRK_Types.Bool SetIoLoadFactor = new NMPRK_Types.Bool("Set IO Load Factor", false);
        public NMPRK_Types.Bool ToggleSwitch = new NMPRK_Types.Bool("Toggle Dynamic/Static", false);
        public NMPRK_Types.U16 StaticCoreLoadFactor = new NMPRK_Types.U16("Static Core Load Factor", 0);
        public NMPRK_Types.U16 StaticMemoryLoadFactor = new NMPRK_Types.U16("Static Memory Load Factor", 0);
        public NMPRK_Types.U16 StaticIoLoadFactor = new NMPRK_Types.U16("Static IO Load Factor", 0);
        public NMPRK_Types.U8 SampleCount = new NMPRK_Types.U8("Sample Count", 0);

        public SetCupsConfigurationFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_set_cups_configuration_input_t ToStruct()
        {
            NMPRK.nm_set_cups_configuration_input_t input = new nm_set_cups_configuration_input_t();
            input.config.cupsEnabled = EnableCUPS.Value;
            input.setCoreLoadFactor = SetCoreLoadFactor.Value;
            input.setMemoryLoadFactor = SetMemoryLoadFactor.Value;
            input.setIoLoadFactor = SetIoLoadFactor.Value;
            input.config.loadFactorTypeToggle = ToggleSwitch.Value;
            input.config.staticCoreLoadFactor = StaticCoreLoadFactor.Value;
            input.config.staticMemoryLoadFactor = StaticMemoryLoadFactor.Value;
            input.config.staticIoLoadFactor = StaticIoLoadFactor.Value;
            input.config.sampleCount = SampleCount.Value;
            return input;
        }
    }

    public class SetCupsPoliciesFunctionInput : FunctionInput
    {
        public NMPRK_Types.Enum<nm_cups_policy_id_t> CupsPolicyId = new NMPRK_Types.Enum<nm_cups_policy_id_t>("CUPS Policy Id", nm_cups_policy_id_t.CUPS_CORE_DOMAIN);
        public NMPRK_Types.Enum<nm_cups_target_identifier_t> TargetIdentifier = new NMPRK_Types.Enum<nm_cups_target_identifier_t>("Target Identifier", nm_cups_target_identifier_t.CUPS_TARGET_BMC);
        public NMPRK_Types.Bool EnablePolicy = new NMPRK_Types.Bool("Enable Policy", false);
        public NMPRK_Types.Bool VolatileStorage = new NMPRK_Types.Bool("Volatile Storage", false);
        public NMPRK_Types.Bool EnableAlert = new NMPRK_Types.Bool("Enable Alert", false);
        public NMPRK_Types.U8 CupsThreshold = new NMPRK_Types.U8("CUPS Threshold", 0);
        public NMPRK_Types.U16 AveragingWindow = new NMPRK_Types.U16("Averaging Window", 0);

        public SetCupsPoliciesFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_set_cups_policies_input_t ToStruct()
        {
            NMPRK.nm_set_cups_policies_input_t input = new nm_set_cups_policies_input_t();
            input.policyId = CupsPolicyId.Value;
            input.target = TargetIdentifier.Value;
            input.policy.policyEnabled = EnablePolicy.Value;
            input.policy.policyStorageVolatileMemory = VolatileStorage.Value;
            input.policy.sendAlertEnabled = EnableAlert.Value;
            input.policy.cupsThreshold = CupsThreshold.Value;
            input.policy.averagingWindow = AveragingWindow.Value;
            return input;
        }
    }

    public class GetCupsPoliciesFunctionInput : FunctionInput
    {
        public NMPRK_Types.Enum<nm_cups_policy_id_t> CupsPolicyId = new NMPRK_Types.Enum<nm_cups_policy_id_t>("CUPS Policy Id", nm_cups_policy_id_t.CUPS_CORE_DOMAIN);
        public NMPRK_Types.Enum<nm_cups_target_identifier_t> TargetIdentifier = new NMPRK_Types.Enum<nm_cups_target_identifier_t>("Target Identifier", nm_cups_target_identifier_t.CUPS_TARGET_BMC);

        public GetCupsPoliciesFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_get_cups_policies_input_t ToStruct()
        {
            NMPRK.nm_get_cups_policies_input_t input = new nm_get_cups_policies_input_t();
            input.policyId = CupsPolicyId.Value;
            input.target = TargetIdentifier.Value;
            return input;
        }
    }

    public class IPMI_RecordIdFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "ushort_t recordId;"; } }

        public NMPRK_Types.U16 RecordId = new NMPRK_Types.U16("Record Id", 0);

        public IPMI_RecordIdFunctionInput(Function func)
            : base(func)
        {
        }

        public ushort ToValue()
        {
            return RecordId.Value;
        }
    }

    public class IPMI_AddSdrRecordFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nm_ipmi_record_t\n{\n\tushort_t nextRecord;\n\tbyte_t data[1024];\n\tunsigned int len;  \n} nm_ipmi_record_t;"; } }

        public NMPRK_Types.U8 Length = new NMPRK_Types.U8("Length", 0, false);
        public NMPRK_Types.ByteArray Data = new NMPRK_Types.ByteArray("Data", new byte[0]);

        public IPMI_AddSdrRecordFunctionInput(Function func)
            : base(func)
        {
        }

        public NMPRK.nm_ipmi_sdr_record_t ToStruct()
        {
            NMPRK.nm_ipmi_sdr_record_t rec = new nm_ipmi_sdr_record_t();
            rec.len = Length.Value;
            rec.data = Data.Value;
            return rec;
        }
    }

    public class IPMI_ReadFruDataFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "int offset;\nint length"; } }

        public NMPRK_Types.U32 Offset = new NMPRK_Types.U32("Offset", 0, false);
        public NMPRK_Types.U32 Length = new NMPRK_Types.U32("Length", 0, false);

        public IPMI_ReadFruDataFunctionInput(Function func)
            : base(func)
        {
        }
    }

    public class IPMI_WriteFruDataFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "int offset;\nint length\nbyte[] data"; } }

        public NMPRK_Types.U32 Offset = new NMPRK_Types.U32("Offset", 0, false);
        public NMPRK_Types.U32 Length = new NMPRK_Types.U32("Length", 0, false);
        public NMPRK_Types.ByteArray Data = new NMPRK_Types.ByteArray("Data", new byte[0]);

        public IPMI_WriteFruDataFunctionInput(Function func)
            : base(func)
        {
        }
    }

    public class IPMI_SetAcpiPowerStateFunctionInput : FunctionInput
    {
        //public override string Structure { get { return "typedef struct nmprk_acpi_power_state_t\n{\n\tipmi_acpi_system_power_state_t systemState;\n\tipmi_acpi_device_power_state_t deviceState;\n} nmprk_acpi_power_state_t;"; } }

        public NMPRK_Types.Bool SetSystemPowerState = new NMPRK_Types.Bool("Set System Power State", false);
        public NMPRK_Types.Bool SetDevicePowerState = new NMPRK_Types.Bool("Set Device Power State", false);
        public NMPRK_Types.Enum<ipmi_acpi_system_power_state_t> SystemPowerState = new NMPRK_Types.Enum<ipmi_acpi_system_power_state_t>("System Power State", ipmi_acpi_system_power_state_t.ACPI_POWER_STATE_SYS_S0G0);
        public NMPRK_Types.Enum<ipmi_acpi_device_power_state_t> DevicePowerState = new NMPRK_Types.Enum<ipmi_acpi_device_power_state_t>("Device Power State", ipmi_acpi_device_power_state_t.ACPI_POWER_STATE_DEV_D0);

        public IPMI_SetAcpiPowerStateFunctionInput(Function func)
            : base(func)
        {
        }
    }
}
