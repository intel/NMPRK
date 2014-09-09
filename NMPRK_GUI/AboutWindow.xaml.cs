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
using System.Reflection;

namespace NMPRK_GUI
{
    /// <summary>
    /// Interaction logic for AboutWindow.xaml
    /// </summary>
    public partial class AboutWindow : Window
    {
        public AboutWindow()
        {
            InitializeComponent();

            lblInfo.Content =
                AssemblyTitle + " v" + ProductInfo.VERSION + Environment.NewLine + 
                AssemblyCopyright + Environment.NewLine + 
                "All rights reserved.";
        }

        public AboutWindow(Window owner)
            : this()
        {
            this.Owner = owner;
        }

        #region Assembly Attribute Accessors
        public string AssemblyTitle
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 8/17/2010 
            //---------------------------------------------- 
            get
            {
                // Get all Title attributes on this assembly
                object[] attributes = Assembly.GetExecutingAssembly().GetCustomAttributes(typeof(AssemblyTitleAttribute), false);
                // If there is at least one Title attribute
                if (attributes.Length > 0)
                {
                    // Select the first one
                    AssemblyTitleAttribute titleAttribute = (AssemblyTitleAttribute)attributes[0];
                    // If it is not an empty string, return it
                    if (titleAttribute.Title != "")
                        return titleAttribute.Title;
                }
                // If there was no Title attribute, or if the Title attribute was the empty string, return the .exe name
                return System.IO.Path.GetFileNameWithoutExtension(Assembly.GetExecutingAssembly().CodeBase);
            }
        }
        public string AssemblyVersion
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 8/17/2010 
            //---------------------------------------------- 
            get
            {
                return Assembly.GetExecutingAssembly().GetName().Version.ToString();
            }
        }
        public string AssemblyDescription
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 8/17/2010 
            //---------------------------------------------- 
            get
            {
                // Get all Description attributes on this assembly
                object[] attributes = Assembly.GetExecutingAssembly().GetCustomAttributes(typeof(AssemblyDescriptionAttribute), false);
                // If there aren't any Description attributes, return an empty string
                if (attributes.Length == 0)
                    return "";
                // If there is a Description attribute, return its value
                return ((AssemblyDescriptionAttribute)attributes[0]).Description;
            }
        }
        public string AssemblyProduct
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 8/17/2010 
            //---------------------------------------------- 
            get
            {
                // Get all Product attributes on this assembly
                object[] attributes = Assembly.GetExecutingAssembly().GetCustomAttributes(typeof(AssemblyProductAttribute), false);
                // If there aren't any Product attributes, return an empty string
                if (attributes.Length == 0)
                    return "";
                // If there is a Product attribute, return its value
                return ((AssemblyProductAttribute)attributes[0]).Product;
            }
        }
        public string AssemblyCopyright
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 8/17/2010 
            //---------------------------------------------- 
            get
            {
                // Get all Copyright attributes on this assembly
                object[] attributes = Assembly.GetExecutingAssembly().GetCustomAttributes(typeof(AssemblyCopyrightAttribute), false);
                // If there aren't any Copyright attributes, return an empty string
                if (attributes.Length == 0)
                    return "";
                // If there is a Copyright attribute, return its value
                return ((AssemblyCopyrightAttribute)attributes[0]).Copyright;
            }
        }
        public string AssemblyCompany
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 8/17/2010 
            //---------------------------------------------- 
            get
            {
                // Get all Company attributes on this assembly
                object[] attributes = Assembly.GetExecutingAssembly().GetCustomAttributes(typeof(AssemblyCompanyAttribute), false);
                // If there aren't any Company attributes, return an empty string
                if (attributes.Length == 0)
                    return "";
                // If there is a Company attribute, return its value
                return ((AssemblyCompanyAttribute)attributes[0]).Company;
            }
        }
        #endregion
    }
}
