using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using System.Globalization;
using System.Diagnostics;
using System.Reflection;
using System.IO;
using System.Net;
using IntelShared.Common;

namespace NMPRK_GUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Thread _executeFunctionThread = null;

        private const string TITLE = "NMPRK - Node Manager Programmers Reference Kit";

        public MainWindow()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(MainWindow_Loaded);
            Title = TITLE;

            ErrorLog.WriteLine("Application Started: " + DateTime.Now.ToLongDateString() + " " + DateTime.Now.ToLongTimeString());

            try
            {
                NMPRKReader.ReadFunctionsHeaderFile();
                NMPRKReader.ReadTypesHeaderFile();
                NMPRKReader.GetFunctionInputOutput("NMPRK_GetPolicy");
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, "Error Reading Header Files. Please reinstall or contact technical support", "NMPRK Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            EventManager.RegisterClassHandler(typeof(SelectAllTextBox), TextBox.GotFocusEvent, new RoutedEventHandler(TextBox_SelectAllText));
            EventManager.RegisterClassHandler(typeof(SelectAllTextBox), TextBox.PreviewMouseDownEvent, new MouseButtonEventHandler(TextBox_SelectivelyIgnoreMouseButton));
            EventManager.RegisterClassHandler(typeof(PasswordBox), PasswordBox.GotFocusEvent, new RoutedEventHandler(PasswordBox_SelectAllText));
            EventManager.RegisterClassHandler(typeof(PasswordBox), PasswordBox.PreviewMouseDownEvent, new MouseButtonEventHandler(PasswordBox_SelectivelyIgnoreMouseButton));

            UpdateFunctionsList();
            cbSelectFunction.SelectedIndex = 0;

            tabControl1.SelectedIndex = 3; // connection tab

            if (Properties.Settings.Default.ConnectionMode == "Remote")
                rbRemote.IsChecked = true;
            else
                rbLocal.IsChecked = true;

            try
            {
                tbIpAddress.Text = Properties.Settings.Default.IpAddress;
            }
            catch { }
            tbUsername.Text = Properties.Settings.Default.Username;
            tbPassword.Password = Properties.Settings.Default.Password;
            cbUseBridgingInformation.IsChecked = Properties.Settings.Default.ManuallyEnterBridgingParameters;

            for(byte i = 0; i < byte.MaxValue; i++)
            {
                cbNmChannel.Items.Add("0x" + i.ToString("X2"));
                cbNmAddress.Items.Add("0x" + i.ToString("X2"));
            }
            cbNmChannel.SelectedIndex = cbNmAddress.SelectedIndex = 0;
            if(Properties.Settings.Default.ManualNmChannel != "")
                cbNmChannel.SelectedItem = Properties.Settings.Default.ManualNmChannel;
            if (Properties.Settings.Default.ManualNmAddress != "")
                cbNmAddress.SelectedItem = Properties.Settings.Default.ManualNmAddress;

            UpdateConnectionStatus("Not Connected");

            statusTimer = new System.Threading.Timer(StatusTimerEvent, null, 0, 0);

            //_fakeConnected = true;
            _executeFunctionThread = new Thread(ExecuteFunctionThread);
            _executeFunctionThread.Start();
        }

        void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            // If I call this in the constructor I get some RSC warning from the debugger
            try
            {
                NMPRK.NMPRK_API.StartDebugLogging("nmprkGui.log");
            }
            catch (Exception ex)
            {
                ErrorLog.LogException(ex);
            }
        }

        private List<Function> _functionsList = new List<Function>();
        private delegate void UpdateFunctionsListDelegate();
        private void UpdateFunctionsList()
        {
            if (this.Dispatcher.CheckAccess())
            {
                _functionsList.Clear();

                if (currentConnectionNmVersion >= 0x02)
                {
                    _functionsList.Add(new EnableDisablePolicyControlFunction());
                    _functionsList.Add(new SetPolicyFunction());
                    _functionsList.Add(new GetPolicyFunction());
                    _functionsList.Add(new SetPolicyAlertThresholdsFunction());
                    _functionsList.Add(new GetPolicyAlertThresholdsFunction());
                    _functionsList.Add(new SetPolicySuspendPeriodsFunction());
                    _functionsList.Add(new GetPolicySuspendPeriodsFunction());
                    _functionsList.Add(new ResetStatisticsFunction());
                    _functionsList.Add(new GetStatisticsFunction());
                    _functionsList.Add(new GetCapabilitiesFunction());
                    _functionsList.Add(new GetVersionFunction());
                    _functionsList.Add(new SetPowerDrawRangeFunction());
                    _functionsList.Add(new SetAlertDestinationFunction());
                    _functionsList.Add(new GetAlertDestinationFunction());
                    _functionsList.Add(new GetDiscoveryParametersFunction());
                }

                if (currentConnectionNmVersion >= 0x05)
                {
                    _functionsList.Add(new PlatformCharacterizationLaunchRequestFunction());
                    _functionsList.Add(new GetPowerCharacterizationRangeFunction());
                    _functionsList.Add(new GetCupsCapabilitiesFunction());
                    _functionsList.Add(new GetCupsDataFunction());
                    _functionsList.Add(new SetCupsConfigurationFunction());
                    _functionsList.Add(new GetCupsConfigurationFunction());
                    _functionsList.Add(new GetCupsCapabilitiesFunction());
                    _functionsList.Add(new SetCupsPoliciesFunction());
                    _functionsList.Add(new GetCupsPoliciesFunction());
                }

                _functionsList.Add(new IPMI_GetDeviceIdFunction());
                _functionsList.Add(new IPMI_GetSdrInfoFunction());
                _functionsList.Add(new IPMI_GetSdrRecordFunction());
                _functionsList.Add(new IPMI_AddSdrRecordFunction());
                _functionsList.Add(new IPMI_DeleteSdrRecordFunction());
                _functionsList.Add(new IPMI_ClearSdrRepositoryFunction());
                _functionsList.Add(new IPMI_GetSelInfoFunction());
                _functionsList.Add(new IPMI_GetSelEntryFunction());
                _functionsList.Add(new IPMI_DeleteSelEntryFunction());
                _functionsList.Add(new IPMI_ClearSelFunction());
                _functionsList.Add(new IPMI_GetFruInfoFunction());
                _functionsList.Add(new IPMI_ReadFruDataFunction());
                _functionsList.Add(new IPMI_WriteFruDataFunction());
                _functionsList.Add(new IPMI_GetAcpiPowerStateFunction());
                _functionsList.Add(new IPMI_SetAcpiPowerStateFunction());

                cbSelectFunction.Items.Clear();
                foreach (Function f in _functionsList)
                {
                    cbSelectFunction.Items.Add(f);
                }
                cbSelectFunction.SelectedIndex = 0;
            }
            else
            {
                this.Dispatcher.Invoke(
                           System.Windows.Threading.DispatcherPriority.Normal,
                           new UpdateFunctionsListDelegate(UpdateFunctionsList));
            }
        }

        #region Text/Password Box Events
        private void TextBox_SelectAllText(object sender, RoutedEventArgs e)
        {
            ((TextBox)sender).SelectAll();
        }

        private void PasswordBox_SelectAllText(object sender, RoutedEventArgs e)
        {
            ((PasswordBox)sender).SelectAll();
        }

        /// <summary>
        /// Handles PreviewMouseDown Event.  Selects all on Triple click.  
        /// If SelectAllOnEnter is true, when the textbox is clicked and doesn't already have keyboard focus, selects all
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void TextBox_SelectivelyIgnoreMouseButton(object sender, MouseButtonEventArgs e)
        {
            // If its a triple click, select all text for the user.
            if (e.ClickCount == 3)
            {
                TextBox_SelectAllText(sender, new RoutedEventArgs());
                return;
            }

            // Find the TextBox
            DependencyObject parent = e.OriginalSource as UIElement;
            while (parent != null && !(parent is TextBox))
            {
                parent = System.Windows.Media.VisualTreeHelper.GetParent(parent);
            }

            if (parent != null)
            {
                if (parent is TextBox)
                {
                    var textBox = (TextBox)parent;
                    if (!textBox.IsKeyboardFocusWithin)
                    {
                        // If the text box is not yet focussed, give it the focus and
                        // stop further processing of this click event.
                        textBox.Focus();
                        e.Handled = true;
                    }
                }
            }
        }

        private void PasswordBox_SelectivelyIgnoreMouseButton(object sender, MouseButtonEventArgs e)
        {
            // If its a triple click, select all text for the user.
            if (e.ClickCount == 3)
            {
                PasswordBox_SelectAllText(sender, new RoutedEventArgs());
                return;
            }

            // Find the TextBox
            DependencyObject parent = e.OriginalSource as UIElement;
            while (parent != null && !(parent is TextBox))
            {
                parent = System.Windows.Media.VisualTreeHelper.GetParent(parent);
            }

            if (parent != null)
            {
                if (parent is TextBox)
                {
                    var textBox = (TextBox)parent;
                    if (!textBox.IsKeyboardFocusWithin)
                    {
                        // If the text box is not yet focussed, give it the focus and
                        // stop further processing of this click event.
                        textBox.Focus();
                        e.Handled = true;
                    }
                }
            }
        }
        #endregion

        private int _noOfErrorsOnScreen = 0;
        private void tbValue_Error(object sender, ValidationErrorEventArgs e)
        {
            if (e.Action == ValidationErrorEventAction.Added)
                _noOfErrorsOnScreen++;
            else
                _noOfErrorsOnScreen--;
        }

        private void btnInvoke_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = ((_noOfErrorsOnScreen == 0) && (_connected == true || _fakeConnected == true));
            e.Handled = true;
        }

        private void btnInvoke_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            Function f = cbSelectFunction.SelectedItem as Function;
            if (f == null)
                return;

            grid1.IsEnabled = false;

            if (_fakeConnected == true)
            {
                f.InvokeSimulation();
                grid1.IsEnabled = true;
            }
            else
            {
                if (f.FunctionProhibitedMsg != null)
                {
                    MessageBox.Show(this, f.FunctionProhibitedMsg, "Function Prohibited", MessageBoxButton.OK, MessageBoxImage.Exclamation);
                    grid1.IsEnabled = true;
                }
                else
                {
                    MessageBoxResult result = MessageBoxResult.OK;
                    if (f.FunctionWarningMsg != null)
                    {
                        result = MessageBox.Show(this, f.FunctionWarningMsg + "\n\nClick OK to proceed, Cancel to abort", "Function Warning", MessageBoxButton.OKCancel, MessageBoxImage.Warning, MessageBoxResult.Cancel);
                    }

                    if (result == MessageBoxResult.OK)
                        AddFunctionToExecute(f);
                    else
                        grid1.IsEnabled = true;
                }
            }
        }

        private Queue<Function> _executeFunctionThreadQueue = new Queue<Function>();
        private object _executeFunctionThreadQueueLock = new object();
        private AutoResetEvent _executeFunctionThreadQueueEvent = new AutoResetEvent(false);
        private ManualResetEvent _executeFunctionThreadExit = new ManualResetEvent(false);

        private void AddFunctionToExecute(Function f)
        {
            lock (_executeFunctionThreadQueueLock)
            {
                _executeFunctionThreadQueue.Enqueue(f);
                _executeFunctionThreadQueueEvent.Set();
            }
        }

        private void ExecuteFunctionThread()
        {
            while (true)
            {
                try
                {
                    WaitHandle[] handles = new WaitHandle[] { _executeFunctionThreadExit, _executeFunctionThreadQueueEvent };
                    int signal = WaitHandle.WaitAny(handles, 30000);
                    if (signal == 0)
                    {
                        Console.WriteLine("Here");
                        return;
                    }
                    else if (signal == 1)
                    {
                        Function f = null;

                        lock (_executeFunctionThreadQueueLock)
                        {
                            f = _executeFunctionThreadQueue.Dequeue();
                        }

                        ErrorLog.WriteLine("Invoking function " + f.Name + "...");
                        try
                        {
                            f.Invoke(_connectionHandle);
                            ErrorLog.WriteLine("Request Bytes: " + f.RequestByteString);
                            ErrorLog.WriteLine("Response Bytes: " + f.ResponseByteString);
                            ErrorLog.WriteLine("Return Value: " + f.ReturnValueString);
                        }
                        catch (Exception ex)
                        {
                            f.ReturnValueString = "Exception Occurred - " + ex.Message;
                            ErrorLog.WriteLine("Return Value: " + f.ReturnValueString);
                        }
                        finally
                        {
                            FunctionInvokeComplete(f);
                        }
                    }
                    else if (signal == WaitHandle.WaitTimeout)
                    {
                        // Send GetVersion function every 30 seconds to keep connection open
                        GetVersionFunction gvf = new GetVersionFunction();
                        gvf.Invoke(_connectionHandle);
                    }
                    else
                    {
                        // TODO: ERROR
                    }
                }
                catch (Exception ex)
                {
                    ErrorLog.LogException(ex);
                }
            }
        }

        private void FunctionInvokeComplete(Function f)
        {
            this.Dispatcher.BeginInvoke(
                new Action(delegate
                {
                    listBox2.Items.Refresh();
                    grid1.IsEnabled = true;
                }));
        }

        private void cbSelectFunction_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count > 0)
            {
                Function f = e.AddedItems[0] as Function;

                _noOfErrorsOnScreen = 0;
                DataContext = f;
                textBox1.Document = f.SignatureFancy;
                if (f.Input != null)
                    textBox2.Document = f.Input.StructureFancy;
                else
                    textBox2.Document = new FlowDocument(new Paragraph(new Run("None")));

                if (f.Output != null)
                    textBox5.Document = f.Output.StructureFancy;
                else
                    textBox5.Document = new FlowDocument(new Paragraph(new Run("None")));
            }
        }

        private bool _fakeConnected = false;
        private bool _connected = false;
        private int _connectionHandle = -1;

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            SaveSettings();

            if (_executeFunctionThread != null)
            {
                _executeFunctionThreadExit.Set();
                _executeFunctionThread.Join(1000);
            }

            if (_connected == true)
                NMPRK.NMPRK_API.Disconnect(_connectionHandle);

            NMPRK.NMPRK_API.StopDebugLogging();

            ErrorLog.WriteLine("Application Closing: " + DateTime.Now.ToLongDateString() + " " + DateTime.Now.ToLongTimeString());
        }

        private void tbValue_Initialized(object sender, EventArgs e)
        {
            TextBox tb = sender as TextBox;
            if (tb.DataContext is NMPRK_Types.NumberType)
            {
                Binding b = BindingOperations.GetBinding(tb, TextBox.TextProperty);
                b.ValidationRules.Add((tb.DataContext as NMPRK_Types.NumberType).ValidationRule);
            }
        }

        private void miGuiHelp_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Process.Start(@"doc\NMPRK_GUI-Help.chm");
            }
            catch (Exception ex)
            {
                ErrorLog.LogException(ex);
            }
        }

        private void miApiHelp_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Process.Start(@"doc\NMPRK_API.chm");
            }
            catch (Exception ex)
            {
                ErrorLog.LogException(ex);
            }
        }

        private void miNodeManagerSpec_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Process.Start(@"https://www-ssl.intel.com/content/www/us/en/power-management/intelligent-power-node-manager-specification.html?");
            }
            catch (Exception ex)
            {
                ErrorLog.LogException(ex);
            }
        }

        private void miIpmiSpec_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Process.Start(@"https://www-ssl.intel.com/content/www/us/en/servers/ipmi/second-gen-interface-spec-v2.html");
            }
            catch (Exception ex)
            {
                ErrorLog.LogException(ex);
            }
        }

        private void miAbout_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                AboutWindow ab = new AboutWindow(this);
                ab.ShowDialog();
            }
            catch (Exception ex)
            {
                ErrorLog.LogException(ex);
            }
        }

        private void miExit_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void miDebugViewLog_Click(object sender, RoutedEventArgs e)
        {
            Process.Start(ErrorLog.LogFilePath);
        }

        private void miDebugSaveLog_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.SaveFileDialog sfd = new Microsoft.Win32.SaveFileDialog();
            sfd.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
            sfd.FileName = System.IO.Path.GetFileName(ErrorLog.LogFilePath);

            Nullable<bool> result = sfd.ShowDialog();
            if (result == true)
            {
                File.Copy(ErrorLog.LogFilePath, sfd.FileName, true);
            }
        }

        #region Connection
        private void btnConnect_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            try
            {
                if (rbRemote.IsChecked == true)
                {
                    IPAddress addr = IPAddress.Parse(tbIpAddress.Text);
                    if (tbUsername.Text.Length == 0)
                        e.CanExecute = false;
                    else
                        e.CanExecute = true;
                }
                else
                {
                    e.CanExecute = true;
                }
            }
            catch
            {
                e.CanExecute = false;
            }
            finally
            {
                e.Handled = true;
            }
        }

        private Thread _connectThread = null;

        private void btnConnect_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            try
            {
                btnConnect.IsEnabled = false;
                gbConnectionType.IsEnabled = false;
                gbBmcInformation.IsEnabled = false;
                gbUseBridgingInformation.IsEnabled = false;

                if (_connected == false)
                {
                    SaveSettings();

                    _connectionHandle = -1;
                    _connected = false;
                    lbConnectionInfo.Items.Clear();

                    ConnectInfo ci = new ConnectInfo();
                    ci.Remote = rbRemote.IsChecked.Value;
                    ci.Ip = tbIpAddress.Text;
                    ci.User = tbUsername.Text;
                    ci.Pass = tbPassword.Password;
                    ci.UseManualBridgingInfo = cbUseBridgingInformation.IsChecked.Value;
                    ci.Channel = Convert.ToByte((cbNmChannel.SelectedItem as string).Substring(2), 16);
                    ci.Address = Convert.ToByte((cbNmAddress.SelectedItem as string).Substring(2), 16);

                    //circularProgressBar1.Visibility = System.Windows.Visibility.Visible;

                    _connectThread = new Thread(ConnectThread);
                    _connectThread.Start(ci);
                }
                else
                {
                    lbConnectionInfo.Items.Clear();

                    _connectThread = new Thread(DisconnectThread);
                    _connectThread.Start();
                }
            }
            catch (Exception ex)
            {
                DisplayNmprkError("Connect", -1, ex.Message);
                ErrorLog.LogException(ex);
            }
        }

        private class ConnectInfo
        {
            public bool Remote = false;
            public string Ip;
            public string User;
            public string Pass;
            public bool UseManualBridgingInfo = false;
            public byte Channel;
            public byte Address;
        }

        private byte currentConnectionNmVersion = 0xFF;

        private void ConnectThread(object data)
        {
            try
            {
                ConnectInfo ci = data as ConnectInfo;

                int handle = -1;
                int status = -1;

                UpdateConnectionStatus("Connecting...");
                if (ci.Remote == true)
                {
                    ErrorLog.WriteLine(string.Format("Connecting to Remote Host ({0}, {1}, {2})...", ci.Ip, ci.User, ci.Pass));
                    NMPRK.nmprk_connect_remote_parameters_t input = new NMPRK.nmprk_connect_remote_parameters_t();
                    input.ipOrHostname = ci.Ip;
                    input.username = ci.User;
                    input.password = ci.Pass;
                    status = NMPRK.NMPRK_API.ConnectRemote(ref input, ref handle);
                }
                else
                {
                    ErrorLog.WriteLine("Connecting to Local Host...");
                    status = NMPRK.NMPRK_API.ConnectLocal(ref handle);
                }

                ErrorLog.WriteLine("Connect Status: " + status);
                if (status == 0)
                {
                    byte channel = 0;
                    byte address = 0;

                    if (ci.UseManualBridgingInfo == true)
                    {
                        channel = ci.Channel;
                        address = ci.Address;
                        AddConnectionInfoItem("Bridging Channel", "0x" + channel.ToString("X2"));
                        AddConnectionInfoItem("Bridging Address", "0x" + address.ToString("X2"));
                    }
                    else
                    {
                        ErrorLog.WriteLine("Searching for NM Discovery Parameters...");
                        UpdateConnectionStatus("Searching for NM Discovery Parameters...");
                        NMPRK.nm_discovery_parameters_t parameters = new NMPRK.nm_discovery_parameters_t();
                        status = NMPRK.NMPRK_API.GetDiscoveryParameters(handle, ref parameters);
                        if (status != NMPRK.NMPRK_API.NMPRK_SUCCESS)
                        {
                            DisplayNmprkError("GetDiscoveryParameters", status, "The server may not support Node Manager or has not implemented the 'Node Manager OEM SDR'");
                            NMPRK.NMPRK_API.Disconnect(handle);
                            ConnectThreadComplete(false);
                            return;
                        }

                        channel = parameters.channel;
                        address = parameters.address;

                        AddConnectionInfoItem("Bridging Channel", "0x" + channel.ToString("X2"));
                        AddConnectionInfoItem("Bridging Address", "0x" + address.ToString("X2"));
                        AddConnectionInfoItem("Health Event Sensor", "0x" + parameters.nmHealthEvSensor.ToString("X2"));
                        AddConnectionInfoItem("Exception Event Sensor", "0x" + parameters.nmExceptionEvSensor.ToString("X2"));
                        AddConnectionInfoItem("Operational Capabilities Sensor", "0x" + parameters.nmOperationalCapSensor.ToString("X2"));
                        AddConnectionInfoItem("Alert Threshold Exceeded Sensor", "0x" + parameters.nmAlertThresExcdSensor.ToString("X2"));
                    }

                    ErrorLog.WriteLine("NM Channel: 0x" + channel.ToString("X2"));
                    ErrorLog.WriteLine("NM Address: 0x" + address.ToString("X2"));
                    status = NMPRK.NMPRK_API.SetDefaultNmCommandBridging(handle, channel, address);
                    if (status != NMPRK.NMPRK_API.NMPRK_SUCCESS)
                    {
                        DisplayNmprkError("SetDefaultNmCommandBridging", status);
                        NMPRK.NMPRK_API.Disconnect(handle);
                        ConnectThreadComplete(false);
                        return;
                    }

                    NMPRK.nm_get_version_output_t versionOutput = new NMPRK.nm_get_version_output_t();
                    status = NMPRK.NMPRK_API.GetVersion(handle, ref versionOutput);
                    if (status == NMPRK.NMPRK_API.NMPRK_SUCCESS)
                    {
                        currentConnectionNmVersion = versionOutput.version;
                        AddConnectionInfoItem("IPMI Version", "0x" + versionOutput.ipmiVersion.ToString("X2"));
                        AddConnectionInfoItem("Node Manager Version", "0x" + versionOutput.version.ToString("X2"));
                        AddConnectionInfoItem("Major Firmware Revision", "0x" + versionOutput.majorFirmwareRevision.ToString("X2"));
                        AddConnectionInfoItem("Minor Firmware Revision", "0x" + versionOutput.minorFirmwareRevision.ToString("X2"));
                    }

                    _connectionHandle = handle;
                    _connected = true;
                    ConnectThreadComplete(true);
                }
                else
                {
                    DisplayNmprkError("Connect", status);
                    NMPRK.NMPRK_API.Disconnect(handle);
                    ConnectThreadComplete(false);
                }
            }
            catch (Exception ex)
            {
                ErrorLog.LogException(ex);
            }
        }

        private void DisconnectThread()
        {
            try
            {
                NMPRK.NMPRK_API.Disconnect(_connectionHandle);
                _connectionHandle = -1;
                _connected = false;
                currentConnectionNmVersion = 0xFF;
                DisconnectThreadComplete();
            }
            catch (Exception ex)
            {
                ErrorLog.LogException(ex);
            }
        }

        private delegate void ConnectThreadCompleteDelegate(bool success);
        private void ConnectThreadComplete(bool success)
        {
            if (this.Dispatcher.CheckAccess())
            {
                try
                {
                    if (success == true)
                    {
                        btnConnect.Content = "Disconnect";
                        UpdateConnectionStatus("Connected");
                        UpdateFunctionsList();
                    }
                    else
                    {
                        gbConnectionType.IsEnabled = true;
                        gbBmcInformation.IsEnabled = true;
                        gbUseBridgingInformation.IsEnabled = true;
                        _connectionHandle = -1;
                        _connected = false;
                        UpdateConnectionStatus("Not Connected");
                    }

                    
                    //circularProgressBar1.Visibility = System.Windows.Visibility.Hidden;
                    btnConnect.IsEnabled = true;
                }
                catch (Exception ex)
                {
                    ErrorLog.LogException(ex);
                }
            }
            else
            {
                this.Dispatcher.Invoke(
                        System.Windows.Threading.DispatcherPriority.Normal,
                        new ConnectThreadCompleteDelegate(ConnectThreadComplete), success);
            }
        }

        private delegate void DisconnectThreadCompleteDelegate();
        private void DisconnectThreadComplete()
        {
            if (this.Dispatcher.CheckAccess())
            {
                UpdateConnectionStatus("Not Connected");
                btnConnect.Content = "Connect";
                btnConnect.IsEnabled = true;
                gbConnectionType.IsEnabled = true;
                gbBmcInformation.IsEnabled = true;
                gbUseBridgingInformation.IsEnabled = true;
            }
            else
            {
                this.Dispatcher.Invoke(
                        System.Windows.Threading.DispatcherPriority.Normal,
                        new DisconnectThreadCompleteDelegate(DisconnectThreadComplete));
            }
        }

        private delegate void UpdateConnectionStatusDelegate(string status);
        private void UpdateConnectionStatus(string status)
        {
            if (this.Dispatcher.CheckAccess())
            {
                lblStatus.Content = status;
            }
            else
            {
                this.Dispatcher.Invoke(
                        System.Windows.Threading.DispatcherPriority.Normal,
                        new UpdateConnectionStatusDelegate(UpdateConnectionStatus), status);
            }
        }

        private delegate void AddConnectionInfoItemDelegate(string name, string value);
        private void AddConnectionInfoItem(string name, string value)
        {
            if (this.Dispatcher.CheckAccess())
            {
                lbConnectionInfo.Items.Add(new NMPRK_Types.StringType(name, value));
            }
            else
            {
                this.Dispatcher.Invoke(
                        System.Windows.Threading.DispatcherPriority.Normal,
                        new AddConnectionInfoItemDelegate(AddConnectionInfoItem), name, value);
            }
        }

        private delegate void DisplayNmprkErrorDelegate(string function, int status, string explanation);
        private void DisplayNmprkError(string function, int status, string explanation = "")
        {
            if (this.Dispatcher.CheckAccess())
            {
                ErrorLog.WriteLine("DisplayNmprkError: " + function + ", " + status + ", " + explanation);
                string msg = "";
                if (explanation != "")
                    msg = explanation + "\n\nDetails: ";
                msg += (function + " Failed! Error Code: 0x" + status.ToString("X2") + " - " + NMPRK.NMPRK_API.GetErrorString(status));
                MessageBox.Show(this, msg, "NMPRK Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
            else
            {
                this.Dispatcher.Invoke(
                        System.Windows.Threading.DispatcherPriority.Normal,
                        new DisplayNmprkErrorDelegate(DisplayNmprkError), function, status, explanation);
            }
        }

        private void SaveSettings()
        {
            Properties.Settings.Default.IpAddress = tbIpAddress.Text;
            Properties.Settings.Default.Username = tbUsername.Text;
            Properties.Settings.Default.Password = tbPassword.Password;
            if (rbLocal.IsChecked != null && rbLocal.IsChecked.HasValue == true && rbLocal.IsChecked.Value == true)
                Properties.Settings.Default.ConnectionMode = "Local";
            else if (rbRemote.IsChecked != null && rbRemote.IsChecked.HasValue == true && rbRemote.IsChecked.Value == true)
                Properties.Settings.Default.ConnectionMode = "Remote";
            Properties.Settings.Default.ManuallyEnterBridgingParameters = cbUseBridgingInformation.IsChecked.Value;
            Properties.Settings.Default.ManualNmChannel = cbNmChannel.SelectedItem as string;
            Properties.Settings.Default.ManualNmAddress = cbNmAddress.SelectedItem as string;
            Properties.Settings.Default.Save();
        }
        #endregion

        #region Timer
        private static System.Threading.Timer statusTimer;

        private void StatusTimerEvent(object aState)
        {
            try
            {
                UpdateTimeDate();
                statusTimer.Change(100, 0);
            }
            catch { }
        }

        private delegate void UpdateTimeDateDelegate();
        private void UpdateTimeDate()
        {
            try
            {
                if (this.Dispatcher.CheckAccess())
                {
                    lblDateTime.Content = DateTime.Now.ToString("MM/dd/yyyy") + " " + DateTime.Now.ToShortTimeString();
                }
                else
                {
                    this.Dispatcher.Invoke(
                        System.Windows.Threading.DispatcherPriority.Normal,
                        new UpdateTimeDateDelegate(UpdateTimeDate));
                }
            }
            catch { }
        }
        #endregion
    }

    public static class Command
    {
        public static readonly RoutedUICommand Invoke = new RoutedUICommand("Invoke", "Invoke", typeof(MainWindow));
    }
}

namespace IntelShared
{
    public class ProjectInfo
    {
        public static string CommonAppDataPath
        {
            get
            {
                string commonAppData = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData);
                string company = ((AssemblyCompanyAttribute)Attribute.GetCustomAttribute(Assembly.GetExecutingAssembly(), typeof(AssemblyCompanyAttribute), false)).Company;
                string product = ((AssemblyTitleAttribute)Attribute.GetCustomAttribute(Assembly.GetExecutingAssembly(), typeof(AssemblyTitleAttribute), false)).Title;
                return commonAppData + "\\" + company + "\\" + product;
            }
        }
    }
}