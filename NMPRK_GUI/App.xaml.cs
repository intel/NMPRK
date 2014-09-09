using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;

namespace NMPRK_GUI
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            WpfSingleInstance.Make("572ce2a0-3769-11e3-aa6e-0800200c9a66", this);

            base.OnStartup(e);
        }
    }
}
