using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.ComponentModel;

namespace IntelShared.Common
{
    class PowerScheme
    {
        private static IntPtr GuidToPtr(Guid guid)
        {
            if (guid == Guid.Empty)
                return IntPtr.Zero;

            var guidPtr = Marshal.AllocHGlobal(Marshal.SizeOf(guid));
            Marshal.StructureToPtr(guid, guidPtr, false);
            return guidPtr;
        }

        // Default Schemes
        public class GuidDefaultScheme
        {
            public static readonly Guid GUID_SCHEME_HIGH_PERFORMANCE = new Guid(0x8c5e7fda, 0xe8bf, 0x4a96, 0x9a, 0x85, 0xa6, 0xe2, 0x3a, 0x8c, 0x63, 0x5c);
            public static readonly Guid GUID_SCHEME_BALANCED = new Guid(0x381b4222, 0xf694, 0x41f0, 0x96, 0x85, 0xff, 0x5b, 0xb2, 0x60, 0xdf, 0x2e);
            public static readonly Guid GUID_SCHEME_POWER_SAVER = new Guid(0xa1841308, 0x3541, 0x4fab, 0xbc, 0x81, 0xf7, 0x15, 0x56, 0xf2, 0x0b, 0x4a);
        }

        // Subgroups
        public class GuidSubgroup
        {
            public static readonly Guid GUID_NO_SUBGROUP = new Guid("fea3413e-7e05-4911-9a71-700331f1c294");
            public static readonly Guid GUID_DISK_SUBGROUP = new Guid("0012ee47-9041-4b5d-9b77-535fba8b1442");
            public static readonly Guid GUID_SYSTEM_BUTTON_SUBGROUP = new Guid("4f971e89-eebd-4455-a8de-9e59040e7347");
            public static readonly Guid GUID_PROCESSOR_SETTINGS_SUBGROUP = new Guid("54533251-82be-4824-96c1-47b60b740d00");
            public static readonly Guid GUID_VIDEO_SUBGROUP = new Guid("7516b95f-f776-4464-8c53-06167f40cc99");
            public static readonly Guid GUID_BATTERY_SUBGROUP = new Guid("e73a048d-bf27-4f12-9731-8b2076e8891f");
            public static readonly Guid GUID_SLEEP_SUBGROUP = new Guid("238c9fa8-0aad-41ed-83f4-97be242c8f20");
            public static readonly Guid GUID_PCIEXPRESS_SETTINGS_SUBGROUP = new Guid("501a4d13-42af-4429-9fd1-a8218c268e20");
        }

