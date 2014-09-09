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
using System.Windows.Shapes;
using System.Net;
using System.Threading;
using System.Diagnostics;
using NMPRK;
using IntelShared.Common;

namespace NMPRK_GUI
{
    /// <summary>
    /// Interaction logic for ConnectionWindow.xaml
    /// </summary>
    public partial class ConnectionWindow : Window
    {
        public static bool Connected { get; protected set; }
        public static int ConnectionHandle { get; protected set; }

        static ConnectionWindow()
        {
            Connected = false;
            ConnectionHandle = -1;
        }
    
        public ConnectionWindow()
        {
            InitializeComponent();

            if (Properties.Settings.Default.ConnectionMode == "Remote")
                rbRemote.IsChecked = true;
            else
                rbLocal.IsChecked = true;

            if (Connected == true)
            {
                btnConnect.Content = "Disconnect";
            }

            tbIpAddress.Text = Properties.Settings.Default.IpAddress;
            tbUsername.Text = Properties.Settings.Default.Username;
            tbPassword.Password = Properties.Settings.Default.Password;

            //rbRemote.IsChecked = true;
            //tbIpAddress.Text = "10.20.0.41";
            //tbUsername.Text = "root";
            //tbPassword.Password = "password";

            EventManager.RegisterClassHandler(typeof(TextBox), TextBox.GotFocusEvent, new RoutedEventHandler(TextBox_SelectAllText));
            EventManager.RegisterClassHandler(typeof(TextBox), TextBox.PreviewMouseDownEvent, new MouseButtonEventHandler(TextBox_SelectivelyIgnoreMouseButton));
        }

        private void TextBox_SelectAllText(object sender, RoutedEventArgs e)
        {
            ((TextBox)sender).SelectAll();
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

                if (Connected == false)
                {
                    SaveSettings();

                    ConnectionHandle = -1;
                    Connected = false;

                    ConnectInfo ci = new ConnectInfo();
                    ci.Remote = rbRemote.IsChecked.Value;
                    ci.Ip = tbIpAddress.Text;
                    ci.User = tbUsername.Text;
                    ci.Pass = tbPassword.Password;

                    circularProgressBar1.Visibility = System.Windows.Visibility.Visible;

                    _connectThread = new Thread(ConnectThread);
                    _connectThread.Start(ci);
                }
                else
                {
                    _connectThread = new Thread(DisconnectThread);
                    _connectThread.Start();
                }
            }
            catch (Exception ex)
            {
                ErrorLog.LogException(ex);
            }
        }

        private class ConnectInfo
        {
            public bool Remote = false;
            public string Ip;
            public string User;
            public string Pass;
        }

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
                    NMPRK.nmprk_connect_remote_parameters_t input = new nmprk_connect_remote_parameters_t();
                    input.ipOrHostname = ci.Ip;
                    input.username = ci.User;
                    input.password = ci.Pass;
                    status = NMPRK_API.ConnectRemote(ref input, ref handle);
                }
                else
                {
                    ErrorLog.WriteLine("Connecting to Local Host...");
                    status = NMPRK_API.ConnectLocal(ref handle);
                }

                ErrorLog.WriteLine("Connect Status: " + status);
                if (status == 0)
                {
                    ErrorLog.WriteLine("Searching for NM Discovery Parameters...");
                    UpdateConnectionStatus("Searching for NM Discovery Parameters...");
                    nm_discovery_parameters_t parameters = new nm_discovery_parameters_t();
                    status = NMPRK_API.GetDiscoveryParameters(handle, ref parameters);
                    if (status != NMPRK_API.NMPRK_SUCCESS)
                    {
                        DisplayNmprkError("GetDiscoveryParameters", status, "The server may not support Node Manager or has not implemented the 'Node Manager OEM SDR'");
                        NMPRK_API.Disconnect(handle);
                        ConnectThreadComplete(false);
                    }
                    else
                    {
                        ErrorLog.WriteLine("NM Channel: 0x" + parameters.channel.ToString("X2"));
                        ErrorLog.WriteLine("NM Address: 0x" + parameters.address.ToString("X2"));
                        status = NMPRK_API.SetDefaultNmCommandBridging(handle, parameters.channel, parameters.address);
                        if (status != NMPRK_API.NMPRK_SUCCESS)
                        {
                            DisplayNmprkError("SetDefaultNmCommandBridging", status);
                            NMPRK_API.Disconnect(handle);
                            ConnectThreadComplete(false);
                        }
                        else
                        {
                            ConnectionHandle = handle;
                            Connected = true;
                            ConnectThreadComplete(true);
                        }
                    }
                }
                else
                {
                    DisplayNmprkError("Connect", status);
                    NMPRK_API.Disconnect(handle);
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
                NMPRK_API.Disconnect(ConnectionHandle);
                ConnectionHandle = -1;
                Connected = false;
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
                        this.Close();
                    }
                    else
                    {
                        ConnectionHandle = -1;
                        Connected = false;
                    }

                    UpdateConnectionStatus("");
                    circularProgressBar1.Visibility = System.Windows.Visibility.Hidden;
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
                btnConnect.Content = "Connect";
                
                btnConnect.IsEnabled = true;
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
                label1.Content = status;
            }
            else
            {
                this.Dispatcher.Invoke(
                        System.Windows.Threading.DispatcherPriority.Normal,
                        new UpdateConnectionStatusDelegate(UpdateConnectionStatus), status);
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
                msg += (function + " Failed! Error Code: 0x" + status.ToString("X2"));
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
            Properties.Settings.Default.Save();
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void Hyperlink_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Process.Start(@"doc\NMPRK_GUI-Help.chm");
            }
            catch (Exception)
            {
            }
        }
    }
}
