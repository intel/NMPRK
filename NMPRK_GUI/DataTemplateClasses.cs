using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Globalization;
using System.Windows.Markup;
using System.ComponentModel;
using System.Reflection;
using System.Windows.Controls;
using System.Windows;

namespace NMPRK_GUI
{
    public class EnumToListConverter : MarkupExtension, IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            Type type = value as Type;
            if (type != null && type.IsEnum)
            {
                return Enum.GetNames(type);
            }

            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }


        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            return this;
        }
    }

    //public class DependencyPropertyInfo
    //{
    //    private DependencyPropertyDescriptor _descriptor;
    //    private DependencyObject _element;

    //    public DependencyPropertyInfo(DependencyPropertyDescriptor descriptor, DependencyObject element)
    //    {
    //        this._descriptor = descriptor;
    //        this._element = element;
    //        PropertyInfo propertyInfo = descriptor.ComponentType.GetProperty(descriptor.DependencyProperty.Name);
    //        var att = propertyInfo.GetCustomAttributes(true);
    //    }

    //    public Type PropertyType
    //    {
    //        get { return _descriptor.PropertyType; }
    //    }

    //    public string Name
    //    {
    //        get { return _descriptor.Name; }
    //    }

    //    public object Value
    //    {
    //        get { return _element.GetValue(_descriptor.DependencyProperty); }
    //        set { _element.SetValue(_descriptor.DependencyProperty, value); }
    //    }
    //}

    public class PropertyDataTemplateSelector : DataTemplateSelector
    {
        public DataTemplate DefaultDataTemplate { get; set; }
        public DataTemplate BooleanDataTemplate { get; set; }
        public DataTemplate EnumDataTemplate { get; set; }
        public DataTemplate SuspendPeriodTemplate { get; set; }
        public DataTemplate DefaultDataDisplayHexTemplate { get; set; }
        public DataTemplate ByteArrayTemplate { get; set; }
        
        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            NMPRK_Types.Type t = item as NMPRK_Types.Type;
            if (t.GetType() == typeof(NMPRK_Types.Bool))
            {
                return BooleanDataTemplate;
            }
            else if (t.PropertyType != null && t.PropertyType.IsEnum)
            {
                return EnumDataTemplate;
            }
            else if (t.GetType() == typeof(NMPRK_Types.Struct_nm_policy_suspend_period_t))
            {
                return SuspendPeriodTemplate;
            }
            else if (t.GetType() == typeof(NMPRK_Types.ByteArray))
            {
                return ByteArrayTemplate;
            }
            else if (t is NMPRK_Types.NumberType && (t as NMPRK_Types.NumberType).DisplayHex == true)
            {
                return DefaultDataDisplayHexTemplate;
            }
            else
                return DefaultDataTemplate;
        }
    }

    public class PropertyDataTemplateOutputSelector : DataTemplateSelector
    {
        public DataTemplate DefaultDataTemplateOutput { get; set; }
        public DataTemplate DefaultDataDisplayHexTemplateOutput { get; set; }
        public DataTemplate BooleanDataTemplateOutput { get; set; }
        public DataTemplate EnumDataTemplateOutput { get; set; }
        public DataTemplate SuspendPeriodTemplateOutput { get; set; }
        public DataTemplate ByteArrayTemplateOutput { get; set; }

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            NMPRK_Types.Type t = item as NMPRK_Types.Type;
            if (t.GetType() == typeof(NMPRK_Types.Bool))
            {
                return BooleanDataTemplateOutput;
            }
            else if (t.PropertyType != null && t.PropertyType.IsEnum)
            {
                return EnumDataTemplateOutput;
            }
            else if (t.GetType() == typeof(NMPRK_Types.Struct_nm_policy_suspend_period_t))
            {
                return SuspendPeriodTemplateOutput;
            }
            else if (t.GetType() == typeof(NMPRK_Types.ByteArray))
            {
                return ByteArrayTemplateOutput;
            }
            else if (t is NMPRK_Types.NumberType && (t as NMPRK_Types.NumberType).DisplayHex == true)
            {
                return DefaultDataDisplayHexTemplateOutput;
            }
            else
            {
                return DefaultDataTemplateOutput;
            }
        }
    }

    public class MyEnumToStringConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return value.ToString();
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value != null)
                return Enum.Parse(targetType, value.ToString(), true);
            else
                return null;
        }
    }

    public class IntToHexStringConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return "0x" + value.ToString();
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value != null)
            {
                try
                {
                    string str = value as string;
                    if (str.StartsWith("0x") == true)
                        return System.Convert.ToUInt32(str.Substring(2), 16);
                    else if (str.ToLower().EndsWith("h") == true)
                        return System.Convert.ToUInt32(str.Substring(0, str.Length - 1), 16);
                    else
                        return System.Convert.ToUInt32(str);
                }
                catch
                {
                    return null;
                }
            }
            else
                return null;
        }
    }
    //public static class TypeHelper
    //{
    //    public static IEnumerable<DependencyPropertyInfo> GetDependencyProperties(Type t, DependencyObject element)
    //    {

    //        foreach (PropertyDescriptor pd in TypeDescriptor.GetProperties(t,
    //                    new Attribute[] { new PropertyFilterAttribute(PropertyFilterOptions.SetValues | 
    //                                                                            PropertyFilterOptions.UnsetValues | 
    //                                                                            PropertyFilterOptions.Valid) }))
    //        {
    //            DependencyPropertyDescriptor dpd = DependencyPropertyDescriptor.FromProperty(pd);
    //            if (dpd != null)
    //            {
    //                yield return new DependencyPropertyInfo(dpd, element);
    //            }
    //        }
    //    }
    //}
}
