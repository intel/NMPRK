using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using NMPRK;

namespace NMPRK_Types
{
    public abstract class Type
    {
        public string Name { get; protected set; }

        public Type(string n)
        {
            Name = n;
        }

        public abstract System.Type PropertyType { get; }
    }

    public abstract class NumberType : Type
    {
        public bool DisplayHex { get; protected set; }

        public ulong MinValue { get; set; }
        public ulong MaxValue { get; set; }

        public NMPRK_GUI.NumberValidationRule ValidationRule { get; protected set; }

        public NumberType(string n, bool hexFormat)
            : base(n)
        {
            DisplayHex = hexFormat;
            MinValue = 5;
            MaxValue = 10;
        }
    }

    public class StringType : Type
    {
        public string Value { get; set; }

        public override System.Type PropertyType { get { return Value.GetType(); } }

        public StringType(string n, string v)
            : base(n)
        {
            Value = v;
        }
    }

    public class U8 : NumberType
    {
        public byte Value { get; set; }

        public override System.Type PropertyType { get { return Value.GetType(); } }

        public U8(string n, byte v, bool hex = true)
            : base(n, hex)
        {
            Value = v;
            ValidationRule = new NMPRK_GUI.NumberValidationRule(byte.MinValue, byte.MaxValue, hex);
        }
    }

    public class U16 : NumberType
    {
        public ushort Value { get; set; }

        public override System.Type PropertyType { get { return Value.GetType(); } }

        public U16(string n, UInt16 v, bool hex = true)
            : base(n, hex)
        {
            Value = v;
            ValidationRule = new NMPRK_GUI.NumberValidationRule(ushort.MinValue, ushort.MaxValue, hex);
        }
    }

    public class U32 : NumberType
    {
        public uint Value { get; set; }

        public override System.Type PropertyType { get { return Value.GetType(); } }

        public U32(string n, UInt32 v, bool hex = true)
            : base(n, hex)
        {
            Value = v;
            ValidationRule = new NMPRK_GUI.NumberValidationRule(uint.MinValue, uint.MaxValue, hex);
        }
    }

    public class U64 : NumberType
    {
        public ulong Value { get; set; }

        public override System.Type PropertyType { get { return Value.GetType(); } }

        public U64(string n, UInt64 v, bool hex = true)
            : base(n, hex)
        {
            Value = v;
            ValidationRule = new NMPRK_GUI.NumberValidationRule(ulong.MinValue, ulong.MaxValue, hex);
        }
    }

    public class Bool : Type
    {
        public bool Value { get; set; }

        public override System.Type PropertyType { get { return Value.GetType(); } }

        public Bool(string n, bool v)
            : base(n)
        {
            Value = v;
        }
    }

    public class ByteArray : Type
    {
        public byte[] Value { get; set; }

        public override System.Type PropertyType { get { return Value.GetType(); } }

        public ByteArray(string n, byte[] v)
            : base(n)
        {
            Value = v;
        }

        public string ValueAsString
        {
            get
            {
                string v = "";
                foreach (byte b in Value)
                {
                    if (v != "")
                        v += " ";
                    v += b.ToString("X2");
                }
                return v;
            }
            set
            {
                try
                {
                    string hexStr = Regex.Replace(value, @"\s+", "");
                    List<byte> buffer = new List<byte>();
                    for (int i = 0; i < hexStr.Length; i += 2)
                        buffer.Add(Convert.ToByte(hexStr.Substring(i, 2), 16));
                    Value = buffer.ToArray();
                }
                catch
                {
                    Value = null;
                }
            }
        }
    }

    public class Struct_nm_policy_suspend_period_t : Type
    {
        public nm_policy_suspend_period_t Value { get; set; }

        public class SuspendPeriodTime
        {
            public byte Value { get; protected set; }

            public SuspendPeriodTime(byte value)
            {
                Value = value;
            }

            public override string ToString()
            {
                int hour = Value / 10;
                int min = (Value % 10) * 6;

                return hour.ToString("00") + ":" + min.ToString("00");
            }
        }

        private SuspendPeriodTime _startTime = new SuspendPeriodTime(0);
        public SuspendPeriodTime StartTime
        {
            get { return _startTime; }
            set 
            { 
                _startTime = value; 
                if (_startTime != null) 
                    Value = new nm_policy_suspend_period_t() { startTime = _startTime.Value, stopTime = Value.stopTime, recurrencePattern = Value.recurrencePattern }; 
            }
        }