        // Processor Group Settings
        public class GuidProcessorGroupSettings
        {
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_INCREASE_THRESHOLD = new Guid("06cadf0e-64ed-448a-8927-ce7bf90eb35d");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_MIN_CORES = new Guid("0cc5b647-c1df-4637-891a-dec35c318583");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_OVER_UTILIZATION_HISTORY_DECREASE_FACTOR = new Guid("1299023c-bc28-4f0a-81ec-d3295a8d815d");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_DECREASE_THRESHOLD = new Guid("12a0ab44-fe28-4fa9-b3bd-4b64f44960a6");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_INCREASE_TIME = new Guid("2ddd5a84-5a71-437e-912a-db0b8c788732");
            public static readonly Guid GUID_ALLOW_THROTTLE_STATES = new Guid("3b04d4fd-1cc7-4f23-ab1c-d1337819c4bb");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_DECREASE_POLICY = new Guid("40fbefc7-2e9d-4d25-a185-0cfd8574bac6");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_PARKED_PERFORMANCE_STATE = new Guid("447235c7-6a8d-4cc0-8e24-9eaf70b96e2b");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_BOOST_POLICY = new Guid("45bcc044-d885-43e2-8605-ee0ec6e96b59");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_INCREASE_POLICY = new Guid("465e1f50-b610-473a-ab58-00d1077dc418");
            public static readonly Guid GUID_PROCESSOR_IDLE_DEMOTE_THRESHOLD = new Guid("4b92d758-5a24-4851-a470-815d78aee119");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_TIME_CHECK_INTERVAL = new Guid("4d2b0152-7d5c-498b-88e2-34345392a2c5");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_AFFINITY_HISTORY_THRESHOLD = new Guid("5b33697b-e89d-4d38-aa46-9e7dfb7cd2f9");
            public static readonly Guid GUID_PROCESSOR_IDLE_DISABLE = new Guid("5d76a2ca-e8c0-402f-a133-2158492d58ad");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_DECREASE_THRESHOLD = new Guid("68dd2f27-a4ce-4e11-8487-3794e4135dfa");
            public static readonly Guid GUID_PROCESSOR_IDLE_THRESHOLD_SCALING = new Guid("6c2993b0-8f48-481f-bcc6-00dd2742aa06");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_DECREASE_POLICY = new Guid("71021b41-c749-4d21-be74-a00f335d582b");
            public static readonly Guid GUID_PROCESSOR_IDLE_PROMOTE_THRESHOLD = new Guid("7b224883-b3cc-4d79-819f-8374152cbe7c");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_HISTORY_COUNT = new Guid("7d24baa7-0b84-480f-840c-1b0743c00f5f");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_OVER_UTILIZATION_WEIGHTING = new Guid("8809c2d8-b155-42d4-bcda-0d345651b1db");
            public static readonly Guid GUID_MINIMUM_PROCESSOR_STATE = new Guid("893dee8e-2bef-41e0-89c6-b55d0929964c");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_AFFINITY_HISTORY_DECREASE_FACTOR = new Guid("8f7b45e3-c393-480a-878c-f67ac3d07082");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_OVERUTILIZATION_THRESHOLD = new Guid("943c8cb6-6f93-4227-ad87-e9a3feec08d1");
            public static readonly Guid GUID_SYSTEM_COOLING_POLICY = new Guid("94d3a615-a899-4ac5-ae2b-e4d8f634367f");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_INCREASE_TIME = new Guid("984cf492-3bed-4488-a8f9-4286c97bf5aa");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_OVER_UTILIZATION_HISTORY_THRESHOLD = new Guid("9ac18e92-aa3c-4e27-b307-01ae37307129");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_CORE_OVERRIDE = new Guid("a55612aa-f624-42c6-a443-7397d064c04f");
            public static readonly Guid GUID_MAXIMUM_PROCESSOR_STATE = new Guid("bc5038f7-23e0-4960-96da-33abaf5935ec");
            public static readonly Guid GUID_PROCESSOR_IDLE_TIME_CHECK = new Guid("c4581c31-89ab-4597-8e2b-9c9cab440e6b");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_INCREASE_POLICY = new Guid("c7be0679-2817-4d69-9d02-519a537ed0c6");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_DECREASE_TIME = new Guid("d8edeb9b-95cf-4f95-a73c-b061973693c8");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_INCREASE_THRESHOLD = new Guid("df142941-20f3-4edf-9a4a-9c83d3d717d1");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_DECREASE_TIME = new Guid("dfd10d17-d5eb-45dd-877a-9a34ddd15c82");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_AFFINITY_WEIGHTING = new Guid("e70867f1-fa2f-4f4e-aea1-4d8a0ba23b20");
            public static readonly Guid GUID_PROCESSOR_PERFORMANCE_CORE_PARKING_MAX_CORES = new Guid("ea062031-0e34-4ff1-9b6d-eb1059334028");
        }

