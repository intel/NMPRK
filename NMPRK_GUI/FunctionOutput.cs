using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Documents;
using NMPRK;

namespace NMPRK_GUI
{
    public abstract class FunctionOutput : IEnumerable<NMPRK_Types.Type>
    {
        public List<NMPRK_Types.Type> TList { get; private set; }

        private Function _func = null;

        public Function Function
        {
            get { return _func; }
        }

        public FunctionOutput(Function func)
        {
            _func = func;
            TList = new List<NMPRK_Types.Type>();
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
                    stru = fio.Output;
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
    public class GetPolicyFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_get_policy_output_t\n{\n\tunion {\n\t\tstruct {\n\t\t\tnm_policy_info_t info;\n\t\t\tbool_t perDomainPolicyControlEnabled;\n\t\t\tbool_t globalPolicyControlEnabled;\n\t\t\tbool_t policyCreatedByOtherClient;\n\t\t} standard;\n\t\tstruct {\n\t\t\tbool_t valid;\n\t\t\tbyte_t nextValidPolicyId;\n\t\t\tbyte_t numberOfDefinedPoliciesForDomain;\n\t\t} policy_id_invalid;\n\t\tstruct {\n\t\t\tbool_t valid;\n\t\t\tbyte_t nextValidDomainId;\n\t\t\tbyte_t numberOfAvailableDomains;\n\t\t} domain_id_invalid;\n\t} u;\n} nm_get_policy_output_t;"; } }

        public void Update(int completionCode, nm_get_policy_output_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.Enum<nm_domain_id_t>("Domain", output.standard.info.domain));
                    Add(new NMPRK_Types.Bool("Policy Enabled", output.standard.info.policyEnabled));
                    Add(new NMPRK_Types.Bool("Per Domain Policy Control", output.standard.perDomainPolicyControlEnabled));
                    Add(new NMPRK_Types.Bool("Global Policy Control", output.standard.globalPolicyControlEnabled));
                    Add(new NMPRK_Types.Bool("Policy Created By Other Client", output.standard.policyCreatedByOtherClient));
                    Add(new NMPRK_Types.Enum<nm_policy_trigger_type_t>("Policy Trigger Type", output.standard.info.policyTriggerType));
                    Add(new NMPRK_Types.Bool("Policy Exception - Shutdown", output.standard.info.policyExceptionActionShutdown));
                    Add(new NMPRK_Types.Bool("Policy Exception - Send Alert", output.standard.info.policyExceptionActionSendAlert));
                    Add(new NMPRK_Types.U16("Power Limit", output.standard.info.policyTargetLimit, false));
                    Add(new NMPRK_Types.U32("Correction Time Limit", output.standard.info.correctionTimeLimit, false));
                    Add(new NMPRK_Types.U16("Policy Trigger Limit", output.standard.info.policyTriggerLimit, false));
                    Add(new NMPRK_Types.U16("Statistics Reporting Period", output.standard.info.statisticsReportingPeriod, false));
                    break;
                case 0x80:
                    Add(new NMPRK_Types.U8("Next Valid Policy ID", output.policy_id_invalid.nextValidPolicyId));
                    Add(new NMPRK_Types.U8("Number Of Defined Policies For Domain", output.policy_id_invalid.numberOfDefinedPoliciesForDomain));
                    break;
                case 0x81:
                    Add(new NMPRK_Types.U8("Next Valid Domain ID", output.domain_id_invalid.nextValidDomainId));
                    Add(new NMPRK_Types.U8("Number Of Available Domains", output.domain_id_invalid.numberOfAvailableDomains));
                    break;
            }
        }

        public GetPolicyFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class GetPolicyAlertThresholdsFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_get_policy_alert_thresholds_output_t\n{\n\tbyte_t numberOfAlertThresholds;\n\tushort_t alertThresholdsArray[3];\n} nm_get_policy_alert_thresholds_output_t;"; } }

