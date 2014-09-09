using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Controls;
using System.Globalization;
using System.Windows;

namespace NMPRK_GUI
{
    public class NumberValidationRule : ValidationRule
    {
        private ulong MinimumValue = ulong.MinValue;
        private ulong MaximumValue = ulong.MaxValue;
        private bool Hex = false;

        public NumberValidationRule(ulong min, ulong max, bool hex)
        {
            MinimumValue = min;
            MaximumValue = max;
            Hex = hex;
        }

        public override ValidationResult Validate(object value,
            CultureInfo cultureInfo)
        {
            ValidationResult result = new ValidationResult(true, null);

            UInt64 num = 0;

            try
            {
                if (value == null)
                {
                    result = new ValidationResult(false, "Expected a number");
                }

                string str = value as string;
                if (str == "")
                {
                }
                else if (str.ToLower().StartsWith("0x") == true)
                {
                    num = System.Convert.ToUInt64(str.Substring(2), 16);
                }
                else if (str.ToLower().EndsWith("h") == true)
                {
                    num = System.Convert.ToUInt64(str.Substring(0, str.Length-1), 16);
                }
                else
                {
                    num = System.Convert.ToUInt64(str);
                }

                if (num < MinimumValue || num > MaximumValue)
                {
                    if (Hex == true)
                        result = new ValidationResult(false, "The value must be in the following range: " + MinimumValue.ToString("X2") + "-" + MaximumValue.ToString("X2"));
                    else
                        result = new ValidationResult(false, "The value must be in the following range: " + MinimumValue + "-" + MaximumValue);
                }
            }
            catch (FormatException ex)
            {
                result = new ValidationResult(false, ex.Message);
            }
            catch (Exception ex)
            {
                result = new ValidationResult(false, ex.Message);
            }

            return result;
        }
    }

    public class HexStringValidationRule : ValidationRule
    {
        public override ValidationResult Validate(object value,
            CultureInfo cultureInfo)
        {
            ValidationResult result = new ValidationResult(true, null);

            const string errorMsg = "Expected a hex string that represents a byte array. Example: 45 F3 C5 1A... (spaces between bytes not required)\nNote: An even number of characters is required as 2 hex characters represent one byte";
            try
            {
                if (value == null)
                {
                    result = new ValidationResult(false, errorMsg);
                }

                string str = value as string;
                str = Regex.Replace(str, @"\s+", ""); // remove all whitespace
                if (str.Length > 0)
                {
                    for (int i = 0; i < str.Length; i += 2)
                    {
                        byte b = Convert.ToByte(str.Substring(i, 2), 16);
                    }
                }
            }
            catch (FormatException ex)
            {
                result = new ValidationResult(false, errorMsg + "\n\n" + ex.Message);
            }
            catch (Exception ex)
            {
                result = new ValidationResult(false, errorMsg + "\n\n" + ex.Message);
            }

            return result;
        }
    }
}