        private SuspendPeriodTime _stopTime = new SuspendPeriodTime(0);
        public SuspendPeriodTime StopTime
        {
            get { return _stopTime; }
            set 
            { 
                _stopTime = value; 
                if (_stopTime != null) 
                    Value = new nm_policy_suspend_period_t() { startTime = Value.startTime, stopTime = _stopTime.Value, recurrencePattern = Value.recurrencePattern }; 
            }
        }

        public nm_suspend_period_recurrence_pattern_t UpdateRecurrencePattern(nm_suspend_period_recurrence_pattern_t current, nm_suspend_period_recurrence_pattern_t day, bool on)
        {
            if (on == true)
                return current | day;
            else
                return current & ~day;
        }

        public bool RecurrenceMonday
        {
            get { return ((int)Value.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.MONDAY) != 0; }
            set { Value = new nm_policy_suspend_period_t(Value.startTime, Value.stopTime, UpdateRecurrencePattern(Value.recurrencePattern, nm_suspend_period_recurrence_pattern_t.MONDAY, value)); }
        }

        public bool RecurrenceTuesday
        {
            get { return ((int)Value.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.TUESDAY) != 0; }
            set { Value = new nm_policy_suspend_period_t(Value.startTime, Value.stopTime, UpdateRecurrencePattern(Value.recurrencePattern, nm_suspend_period_recurrence_pattern_t.TUESDAY, value)); }
        }

        public bool RecurrenceWednesday
        {
            get { return ((int)Value.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.WEDNESDAY) != 0; }
            set { Value = new nm_policy_suspend_period_t(Value.startTime, Value.stopTime, UpdateRecurrencePattern(Value.recurrencePattern, nm_suspend_period_recurrence_pattern_t.WEDNESDAY, value)); }
        }

        public bool RecurrenceThursday
        {
            get { return ((int)Value.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.THURSDAY) != 0; }
            set { Value = new nm_policy_suspend_period_t(Value.startTime, Value.stopTime, UpdateRecurrencePattern(Value.recurrencePattern, nm_suspend_period_recurrence_pattern_t.THURSDAY, value)); }
        }

        public bool RecurrenceFriday
        {
            get { return ((int)Value.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.FRIDAY) != 0; }
            set { Value = new nm_policy_suspend_period_t(Value.startTime, Value.stopTime, UpdateRecurrencePattern(Value.recurrencePattern, nm_suspend_period_recurrence_pattern_t.FRIDAY, value)); }
        }

        public bool RecurrenceSaturday
        {
            get { return ((int)Value.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.SATURDAY) != 0; }
            set { Value = new nm_policy_suspend_period_t(Value.startTime, Value.stopTime, UpdateRecurrencePattern(Value.recurrencePattern, nm_suspend_period_recurrence_pattern_t.SATURDAY, value)); }
        }

        public bool RecurrenceSunday
        {
            get { return ((int)Value.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.SUNDAY) != 0; }
            set { Value = new nm_policy_suspend_period_t(Value.startTime, Value.stopTime, UpdateRecurrencePattern(Value.recurrencePattern, nm_suspend_period_recurrence_pattern_t.SUNDAY, value)); }
        }

        public List<SuspendPeriodTime> _suspendPeriodTimeOptions = new List<SuspendPeriodTime>();

        public List<SuspendPeriodTime> SuspendPeriodTimeOptions { get { return _suspendPeriodTimeOptions; } }

        public override System.Type PropertyType { get { return Value.GetType(); } }

        public Struct_nm_policy_suspend_period_t(string n, nm_policy_suspend_period_t v)
            : base(n)
        {
            for (byte b = 0; b < 240; b++)
                _suspendPeriodTimeOptions.Add(new SuspendPeriodTime(b));

            Value = v;
            foreach (SuspendPeriodTime s in _suspendPeriodTimeOptions)
            {
                if (s.Value == v.startTime)
                    StartTime = s;
                if (s.Value == v.stopTime)
                    StopTime = s;
            }
            RecurrenceMonday = ((int)v.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.MONDAY) != 0;
            RecurrenceTuesday = ((int)v.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.TUESDAY) != 0;
            RecurrenceWednesday = ((int)v.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.WEDNESDAY) != 0;
            RecurrenceThursday = ((int)v.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.THURSDAY) != 0;
            RecurrenceFriday = ((int)v.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.FRIDAY) != 0;
            RecurrenceSaturday = ((int)v.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.SATURDAY) != 0;
            RecurrenceSunday = ((int)v.recurrencePattern & (int)nm_suspend_period_recurrence_pattern_t.SUNDAY) != 0;
        }
    }

    public class Enum<T> : Type
    {
        public T Value { get; set; }

        public override System.Type PropertyType { get { return Value.GetType(); } }

        public Enum(string n, T v)
            : base(n)
        {
            Value = v;
        }
    }
}
