using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Windows.Documents;
using System.Windows.Media;
using System.Text.RegularExpressions;

namespace NMPRK_GUI
{
    public abstract class Function : INotifyPropertyChanged
    {
        public abstract string Name { get; }
        public string Signature { get { return NMPRKReader.GetFunctionSignature(Name); } }
        public abstract FunctionInput Input { get; }
        public abstract FunctionOutput Output { get; }
        //protected abstract int InvokeSimulation(int handle);
        protected abstract int InvokeInternal(int handle);
        protected abstract int InvokeInternalSimulation();

        private int _returnValue = int.MinValue;
        public int ReturnValue
        {
            get
            {
                return _returnValue;
            }
            protected set
            {
                _returnValue = value;
                ReturnValueString = GetReturnValueString(_returnValue);
                OnPropertyChanged("ReturnValue");
            }
        }

        private string _returnValueString = "";
        public string ReturnValueString
        { 
            get
            {
                return _returnValueString;
            }
            set
            {
                _returnValueString = value;
                OnPropertyChanged("ReturnValueString");
            }
        }

        public virtual string FunctionProhibitedMsg
        {
            get { return null; }
        }

        public virtual string FunctionWarningMsg
        {
            get { return null; }
        }

        public override string ToString()
        {
            return Name;
        }

        private string GetReturnValueString(int ret)
        {
            if (ret == int.MinValue)
                return "";

            string msg = "";

            switch (ret)
            {
                case 0:
                    msg = "Success";
                    break;
                default:
                    msg = NMPRK.NMPRK_API.GetErrorString(ret);
                    break;
            }

            if (ret >= 0)
                msg = ret.ToString("X2") + "h - " + msg;
            else
                msg = ret.ToString() + msg;
            return msg;
        }

        private string _requestByteString = "";
        public string RequestByteString
        {
            get
            {
                return _requestByteString;
            }
            protected set
            {
                _requestByteString = value;
                OnPropertyChanged("RequestByteString");
            }
        }

        private string _responseByteString = "";
        public string ResponseByteString
        {
            get
            {
                return _responseByteString;
            }
            protected set
            {
                _responseByteString = value;
                OnPropertyChanged("ResponseByteString");
            }
        }

        public int Invoke(int handle)
        {
            RequestByteString = "";
            ResponseByteString = "";
            ReturnValueString = "";

            int ret = InvokeInternal(handle);

            NMPRK.ipmi_capture_req_t req = new NMPRK.ipmi_capture_req_t();
            req.count = 0;
            req.data = new byte[NMPRK.ipmi_capture_req_t.MAX_DATA_LEN];
            NMPRK.ipmi_capture_rsp_t rsp = new NMPRK.ipmi_capture_rsp_t();
            rsp.count = 0;
            rsp.data = new byte[NMPRK.ipmi_capture_rsp_t.MAX_DATA_LEN];
            if (NMPRK.NMPRK_API.GetLastRequestResponse(handle, ref req, ref rsp) == 0)
            {
                string reqStr = "";
                for (int i = 0; i < req.count; i++)
                {
                    if (reqStr != "")
                        reqStr += " ";
                    reqStr += req.data[i].ToString("X2");
                }
                RequestByteString = reqStr;

                string rspStr = "";
                for (int i = 0; i < rsp.count; i++)
                {
                    if (rspStr != "")
                        rspStr += " ";
                    rspStr += rsp.data[i].ToString("X2");
                }
                ResponseByteString = rspStr;
            }
            else
            {
                RequestByteString = "Not Available";
                ResponseByteString = "Not Available";
            }

            return ret;
        }