        #region Interop Methods
        [DllImport("powrprof.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "PowerGetActiveScheme")]
        private static extern UInt32 PowerGetActiveScheme(IntPtr UserRootPowerKey, ref IntPtr ActivePolicyGuid);
        [DllImport("powrprof.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "PowerSetActiveScheme")]
        private static extern uint PowerSetActiveScheme(IntPtr rootPowerKey, IntPtr schemeGuid);
        [DllImport("powrprof.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "PowerReadACValueIndex")]
        private static extern UInt32 PowerReadACValueIndex(
            IntPtr rootPowerKey,
            IntPtr schemeGuid,
            IntPtr subGroupOfPowerSettingGuid,
            IntPtr settingGuid,
            ref uint valueIndex);
        [DllImport("powrprof.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "PowerWriteACValueIndex")]
        private static extern UInt32 PowerWriteACValueIndex(
            IntPtr rootPowerKey,
            IntPtr schemeGuid,
            IntPtr subGroupOfPowerSettingGuid,
            IntPtr settingGuid,
            uint valueIndex);
        [DllImport("powrprof.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "PowerDuplicateScheme")]
        private static extern UInt32 PowerDuplicateScheme(
            IntPtr RootPowerKey,
            IntPtr SrcSchemeGuid,
            ref IntPtr DstSchemeGuid);
        [DllImport("powrprof.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "PowerDeleteScheme")]
        private static extern UInt32 PowerDeleteScheme(
            IntPtr RootPowerKey,
            IntPtr SchemeGuid);
        [DllImport("powrprof.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "PowerWriteFriendlyName")]
        private static extern UInt32 PowerWriteFriendlyName(
            IntPtr RootPowerKey,
            IntPtr SchemeGuid,
            IntPtr SubGroupOfPowerSettingGuid,
            IntPtr PowerSettingGuid,
            String Buffer,
            UInt32 BufferSize);
        [DllImport("powrprof.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "PowerWriteDescription")]
        private static extern UInt32 PowerWriteDescription(
            IntPtr RootPowerKey,
            IntPtr SchemeGuid,
            IntPtr SubGroupOfPowerSettingGuid,
            IntPtr PowerSettingGuid,
            String Buffer,
            UInt32 BufferSize);
        [DllImport("powrprof.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "PowerEnumerate")]
        static extern uint PowerEnumerate(
            IntPtr RootPowerKey,
            IntPtr SchemeGuid,
            IntPtr SubGroupOfPowerSettingGuid,
            uint AccessFlags,
            uint Index,
            ref Guid Buffer,
            ref uint BufferSize);
        [DllImport("powrprof.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "PowerReadACValue")]
        static extern uint PowerReadACValue(
            IntPtr RootPowerKey,
            IntPtr SchemeGuid,
            IntPtr SubGroupOfPowerSettingGuid,
            ref Guid PowerSettingGuid,
            ref int Type,
            ref IntPtr Buffer,
            ref uint BufferSize
            );
        [DllImport("powrprof.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "PowerReadFriendlyName")]
        static extern uint PowerReadFriendlyName(
            IntPtr RootPowerKey,
            IntPtr SchemeGuid,
            IntPtr SubGroupOfPowerSettingGuid,
            IntPtr PowerSettingGuid,
            StringBuilder Buffer,
            ref uint BufferSize
            );

        #endregion

        private IntPtr _scheme = IntPtr.Zero;

        public IntPtr GetSchemeGuid
        {
            get { return _scheme; }
        }

        private PowerScheme(IntPtr schemeGuid)
        {
            _scheme = schemeGuid;
        }

        public static PowerScheme GetActiveScheme()
        {
            IntPtr scheme = IntPtr.Zero;

            PowerGetActiveScheme(IntPtr.Zero, ref scheme);

            return new PowerScheme(scheme);
        }

        public static PowerScheme NewScheme(string name, string description)
        {
            IntPtr scheme = IntPtr.Zero;
            IntPtr newScheme = IntPtr.Zero;

            PowerGetActiveScheme(IntPtr.Zero, ref scheme);

            PowerDuplicateScheme(IntPtr.Zero, scheme, ref newScheme);

            PowerWriteFriendlyName(IntPtr.Zero, newScheme, IntPtr.Zero, IntPtr.Zero, name, (uint)System.Text.ASCIIEncoding.Unicode.GetByteCount(name));
            PowerWriteDescription(IntPtr.Zero, newScheme, IntPtr.Zero, IntPtr.Zero, description, (uint)System.Text.ASCIIEncoding.Unicode.GetByteCount(description));

            return new PowerScheme(newScheme);
        }

        public static void DeleteScheme(PowerScheme scheme)
        {
            PowerDeleteScheme(IntPtr.Zero, scheme.GetSchemeGuid);
        }

        public void SetAsActiveScheme()
        {
            PowerSetActiveScheme(IntPtr.Zero, _scheme);
            //PowerSetActiveScheme(IntPtr.Zero, GuidToPtr(GUID_SCHEME_HIGH_PERFORMANCE));
        }

        public uint ReadACValueIndex(Guid SubGroupGuid, Guid PowerSettingGuid)
        {
            uint value = 0;
            PowerReadACValueIndex(IntPtr.Zero, _scheme, GuidToPtr(SubGroupGuid), GuidToPtr(PowerSettingGuid), ref value);
            return value;
        }

        public void WriteACValueIndex(Guid SubGroupGuid, Guid PowerSettingGuid, uint value)
        {
            PowerWriteACValueIndex(IntPtr.Zero, _scheme, GuidToPtr(SubGroupGuid), GuidToPtr(PowerSettingGuid), value);
        }

        private const uint ERROR_MORE_DATA = 234;

        public override string ToString()
        {
            string str = "";

            IntPtr activeGuidPtr = _scheme;
            try
            {
                uint res = 0;
                //uint res = PowerGetActiveScheme(IntPtr.Zero, ref activeGuidPtr);
                //if (res != 0)
                //    throw new Win32Exception();

                //Get Friendly Name
                uint buffSize = 0;
                StringBuilder buffer = new StringBuilder();
                Guid subGroupGuid = Guid.Empty;
                Guid powerSettingGuid = Guid.Empty;
                res = PowerReadFriendlyName(IntPtr.Zero, activeGuidPtr,
                    IntPtr.Zero, IntPtr.Zero, buffer, ref buffSize);

                if (res == ERROR_MORE_DATA)
                {
                    buffer.Capacity = (int)buffSize;
                    res = PowerReadFriendlyName(IntPtr.Zero, activeGuidPtr,
                        IntPtr.Zero, IntPtr.Zero, buffer, ref buffSize);
                }

                if (res != 0)
                    throw new Win32Exception();

                Console.WriteLine("ReadFriendlyName = " +
                    buffer.ToString());

                //Get the Power Settings
                Guid VideoSettingGuid = Guid.Empty;
                uint index = 0;
                uint BufferSize = Convert.ToUInt32(Marshal.SizeOf(typeof(Guid)));

                while (
                    PowerEnumerate(IntPtr.Zero, activeGuidPtr, GuidToPtr(GuidSubgroup.GUID_PROCESSOR_SETTINGS_SUBGROUP),
                    18, index, ref VideoSettingGuid, ref BufferSize) == 0)
                {
                    uint size = 4;
                    IntPtr temp = IntPtr.Zero;
                    int type = 0;
                    res = PowerReadACValue(IntPtr.Zero, activeGuidPtr, IntPtr.Zero,
                        ref VideoSettingGuid, ref type, ref temp, ref size);

                    IntPtr pSubGroup = Marshal.AllocHGlobal(Marshal.SizeOf(GuidSubgroup.GUID_PROCESSOR_SETTINGS_SUBGROUP));
                    Marshal.StructureToPtr(GuidSubgroup.GUID_PROCESSOR_SETTINGS_SUBGROUP, pSubGroup, false);
                    IntPtr pSetting = Marshal.AllocHGlobal(Marshal.SizeOf(VideoSettingGuid));
                    Marshal.StructureToPtr(VideoSettingGuid, pSetting, false);

                    uint builderSize = 200;
                    StringBuilder builder = new StringBuilder((int)builderSize);
                    res = PowerReadFriendlyName(IntPtr.Zero, activeGuidPtr,
                        pSubGroup, pSetting, builder, ref builderSize);

                    if(str != "")
                        str += Environment.NewLine;
                    str += (builder.ToString() + " = " + temp.ToString());
                    //Console.WriteLine(builder.ToString() + " = " + temp.ToString());
                    //Console.WriteLine(builder.ToString());
                    Console.WriteLine(temp.ToString());
                    //Console.WriteLine(VideoSettingGuid.ToString());
                    //Console.WriteLine("private static readonly Guid GUID_" + builder.ToString().ToUpper().Replace(' ', '_') + " = new Guid(\"" + VideoSettingGuid.ToString() + "\");");

                    index++;
                }
            }
            finally
            {
                if (activeGuidPtr != IntPtr.Zero)
                {
                    //IntPtr res = LocalFree(activeGuidPtr);
                    //if (res != IntPtr.Zero)
                    //    throw new Win32Exception();
                }
            }

            return str;
        }
    }
}