        public void Update(int completionCode, nm_get_policy_alert_thresholds_output_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U8("Number of Alert Thresholds", output.numberOfAlertThresholds, false));
                    Add(new NMPRK_Types.U16("Alert Threshold 1", output.alertThresholdsArray[0], false));
                    Add(new NMPRK_Types.U16("Alert Threshold 2", output.alertThresholdsArray[1], false));
                    Add(new NMPRK_Types.U16("Alert Threshold 3", output.alertThresholdsArray[2], false));
                    break;
            }
        }

        public GetPolicyAlertThresholdsFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class GetPolicySuspendPeriodsFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_get_policy_suspend_periods_output_t\n{\n\tbyte_t numberOfSuspendPeriods;\n\tnm_policy_suspend_period_t suspendPeriods[5];\n} nm_get_policy_suspend_periods_output_t;"; } }

        public void Update(int completionCode, nm_get_policy_suspend_periods_output_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U8("Number of Suspend Periods", output.numberOfSuspendPeriods, false));
                    for (int i = 0; i < output.numberOfSuspendPeriods; i++)
                        Add(new NMPRK_Types.Struct_nm_policy_suspend_period_t("Suspend Period " + (i + 1), output.suspendPeriods[i]));
                    break;
            }
        }

        public GetPolicySuspendPeriodsFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class GetStatisticsFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_get_statistics_output_t\n{\n\tushort_t currentValue;\n\tushort_t minimumValue;\n\tushort_t maximumValue;\n\tushort_t averageValue;\n\tuint_t timestamp;\n\tuint_t statisticsReportingPeriod;\n\tnm_domain_id_t domain;\n\tbool_t policyGlobalAdministrativeState;\n\tbool_t policyOperationalState;\n\tbool_t measurementsState;\n\tbool_t policyActivationState;\n} nm_get_statistics_output_t;"; } }

        public void Update(int completionCode, nm_get_statistics_output_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U16("Current Value", output.currentValue, false));
                    Add(new NMPRK_Types.U16("Minimum Value", output.minimumValue, false));
                    Add(new NMPRK_Types.U16("Maximum Value", output.maximumValue, false));
                    Add(new NMPRK_Types.U16("Average Value", output.averageValue, false));
                    Add(new NMPRK_Types.U32("Timestamp", output.timestamp));
                    Add(new NMPRK_Types.U32("Statistics Reporting Period", output.statisticsReportingPeriod, false));
                    Add(new NMPRK_Types.Enum<nm_domain_id_t>("Domain", output.domain));
                    Add(new NMPRK_Types.Bool("Policy/Global Admin State", output.policyGlobalAdministrativeState));
                    Add(new NMPRK_Types.Bool("Policy Operational State", output.policyOperationalState));
                    Add(new NMPRK_Types.Bool("Measurements State", output.measurementsState));
                    Add(new NMPRK_Types.Bool("Policy Activation State", output.policyActivationState));
                    break;
            }
        }

        public GetStatisticsFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class GetCapabilitiesFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_get_capabilities_output_t\n{\n\tbyte_t maxConcurrentSettings;\n\tushort_t maxValue;\n\tushort_t minValue;\n\tuint_t minCorrectionTime;\n\tuint_t maxCorrectionTime;\n\tushort_t minStatisticsReportingPeriod;\n\tushort_t maxStatisticsReportingPeriod;\n\tnm_domain_id_t domainLimitingScope;\n\tbool_t limitingBasedOn;\n} nm_get_capabilities_output_t;"; } }

        public void Update(int completionCode, nm_get_capabilities_output_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U8("Max Concurrent Settings", output.maxConcurrentSettings, false));
                    Add(new NMPRK_Types.U16("Max Value", output.maxValue, false));
                    Add(new NMPRK_Types.U16("Min Value", output.minValue, false));
                    Add(new NMPRK_Types.U32("Min Correction Time", output.minCorrectionTime, false));
                    Add(new NMPRK_Types.U32("Max Correction Time", output.maxCorrectionTime, false));
                    Add(new NMPRK_Types.U16("Min Statistics Reporting Period", output.minStatisticsReportingPeriod, false));
                    Add(new NMPRK_Types.U16("Max Statistics Reporting Period", output.maxStatisticsReportingPeriod, false));
                    Add(new NMPRK_Types.Enum<nm_domain_id_t>("Domain Limiting Scope", output.domainLimitingScope));
                    Add(new NMPRK_Types.Bool("Limiting based on", output.limitingBasedOn));
                    break;
            }
        }

        public GetCapabilitiesFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class GetVersionFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_get_version_output_t\n{\n\tbyte_t version;\n\tbyte_t ipmiVersion;\n\tbyte_t patchVersion;\n\tbyte_t majorFirmwareRevision;\n\tbyte_t minorFirmwareRevision;\n} nm_get_version_output_t;"; } }

        public void Update(int completionCode, nm_get_version_output_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U8("Node Manager Version", output.version));
                    Add(new NMPRK_Types.U8("IPMI Version", output.ipmiVersion));
                    Add(new NMPRK_Types.U8("Patch Version", output.patchVersion));
                    Add(new NMPRK_Types.U8("Major Firmware Version", output.majorFirmwareRevision));
                    Add(new NMPRK_Types.U8("Minor Firmware Version", output.minorFirmwareRevision));
                    break;
            }
        }

        public GetVersionFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class GetAlertDestinationFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_get_alert_destination_output_t\n{\n\tnm_alert_destination_t alertDestination;\n} nm_get_alert_destination_output_t;"; } }

        public void Update(int completionCode, nm_get_alert_destination_output_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U8("Channel Number", output.alertDestination.channelNumber));
                    Add(new NMPRK_Types.Bool("Unregister Alert Receiver", output.alertDestination.destinationInformationReceiver));
                    Add(new NMPRK_Types.U8("Destination Selector", output.alertDestination.i2cSlaveAddressDestSelector));
                    Add(new NMPRK_Types.U8("Alert String Selector", output.alertDestination.alertStringSelector));
                    Add(new NMPRK_Types.Bool("Send Alert String", output.alertDestination.sendAlertString));
                    break;
            }
        }

        public GetAlertDestinationFunctionOutput(Function func)
            : base(func)
        {
        }
    }
    #endregion

    public class GetPowerCharacterizationRangeFunctionOutput : FunctionOutput
    {
        public void Update(int completionCode, nm_get_power_characterization_range_output_t output)
        {
            TList.Clear();

            Add(new NMPRK_Types.U32("Timestamp", output.timestamp));
            Add(new NMPRK_Types.U16("Max Power Draw", output.maxPowerDraw, false));
            Add(new NMPRK_Types.U16("Min Power Draw", output.minPowerDraw, false));
            Add(new NMPRK_Types.U16("Efficient Power Draw", output.effPowerDraw, false));
        }

        public GetPowerCharacterizationRangeFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    #region CUPS
    public class GetCupsCapabiltiesFunctionOutput : FunctionOutput
    {
        public void Update(int completionCode, nm_get_cups_capabilities_output_t output)
        {
            TList.Clear();

            Add(new NMPRK_Types.Bool("CUPS Feature Enabled", output.cupsEnabled));
            Add(new NMPRK_Types.Bool("CUPS Policies Available", output.cupsPoliciesAvailable));
            Add(new NMPRK_Types.U8("CUPS Version", output.cupsVersion, false));
        }

        public GetCupsCapabiltiesFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class GetCupsDataFunctionOutput : FunctionOutput
    {
        public void Update(int completionCode, nm_get_cups_data_output_t output)
        {
            TList.Clear();

            GetCupsDataFunctionInput input = Function.Input as GetCupsDataFunctionInput;

            switch (input.Parameter.Value)
            {
                case nm_get_cups_data_parameter_t.CUPS_PARAMETER_INDEX:
                    Add(new NMPRK_Types.U16("Index", output.index.index, false));
                    break;
                case nm_get_cups_data_parameter_t.CUPS_PARAMETER_DYNAMIC:
                    Add(new NMPRK_Types.U16("CPU", output.dynamic.cpu, false));
                    Add(new NMPRK_Types.U16("Memory", output.dynamic.memory, false));
                    Add(new NMPRK_Types.U16("IO", output.dynamic.io, false));
                    break;
                case nm_get_cups_data_parameter_t.CUPS_PARAMETER_BASE:
                    Add(new NMPRK_Types.U64("CPU", output.baseData.cpu));
                    Add(new NMPRK_Types.U64("Memory", output.baseData.memory));
                    Add(new NMPRK_Types.U64("IO", output.baseData.io));
                    break;
                case nm_get_cups_data_parameter_t.CUPS_PARAMETER_AGGREGATE:
                    Add(new NMPRK_Types.U64("CPU", output.aggregate.cpu));
                    Add(new NMPRK_Types.U64("Memory", output.aggregate.memory));
                    Add(new NMPRK_Types.U64("IO", output.aggregate.io));
                    break;
            }
        }

        public GetCupsDataFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class GetCupsConfigurationFunctionOutput : FunctionOutput
    {
        public void Update(int completionCode, nm_get_cups_configuration_output_t output)
        {
            TList.Clear();

            Add(new NMPRK_Types.Bool("CUPS Feature Enabled", output.config.cupsEnabled));
            Add(new NMPRK_Types.Bool("Load Factor Static", output.config.loadFactorTypeToggle));
            Add(new NMPRK_Types.U16("Static Core Load Factor", output.config.staticCoreLoadFactor, false));
            Add(new NMPRK_Types.U16("Static Memory Load Factor", output.config.staticMemoryLoadFactor, false));
            Add(new NMPRK_Types.U16("Static IO Load Factor", output.config.staticIoLoadFactor, false));
            Add(new NMPRK_Types.U8("Sample Count", output.config.sampleCount, false));
        }

        public GetCupsConfigurationFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class GetCupsPoliciesFunctionOutput : FunctionOutput
    {
        public void Update(int completionCode, nm_get_cups_policies_output_t output)
        {
            TList.Clear();

            Add(new NMPRK_Types.Bool("Enable Policy", output.policy.policyEnabled));
            Add(new NMPRK_Types.Bool("Volatile Storage", output.policy.policyStorageVolatileMemory));
            Add(new NMPRK_Types.Bool("Enable Alert", output.policy.sendAlertEnabled));
            Add(new NMPRK_Types.U8("CUPS Threshold", output.policy.cupsThreshold, false));
            Add(new NMPRK_Types.U16("Averaging Window", output.policy.averagingWindow, false));
        }

        public GetCupsPoliciesFunctionOutput(Function func)
            : base(func)
        {
        }
    }
    #endregion

    public class GetDiscoveryParametersFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_discovery_parameters_t\n{\n\tbyte_t channel;\n\tbyte_t address;\n\tbyte_t nmHealthEvSensor;\n\tbyte_t nmExceptionEvSensor;\n\tbyte_t nmOperationalCapSensor;\n\tbyte_t nmAlertThresExcdSensor;\n} nm_discovery_parameters_t;"; } }

        public void Update(int completionCode, nm_discovery_parameters_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U8("Address", output.address));
                    Add(new NMPRK_Types.U8("Channel", output.channel));
                    Add(new NMPRK_Types.U8("Health Event Sensor", output.nmHealthEvSensor));
                    Add(new NMPRK_Types.U8("Exception Event Sensor", output.nmExceptionEvSensor));
                    Add(new NMPRK_Types.U8("Operational Cap. Sensor", output.nmOperationalCapSensor));
                    Add(new NMPRK_Types.U8("Alert Threshold Exceeded Sensor", output.nmAlertThresExcdSensor));
                    break;
            }
        }

        public GetDiscoveryParametersFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class IPMI_GetDeviceIdFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_ipmi_device_id_t {\n\tbyte_t deviceId;\n\tuint_t deviceRev;\n\tbool_t deviceProvidesSdr;\n\tuint_t firmwareRev;\n\tbool_t devNormOp;\n\tuint_t firmwareRev2;\n\tbyte_t ipmiVersion;\n\tbool_t isSensorDev;\n\tbool_t isSdrRepoDev;\n\tbool_t isSelDev;\n\tbool_t isFruInvDev; \n\tbool_t isIpmbRevDev;\n\tbool_t isIpmbGenDev;\n\tbool_t isBridgeDev;\n\tbool_t isChassisDev;\n\tbyte_t manufId[3];\n\tbyte_t productId[2];\n} nm_ipmi_device_id_t;"; } }

        public void Update(int completionCode, nm_ipmi_device_id_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U8("Device ID", output.deviceId));
                    Add(new NMPRK_Types.Bool("Device Provides SDRs", output.deviceProvidesSdr));
                    Add(new NMPRK_Types.U32("Device Revision", output.deviceRev));
                    Add(new NMPRK_Types.U32("Firmware Revision 1", output.firmwareRev));
                    Add(new NMPRK_Types.Bool("Device Available", output.devNormOp));
                    Add(new NMPRK_Types.U32("Firmware Revision 1", output.firmwareRev2));
                    Add(new NMPRK_Types.U32("IPMI Version", output.ipmiVersion));
                    Add(new NMPRK_Types.Bool("Chassis Device", output.isChassisDev));
                    Add(new NMPRK_Types.Bool("Bridge Device", output.isBridgeDev));
                    Add(new NMPRK_Types.Bool("IPMB Event Generator", output.isIpmbGenDev));
                    Add(new NMPRK_Types.Bool("IPMB Event Receiver", output.isIpmbRevDev));
                    Add(new NMPRK_Types.Bool("FRU Inventory Device", output.isFruInvDev));
                    Add(new NMPRK_Types.Bool("SEL Device", output.isSelDev));
                    Add(new NMPRK_Types.Bool("SDR Repository Device", output.isSdrRepoDev));
                    Add(new NMPRK_Types.Bool("Sensor Device", output.isSensorDev));
                    Add(new NMPRK_Types.ByteArray("Manufacturer ID", output.manufId));
                    Add(new NMPRK_Types.ByteArray("Product ID", output.productId));
                    break;
            }
        }

        public IPMI_GetDeviceIdFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class IPMI_GetSdrInfoFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_ipmi_repo_info_t {\n\tbyte_t repoVersion;\n\tunsigned int repoEntries;\n\tunsigned int repoFreeSpace;\n\ttm mostRecentAddTS;\n\ttm mostRecentDelTS;\n\tbool getAllocInfoSup;\n\tbool reserveSup;\n\tbool parAddSup;\n\tbool delSup;\n\tbool nonmodalSupported;\n\tbool modalSupported;\n} nm_ipmi_repo_info_t;"; } }

        public void Update(int completionCode, nm_ipmi_repo_info_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U8("Version", output.repoVersion));
                    Add(new NMPRK_Types.U32("Entries", output.repoEntries, false));
                    Add(new NMPRK_Types.U32("Free Space", output.repoFreeSpace, false));
                    // TODO: mostRecentAddTS
                    // TODO: mostRecentDelTS
                    Add(new NMPRK_Types.Bool("Get SDR Repository Allocation Information Support", output.getAllocInfoSup));
                    Add(new NMPRK_Types.Bool("Reserve SDR Repository Support", output.reserveSup));
                    Add(new NMPRK_Types.Bool("Partial Add SDR Support", output.parAddSup));
                    Add(new NMPRK_Types.Bool("Delete SDR Support", output.delSup));
                    Add(new NMPRK_Types.Bool("Non-Modal Support", output.nonmodalSupported));
                    Add(new NMPRK_Types.Bool("Modal Support", output.modalSupported));
                    break;
            }
        }

        public IPMI_GetSdrInfoFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class IPMI_GetRecordFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_ipmi_record_t\n{\n\tushort_t nextRecord;\n\tbyte_t data[1024];\n\tunsigned int len;  \n} nm_ipmi_record_t;"; } }

        public void Update(int completionCode, ushort nextRecord, nm_ipmi_sdr_record_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U16("Next Record", nextRecord));
                    Add(new NMPRK_Types.U16("Record ID", output.recordId));
                    Add(new NMPRK_Types.U8("Version", output.version));
                    Add(new NMPRK_Types.U8("Type", output.type));
                    Add(new NMPRK_Types.U32("Length", output.len));
                    Add(new NMPRK_Types.ByteArray("Data", output.data.Take((int)output.len).ToArray()));
                    break;
            }
        }

        public IPMI_GetRecordFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class IPMI_GetEntryFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_ipmi_sel_entry_t\n{\n\tushort_t nextEntry;\n\tbyte_t data[1024];\n\tuint_t len; \n} nm_ipmi_sel_entry_t;"; } }

        public void Update(int completionCode, ushort nextEntry, nm_ipmi_sel_entry_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    
                    Add(new NMPRK_Types.U16("Next Entry", nextEntry));
                    Add(new NMPRK_Types.U32("Length", output.len));
                    Add(new NMPRK_Types.ByteArray("Data", output.data.Take((int)output.len).ToArray()));
                    break;
            }
        }

        public IPMI_GetEntryFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class IPMI_AddSdrRecordFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "ushort_t recordId;"; } }

        public void Update(int completionCode, ushort output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U16("Record ID", output));
                    break;
            }
        }

        public IPMI_AddSdrRecordFunctionOutput(Function func)
            : base(func)
        {
        }
    }
    
    public class IPMI_GetSelInfoFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_ipmi_repo_info_t {\n\tbyte_t repoVersion;\n\tunsigned int repoEntries;\n\tunsigned int repoFreeSpace;\n\ttm mostRecentAddTS;\n\ttm mostRecentDelTS;\n\tbool getAllocInfoSup;\n\tbool reserveSup;\n\tbool parAddSup;\n\tbool delSup;\n\tbool nonmodalSupported;\n\tbool modalSupported;\n} nm_ipmi_repo_info_t;"; } }

        public void Update(int completionCode, nm_ipmi_repo_info_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U8("Version", output.repoVersion));
                    Add(new NMPRK_Types.U32("Entries", output.repoEntries, false));
                    Add(new NMPRK_Types.U32("Free Space", output.repoFreeSpace, false));
                    // TODO: mostRecentAddTS
                    // TODO: mostRecentDelTS
                    Add(new NMPRK_Types.Bool("Get SEL Repository Allocation Information Support", output.getAllocInfoSup));
                    Add(new NMPRK_Types.Bool("Reserve SEL Repository Support", output.reserveSup));
                    Add(new NMPRK_Types.Bool("Partial Add SEL Support", output.parAddSup));
                    Add(new NMPRK_Types.Bool("Delete SEL Support", output.delSup));
                    break;
            }
        }

        public IPMI_GetSelInfoFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class IPMI_GetFruInfoFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nm_ipmi_fru_info_t\n{\n\tushort_t fruSize;\n\tbool_t accessByWord;\n} nm_ipmi_fru_info_t;"; } }

        public void Update(int completionCode, nm_ipmi_fru_info_t output)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U16("FRU Size", output.fruSize, false));
                    Add(new NMPRK_Types.Bool("Access By Word", output.accessByWord));
                    break;
            }
        }

        public IPMI_GetFruInfoFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class IPMI_ReadFruDataFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "int_t *length;\nbyte_t *data"; } }

        public void Update(int completionCode, int length, byte[] data)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U32("Length", (uint)length, false));
                    Add(new NMPRK_Types.ByteArray("Data", data));
                    break;
            }
        }

        public IPMI_ReadFruDataFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class IPMI_WriteFruDataFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "int_t *length;"; } }

        public void Update(int completionCode, int length)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.U32("Length", (uint)length, false));
                    break;
            }
        }

        public IPMI_WriteFruDataFunctionOutput(Function func)
            : base(func)
        {
        }
    }

    public class IPMI_GetAcpiPowerStateFunctionOutput : FunctionOutput
    {
        //public override string Structure{ get { return "typedef struct nmprk_acpi_power_state_t\n{\n\tipmi_acpi_system_power_state_t systemState;\n\tipmi_acpi_device_power_state_t deviceState;\n} nmprk_acpi_power_state_t;"; } }

        public void Update(int completionCode, ipmi_acpi_power_state_t state)
        {
            TList.Clear();

            switch (completionCode)
            {
                case 0:
                    Add(new NMPRK_Types.Enum<ipmi_acpi_system_power_state_t>("System Power State", state.systemState));
                    Add(new NMPRK_Types.Enum<ipmi_acpi_device_power_state_t>("Device Power State", state.deviceState));
                    break;
            }
        }

        public IPMI_GetAcpiPowerStateFunctionOutput(Function func)
            : base(func)
        {
        }
    }
}