        public int InvokeSimulation()
        {
            RequestByteString = "2E C0 57 01 00 00 00 00";
            ResponseByteString = "00 57 01 00";
            return InvokeInternalSimulation();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        // Create the OnPropertyChanged method to raise the event 
        protected void OnPropertyChanged(string name)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(name));
            }
        }

        public FlowDocument SignatureFancy
        {
            get
            {
                return Function.ParseSyntax(Signature);
            }
        }

        public static Run WordCallback(string word)
        {
            Run r = new Run(word);

            if (word == "typedef" || word == "struct" || word == "union")
            {
                r.Foreground = Brushes.ForestGreen;
            }
            else if (word == "NMPRKC_API" || word == "IN" || word == "OUT")
            {
                r.Foreground = Brushes.ForestGreen;
            }
            else if (word.EndsWith("_t") == true)
            {
                r.Foreground = Brushes.Blue;
            }

            return r;
        }

        public static Run CommentCallback(string comment)
        {
            Run r = new Run(comment);
            r.Foreground = Brushes.Gray;
            return r;
        }

        public delegate Run ParseSyntaxWordCallbackDelegate(string word);
        public static FlowDocument ParseSyntax(string str, ParseSyntaxWordCallbackDelegate customWordCallback = null)
        {
            Paragraph p = new Paragraph();
            Run r = null;

            ParseSyntaxWordCallbackDelegate wordCallback = customWordCallback;
            if (wordCallback == null)
                wordCallback = WordCallback;

            string word = string.Empty;
            string comment = string.Empty;
            foreach (char c in str)
            {
                Match m = Regex.Match(c.ToString(), @"\w");
                if (m.Success == true)
                {
                    word += c;
                }
                else
                {
                    // end of word or no word
                    if (word.Length > 0)
                    {
                        r = wordCallback(word);
                        p.Inlines.Add(r);

                        word = string.Empty;
                    }

#if false
                    if (c == '/' && comment == string.Empty)
                        comment = c.ToString();
                    else if (comment != string.Empty)
                    {
                        // we might be in a comment
                        if (comment == "/")
                        {
                            if (c == '*' || c == '/')
                                comment += c.ToString();
                        }
                        else if(comment.StartsWith("//") == true)
                        {
                            //if(c == '\n' || c == '\r')
                            //{
                            // commentCallback
                            // comment = string.Empty;
                            //}
                            //else
                            comment += c.ToString();
                        }
                        else if (comment.StartsWith("/*") == true)
                        {
                            if (comment.EndsWith("*") == true && c == '/')
                            {
                                //commentCallback
                                // comment = string.Empty;
                            }
                        }
                    }
#endif
                    if (c == '\r')
                    {
                        // ignore
                    }
                    else if (c == '\t')
                        p.Inlines.Add("    ");
                    else if (c == '\n')
                        p.Inlines.Add(new LineBreak());
                    else
                        p.Inlines.Add(c.ToString());
                }
            }

            if (word != string.Empty)
            {
                r = wordCallback(word);
                p.Inlines.Add(r);
                word = string.Empty;
            }

            FlowDocument fd = new FlowDocument();
            fd.Blocks.Add(p);
            return fd;
        }
    }

    #region Node Manager Basic
    public class EnableDisablePolicyControlFunction : Function
    {
        public override string Name { get { return "NMPRK_EnableDisablePolicyControl"; } }

        private EnableDisablePolicyControlFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_enable_disable_policy_control_input_t input = _input.ToStruct();
            int compCode = NMPRK.NMPRK_API.EnableDisablePolicyControl(handle, ref input);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public EnableDisablePolicyControlFunction()
        {
            _input = new EnableDisablePolicyControlFunctionInput(this);
        }
    }

    public class SetPolicyFunction : Function
    {
        public override string Name { get { return "NMPRK_SetPolicy"; } }

        private SetPolicyFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_set_policy_input_t input = _input.ToStruct();
            int compCode = NMPRK.NMPRK_API.SetPolicy(handle, ref input);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public SetPolicyFunction()
        {
            _input = new SetPolicyFunctionInput(this);
        }
    }

    public class GetPolicyFunction : Function
    {
        public override string Name { get { return "NMPRK_GetPolicy"; } }

        private GetPolicyFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private GetPolicyFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_get_policy_input_t input = _input.ToStruct();
            NMPRK.nm_get_policy_output_t output = new NMPRK.nm_get_policy_output_t();
            int compCode = NMPRK.NMPRK_API.GetPolicy(handle, ref input, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            NMPRK.nm_get_policy_output_t output = new NMPRK.nm_get_policy_output_t();
            output.standard = new NMPRK.nm_get_policy_output_standard_t();
            output.standard.info.domain = NMPRK.nm_domain_id_t.CPU;
            output.standard.info.policyEnabled = true;
            output.standard.info.policyTriggerType = NMPRK.nm_policy_trigger_type_t.INLET_TEMPERATURE_TRIGGER;
            output.standard.info.policyTriggerLimit = 25;
            output.standard.info.policyTargetLimit = 300;
            output.standard.info.correctionTimeLimit = 1000;
            output.standard.info.statisticsReportingPeriod = 60;
            _output.Update(0, output);
            return (ReturnValue = 0);
        }

        public GetPolicyFunction()
        {
            _input = new GetPolicyFunctionInput(this);
            _output = new GetPolicyFunctionOutput(this);
        }
    }

    public class SetPolicyAlertThresholdsFunction : Function
    {
        public override string Name { get { return "NMPRK_SetPolicyAlertThresholds"; } }

        private SetPolicyAlertThresholdsFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_set_policy_alert_thresholds_input_t input = _input.ToStruct();
            return (ReturnValue = NMPRK.NMPRK_API.SetPolicyAlertThresholds(handle, ref input));
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public SetPolicyAlertThresholdsFunction()
        {
            _input = new SetPolicyAlertThresholdsFunctionInput(this);
        }
    }

    public class GetPolicyAlertThresholdsFunction : Function
    {
        public override string Name { get { return "NMPRK_GetPolicyAlertThresholds"; } }

        private GetPolicyAlertThresholdsFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private GetPolicyAlertThresholdsFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_get_policy_alert_thresholds_input_t input = _input.ToStruct();
            NMPRK.nm_get_policy_alert_thresholds_output_t output = new NMPRK.nm_get_policy_alert_thresholds_output_t();
            int compCode = NMPRK.NMPRK_API.GetPolicyAlertThresholds(handle, ref input, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public GetPolicyAlertThresholdsFunction()
        {
            _input = new GetPolicyAlertThresholdsFunctionInput(this);
            _output = new GetPolicyAlertThresholdsFunctionOutput(this);
        }
    }

    public class SetPolicySuspendPeriodsFunction : Function
    {
        public override string Name { get { return "NMPRK_SetPolicySuspendPeriods"; } }

        private SetPolicySuspendPeriodsFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_set_policy_suspend_periods_input_t input = _input.ToStruct();
            return (ReturnValue = NMPRK.NMPRK_API.SetPolicySuspendPeriods(handle, ref input));
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public SetPolicySuspendPeriodsFunction()
        {
            _input = new SetPolicySuspendPeriodsFunctionInput(this);
        }
    }

    public class GetPolicySuspendPeriodsFunction : Function
    {
        public override string Name { get { return "NMPRK_GetPolicySuspendPeriods"; } }

        private GetPolicySuspendPeriodsFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private GetPolicySuspendPeriodsFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_get_policy_suspend_periods_input_t input = _input.ToStruct();
            NMPRK.nm_get_policy_suspend_periods_output_t output = new NMPRK.nm_get_policy_suspend_periods_output_t();
            output.suspendPeriods = new NMPRK.nm_policy_suspend_period_t[5];
            int compCode = NMPRK.NMPRK_API.GetPolicySuspendPeriods(handle, ref input, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public GetPolicySuspendPeriodsFunction()
        {
            _input = new GetPolicySuspendPeriodsFunctionInput(this);
            _output = new GetPolicySuspendPeriodsFunctionOutput(this);
        }
    }

    public class ResetStatisticsFunction : Function
    {
        public override string Name { get { return "NMPRK_ResetStatistics"; } }

        private ResetStatisticsFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_reset_statistics_input_t input = _input.ToStruct();
            return (ReturnValue = NMPRK.NMPRK_API.ResetStatistics(handle, ref input));
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public ResetStatisticsFunction()
        {
            _input = new ResetStatisticsFunctionInput(this);
        }
    }

    public class GetStatisticsFunction : Function
    {
        public override string Name { get { return "NMPRK_GetStatistics"; } }

        private GetStatisticsFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private GetStatisticsFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_get_statistics_input_t input = _input.ToStruct();
            NMPRK.nm_get_statistics_output_t output = new NMPRK.nm_get_statistics_output_t();
            int compCode = NMPRK.NMPRK_API.GetStatistics(handle, ref input, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            NMPRK.nm_get_statistics_output_t output = new NMPRK.nm_get_statistics_output_t();
            output = new NMPRK.nm_get_statistics_output_t();
            output.currentValue = (ushort)(95 + (DateTime.Now.Ticks % 50));
            output.minimumValue = 94;
            output.maximumValue = 155;
            output.averageValue = 123;
            output.timestamp = (uint)(DateTime.Now.Ticks % 0x0FFFFFFF);
            output.statisticsReportingPeriod = (uint)(DateTime.Now.Ticks % 0xFFFF); ;
            output.domain = _input.Domain.Value;
            output.policyGlobalAdministrativeState = false;
            output.policyOperationalState = true;
            output.measurementsState = false;
            output.policyActivationState = false;
            _output.Update(0, output);
            return (ReturnValue = 0);
        }

        public GetStatisticsFunction()
        {
            _input = new GetStatisticsFunctionInput(this);
            _output = new GetStatisticsFunctionOutput(this);
        }
    }

    public class GetCapabilitiesFunction : Function
    {
        public override string Name { get { return "NMPRK_GetCapabilities"; } }

        private GetCapabilitiesFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private GetCapabilitiesFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_get_capabilities_input_t input = _input.ToStruct();
            NMPRK.nm_get_capabilities_output_t output = new NMPRK.nm_get_capabilities_output_t();
            int compCode = NMPRK.NMPRK_API.GetCapabilities(handle, ref input, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public GetCapabilitiesFunction()
        {
            _input = new GetCapabilitiesFunctionInput(this);
            _output = new GetCapabilitiesFunctionOutput(this);
        }
    }

    public class GetVersionFunction : Function
    {
        public override string Name { get { return "NMPRK_GetVersion"; } }

        public override FunctionInput Input { get { return null; } }

        private GetVersionFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_get_version_output_t output = new NMPRK.nm_get_version_output_t();
            int compCode = NMPRK.NMPRK_API.GetVersion(handle, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public GetVersionFunction()
        {
            _output = new GetVersionFunctionOutput(this);
        }
    }

    public class SetPowerDrawRangeFunction : Function
    {
        public override string Name { get { return "NMPRK_SetPowerDrawRange"; } }
        //public override string Signature { get { return "NMPRKC_API nmprk_status_t\nNMPRK_SetPowerDrawRange(\n\tIN nmprk_conn_handle_t h,\n\tIN nm_set_power_draw_range_input_t *input);"; } }

        private SetPowerDrawRangeFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_set_power_draw_range_input_t input = _input.ToStruct();
            return (ReturnValue = NMPRK.NMPRK_API.SetPowerDrawRange(handle, ref input));
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public SetPowerDrawRangeFunction()
        {
            _input = new SetPowerDrawRangeFunctionInput(this);
        }
    }

    public class SetAlertDestinationFunction : Function
    {
        public override string Name { get { return "NMPRK_SetAlertDestination"; } }
        //public override string Signature { get { return "NMPRKC_API nmprk_status_t\nNMPRK_SetAlertDestination(\n\tIN nmprk_conn_handle_t h,\n\tIN nm_set_alert_destination_input_t *input);"; } }

        private SetAlertDestinationFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_set_alert_destination_input_t input = _input.ToStruct();
            return (ReturnValue = NMPRK.NMPRK_API.SetAlertDestination(handle, ref input));
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public SetAlertDestinationFunction()
        {
            _input = new SetAlertDestinationFunctionInput(this);
        }
    }

    public class GetAlertDestinationFunction : Function
    {
        public override string Name { get { return "NMPRK_GetAlertDestination"; } }

        public override FunctionInput Input { get { return null; } }

        private GetAlertDestinationFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_get_alert_destination_output_t output = new NMPRK.nm_get_alert_destination_output_t();
            int compCode = NMPRK.NMPRK_API.GetAlertDestination(handle, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public GetAlertDestinationFunction()
        {
            _output = new GetAlertDestinationFunctionOutput(this);
        }
    }
    #endregion

    #region PTU
    public class PlatformCharacterizationLaunchRequestFunction : Function
    {
        public override string Name { get { return "NMPRK_PlatformCharacterizationLaunchRequest"; } }

        private PlatformCharacterizationLaunchRequestFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_platform_characterization_launch_req_input_t input = _input.ToStruct();
            int compCode = NMPRK.NMPRK_API.PlatformCharacterizationLaunchRequest(handle, ref input);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public PlatformCharacterizationLaunchRequestFunction()
        {
            _input = new PlatformCharacterizationLaunchRequestFunctionInput(this);
        }
    }

    public class GetPowerCharacterizationRangeFunction : Function
    {
        public override string Name { get { return "NMPRK_GetPowerCharacterizationRange"; } }

        private GetPowerCharacterizationRangeFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private GetPowerCharacterizationRangeFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_get_power_characterization_range_input_t input = _input.ToStruct();
            NMPRK.nm_get_power_characterization_range_output_t output = new NMPRK.nm_get_power_characterization_range_output_t();
            int compCode = NMPRK.NMPRK_API.GetPowerCharacterizationRange(handle, ref input, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public GetPowerCharacterizationRangeFunction()
        {
            _input = new GetPowerCharacterizationRangeFunctionInput(this);
            _output = new GetPowerCharacterizationRangeFunctionOutput(this);
        }
    }
    #endregion

    #region CUPS
    public class GetCupsCapabilitiesFunction : Function
    {
        public override string Name { get { return "NMPRK_GetCupsCapabilities"; } }

        public override FunctionInput Input { get { return null; } }

        private GetCupsCapabiltiesFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_get_cups_capabilities_output_t output = new NMPRK.nm_get_cups_capabilities_output_t();
            int compCode = NMPRK.NMPRK_API.GetCupsCapabilities(handle, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public GetCupsCapabilitiesFunction()
        {
            _output = new GetCupsCapabiltiesFunctionOutput(this);
        }
    }

    public class GetCupsDataFunction : Function
    {
        public override string Name { get { return "NMPRK_GetCupsData"; } }

        private GetCupsDataFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private GetCupsDataFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_get_cups_data_input_t input = _input.ToStruct();
            NMPRK.nm_get_cups_data_output_t output = new NMPRK.nm_get_cups_data_output_t();
            int compCode = NMPRK.NMPRK_API.GetCupsData(handle, ref input, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public GetCupsDataFunction()
        {
            _input = new GetCupsDataFunctionInput(this);
            _output = new GetCupsDataFunctionOutput(this);
        }
    }

    public class SetCupsConfigurationFunction : Function
    {
        public override string Name { get { return "NMPRK_SetCupsConfiguration"; } }

        private SetCupsConfigurationFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_set_cups_configuration_input_t input = _input.ToStruct();
            int compCode = NMPRK.NMPRK_API.SetCupsConfiguration(handle, ref input);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public SetCupsConfigurationFunction()
        {
            _input = new SetCupsConfigurationFunctionInput(this);
        }
    }

    public class GetCupsConfigurationFunction : Function
    {
        public override string Name { get { return "NMPRK_GetCupsConfiguration"; } }

        public override FunctionInput Input { get { return null; } }

        private GetCupsConfigurationFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_get_cups_configuration_output_t output = new NMPRK.nm_get_cups_configuration_output_t();
            int compCode = NMPRK.NMPRK_API.GetCupsConfiguration(handle, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public GetCupsConfigurationFunction()
        {
            _output = new GetCupsConfigurationFunctionOutput(this);
        }
    }

    public class SetCupsPoliciesFunction : Function
    {
        public override string Name { get { return "NMPRK_SetCupsPolicies"; } }

        private SetCupsPoliciesFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_set_cups_policies_input_t input = _input.ToStruct();
            int compCode = NMPRK.NMPRK_API.SetCupsPolicies(handle, ref input);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public SetCupsPoliciesFunction()
        {
            _input = new SetCupsPoliciesFunctionInput(this);
        }
    }

    public class GetCupsPoliciesFunction : Function
    {
        public override string Name { get { return "NMPRK_GetCupsPolicies"; } }

        private GetCupsPoliciesFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private GetCupsPoliciesFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_get_cups_policies_input_t input = _input.ToStruct();
            NMPRK.nm_get_cups_policies_output_t output = new NMPRK.nm_get_cups_policies_output_t();
            int compCode = NMPRK.NMPRK_API.GetCupsPolicies(handle, ref input, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public GetCupsPoliciesFunction()
        {
            _input = new GetCupsPoliciesFunctionInput(this);
            _output = new GetCupsPoliciesFunctionOutput(this);
        }
    }
    #endregion

    #region Node Manager Advanced
    public class GetDiscoveryParametersFunction : Function
    {
        public override string Name { get { return "NMPRK_GetDiscoveryParameters"; } }

        public override FunctionInput Input { get { return null; } }

        private GetDiscoveryParametersFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_discovery_parameters_t output = new NMPRK.nm_discovery_parameters_t();
            int compCode = NMPRK.NMPRK_API.GetDiscoveryParameters(handle, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public GetDiscoveryParametersFunction()
        {
            _output = new GetDiscoveryParametersFunctionOutput(this);
        }
    }
    #endregion

    #region IPMI Functions
    public class IPMI_GetDeviceIdFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_GetDeviceId"; } }

        public override FunctionInput Input { get { return null; } }

        private IPMI_GetDeviceIdFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_ipmi_device_id_t output = new NMPRK.nm_ipmi_device_id_t();
            output.manufId = new byte[3];
            output.productId = new byte[2];
            int compCode = NMPRK.NMPRK_API.IPMI_GetDeviceId(handle, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_GetDeviceIdFunction()
        {
            _output = new IPMI_GetDeviceIdFunctionOutput(this);
        }
    }

    public class IPMI_GetSdrInfoFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_GetSdrInfo"; } }

        public override FunctionInput Input { get { return null; } }

        private IPMI_GetSdrInfoFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_ipmi_repo_info_t output = new NMPRK.nm_ipmi_repo_info_t();
            int compCode = NMPRK.NMPRK_API.IPMI_GetSdrInfo(handle, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_GetSdrInfoFunction()
        {
            _output = new IPMI_GetSdrInfoFunctionOutput(this);
        }
    }

    public class IPMI_GetSdrRecordFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_GetSdrRecord"; } }

        private IPMI_RecordIdFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private IPMI_GetRecordFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_ipmi_sdr_record_t output = new NMPRK.nm_ipmi_sdr_record_t();
            ushort nextRecord = 0;
            int compCode = NMPRK.NMPRK_API.IPMI_GetSdrRecord(handle, _input.ToValue(), ref nextRecord, ref output);
            _output.Update(compCode, nextRecord, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_GetSdrRecordFunction()
        {
            _input = new IPMI_RecordIdFunctionInput(this);
            _output = new IPMI_GetRecordFunctionOutput(this);
        }
    }

    public class IPMI_AddSdrRecordFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_AddSdrRecord"; } }

        private IPMI_AddSdrRecordFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private IPMI_AddSdrRecordFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        public override string FunctionWarningMsg
        {
            get { return "This function modifies the SDR repository. Please do not execute this function unless you know what you are doing."; }
        }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_ipmi_sdr_record_t input = _input.ToStruct();
            ushort output = 0;
            int compCode = NMPRK.NMPRK_API.IPMI_AddSdrRecord(handle, ref input, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_AddSdrRecordFunction()
        {
            _input = new IPMI_AddSdrRecordFunctionInput(this);
            _output = new IPMI_AddSdrRecordFunctionOutput(this);
        }
    }

    public class IPMI_DeleteSdrRecordFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_DeleteSdrRecord"; } }

        private IPMI_RecordIdFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        public override string FunctionWarningMsg
        {
            get { return "This function modifies the SDR repository. Please do not execute this function unless you know what you are doing."; }
        }

        protected override int InvokeInternal(int handle)
        {
            ushort input = _input.ToValue();
            int compCode = NMPRK.NMPRK_API.IPMI_DeleteSdrRecord(handle, input);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_DeleteSdrRecordFunction()
        {
            _input = new IPMI_RecordIdFunctionInput(this);
        }
    }

    public class IPMI_ClearSdrRepositoryFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_ClearSdrRepository"; } }

        public override FunctionInput Input { get { return null; } }

        public override FunctionOutput Output { get { return null; } }

        public override string FunctionProhibitedMsg
        {
            get { return "This function clears the SDR repository. It cannot be invoked from this tool."; }
        }

        protected override int InvokeInternal(int handle)
        {
            return (ReturnValue = 0);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }
    }

    public class IPMI_GetSelInfoFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_GetSelInfo"; } }

        public override FunctionInput Input { get { return null; } }

        private IPMI_GetSelInfoFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_ipmi_repo_info_t output = new NMPRK.nm_ipmi_repo_info_t();
            int compCode = NMPRK.NMPRK_API.IPMI_GetSelInfo(handle, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_GetSelInfoFunction()
        {
            _output = new IPMI_GetSelInfoFunctionOutput(this);
        }
    }

    public class IPMI_GetSelEntryFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_GetSelEntry"; } }

        private IPMI_RecordIdFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private IPMI_GetEntryFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_ipmi_sel_entry_t output = new NMPRK.nm_ipmi_sel_entry_t();
            ushort nextEntry = 0;
            int compCode = NMPRK.NMPRK_API.IPMI_GetSelEntry(handle, _input.ToValue(), ref nextEntry, ref output);
            _output.Update(compCode, nextEntry, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_GetSelEntryFunction()
        {
            _input = new IPMI_RecordIdFunctionInput(this);
            _output = new IPMI_GetEntryFunctionOutput(this);
        }
    }

    public class IPMI_DeleteSelEntryFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_DeleteSelEntry"; } }

        private IPMI_RecordIdFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            ushort input = _input.ToValue();
            int compCode = NMPRK.NMPRK_API.IPMI_DeleteSelEntry(handle, input);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_DeleteSelEntryFunction()
        {
            _input = new IPMI_RecordIdFunctionInput(this);
        }
    }

    public class IPMI_ClearSelFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_ClearSel"; } }

        public override FunctionInput Input { get { return null; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            int compCode = NMPRK.NMPRK_API.IPMI_ClearSel(handle);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }
    }

    public class IPMI_GetFruInfoFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_GetFruInfo"; } }

        public override FunctionInput Input { get { return null; } }

        private IPMI_GetFruInfoFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.nm_ipmi_fru_info_t output = new NMPRK.nm_ipmi_fru_info_t();
            int compCode = NMPRK.NMPRK_API.IPMI_GetFruInfo(handle, ref output);
            _output.Update(compCode, output);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_GetFruInfoFunction()
        {
            _output = new IPMI_GetFruInfoFunctionOutput(this);
        }
    }

    public class IPMI_ReadFruDataFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_ReadFruData"; } }

        private IPMI_ReadFruDataFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private IPMI_ReadFruDataFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            int length = (int)_input.Length.Value;
            byte[] data = new byte[length];
            int compCode = NMPRK.NMPRK_API.IPMI_ReadFruData(handle, (int)_input.Offset.Value, ref length, data);
            _output.Update(compCode, length, data);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_ReadFruDataFunction()
        {
            _input = new IPMI_ReadFruDataFunctionInput(this);
            _output = new IPMI_ReadFruDataFunctionOutput(this);
        }
    }

    public class IPMI_WriteFruDataFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_WriteFruData"; } }

        private IPMI_WriteFruDataFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        private IPMI_WriteFruDataFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        public override string FunctionWarningMsg
        {
            get { return "This function modifies the FRU. Please do not execute this function unless you know what you are doing."; }
        }

        protected override int InvokeInternal(int handle)
        {
            int length = (int)_input.Length.Value;
            int compCode = NMPRK.NMPRK_API.IPMI_WriteFruData(handle, (int)_input.Offset.Value, ref length, _input.Data.Value);
            _output.Update(compCode, length);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_WriteFruDataFunction()
        {
            _input = new IPMI_WriteFruDataFunctionInput(this);
            _output = new IPMI_WriteFruDataFunctionOutput(this);
        }
    }

    public class IPMI_GetAcpiPowerStateFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_GetAcpiPowerState"; } }

        public override FunctionInput Input { get { return null; } }

        private IPMI_GetAcpiPowerStateFunctionOutput _output = null;
        public override FunctionOutput Output { get { return _output; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.ipmi_acpi_power_state_t pwrState = new NMPRK.ipmi_acpi_power_state_t();
            int compCode = NMPRK.NMPRK_API.IPMI_GetAcpiPowerState(handle, ref pwrState);
            _output.Update(compCode, pwrState);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_GetAcpiPowerStateFunction()
        {
            _output = new IPMI_GetAcpiPowerStateFunctionOutput(this);
        }
    }

    public class IPMI_SetAcpiPowerStateFunction : Function
    {
        public override string Name { get { return "NMPRK_IPMI_SetAcpiPowerState"; } }

        private IPMI_SetAcpiPowerStateFunctionInput _input = null;
        public override FunctionInput Input { get { return _input; } }

        public override FunctionOutput Output { get { return null; } }

        protected override int InvokeInternal(int handle)
        {
            NMPRK.ipmi_acpi_power_state_t pwrState = new NMPRK.ipmi_acpi_power_state_t();
            pwrState.setSystemState = _input.SetSystemPowerState.Value;
            pwrState.setDeviceState = _input.SetDevicePowerState.Value;
            pwrState.systemState = _input.SystemPowerState.Value;
            pwrState.deviceState = _input.DevicePowerState.Value;
            int compCode = NMPRK.NMPRK_API.IPMI_SetAcpiPowerState(handle, ref pwrState);
            return (ReturnValue = compCode);
        }

        protected override int InvokeInternalSimulation()
        {
            return (ReturnValue = 0);
        }

        public IPMI_SetAcpiPowerStateFunction()
        {
            _input = new IPMI_SetAcpiPowerStateFunctionInput(this);
        }
    }
    #endregion
}
