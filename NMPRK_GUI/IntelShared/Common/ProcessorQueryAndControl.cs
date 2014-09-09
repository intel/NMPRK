using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;
using System.Collections;
using System.ComponentModel;

namespace IntelShared.Common
{
    public class ProcessorQueryAndControl
    {
        private const int NUM_PROCESSOR_GROUPS = 16; // only 4 groups supported now, but add more for future

        static ProcessorQueryAndControl()
        {
            CurrentProperties = null;
        }

        #region Public Enums
        public enum PackagePowerLimitIndex
        {
            _1,
            _2
        }

        public enum PackagePowerLimitMask
        {
            PowerLimit = 1,
            TimeWindowForPowerLimit = 2,
            EnablePowerLimit = 4,
            PackageClampingLimitation = 8
        }
        #endregion

        #region Public Classes
        public class LogicalProcessorInfo
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            // Variables
            //----------------------------------------------
            private ushort _GroupNumber;
            private byte _GroupProcessor;
            private byte _AbsoluteProcessor;
            private UIntPtr _ProcessorMask;
            //----------------------------------------------
            // Constructor
            //----------------------------------------------
            public LogicalProcessorInfo(ushort GN, byte GP, byte AP, UIntPtr M)
            {
                //------------------------------------------ 
                // Intel Corporation 
                // Copyright © 2009/2010 - All Rights Reserved 
                // Department : SST/NTE 
                // Written by : Bill Hines - william.l.hines@intel.com 
                // Modified by : N/A 
                // Date : 12/4/2009 
                //------------------------------------------
                _GroupNumber = GN;
                _GroupProcessor = GP;
                _AbsoluteProcessor = AP;
                _ProcessorMask = M;
            }
            //----------------------------------------------
            // Properties
            //----------------------------------------------
            public ushort GroupNumber
            {
                //------------------------------------------ 
                // Intel Corporation 
                // Copyright © 2009/2010 - All Rights Reserved 
                // Department : SST/NTE 
                // Written by : Bill Hines - william.l.hines@intel.com 
                // Modified by : N/A 
                // Date : 12/4/2009 
                //------------------------------------------
                get { return _GroupNumber; }
                set { _GroupNumber = (ushort)value; }
            }
            public byte GroupProcessor
            {
                get { return _GroupProcessor; }
                set { _GroupProcessor = (byte)value; }
            }
            public byte AbsoluteProcessor
            {
                //------------------------------------------ 
                // Intel Corporation 
                // Copyright © 2009/2010 - All Rights Reserved 
                // Department : SST/NTE 
                // Written by : Bill Hines - william.l.hines@intel.com 
                // Modified by : N/A 
                // Date : 12/4/2009 
                //------------------------------------------
                get { return _AbsoluteProcessor; }
                set { _AbsoluteProcessor = (byte)value; }
            }
            public UIntPtr ProcessorMask
            {
                //------------------------------------------ 
                // Intel Corporation 
                // Copyright © 2009/2010 - All Rights Reserved 
                // Department : SST/NTE 
                // Written by : Bill Hines - william.l.hines@intel.com 
                // Modified by : N/A 
                // Date : 12/4/2009 
                //------------------------------------------
                get { return _ProcessorMask; }
                set { _ProcessorMask = (UIntPtr)value; }
            }
        }

        public class PackagePowerLimit
        {
            public double PowerLimit;
            public double TimeWindowForPowerLimit;
            public bool EnablePowerLimit;
            public bool PackageClampingLimitation;
        }
        #endregion

        #region Public Methods
        public static bool Is64BitProcess
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            get
            {
                return Marshal.SizeOf(typeof(IntPtr)) == 8;
            }
        }
        public static bool Is64BitWindows
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            get
            {
                try
                {
                    string sArchitecture = Environment.GetEnvironmentVariable(
                    "PROCESSOR_ARCHITECTURE", EnvironmentVariableTarget.Machine);
                    if (sArchitecture == null)
                        return false;
                    else
                        return sArchitecture.Contains("64");
                }
                catch (NotSupportedException)
                {
                    return false;
                }
                catch (ArgumentException)
                {
                    return false;
                }
            }
        }
        public static bool IsWow64Process
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            get
            {
                return Is64BitWindows && !Is64BitProcess;
            }
        }

        public static void GetCpuInfo(out int phyProc, out int cores, out int logProc, ref LogicalProcessorInfo[] aProcessorList)
        {
            int pCores = 0;
            int pLPs = 0;
            int uiCPUs = 0;
            if (!CheckForGPPExEntryPoint())
                uiCPUs = GetPhysicalProcessorCount(ref pCores, ref pLPs, ref aProcessorList);
            else
                uiCPUs = GetPhysicalProcessorCountForGT64LPs(ref pCores, ref pLPs, ref aProcessorList);
            phyProc = uiCPUs;
            cores = pCores;
            logProc = pLPs;
        }

        public static uint GetCurrentProcessorNumber(out ushort group)
        {
            group = 0;

            if (!CheckForGPPExEntryPoint())
            {
                return GetCurrentProcessorNumber();
            }
            else
            {
                _PROCESSOR_NUMBER pN = new _PROCESSOR_NUMBER();
                GetCurrentProcessorNumberEx(ref pN);
                group = pN.Group;
                return pN.Number;
            }
        }

        public static void SetAffinity(LogicalProcessorInfo lpInfo)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //---------------------------------------------- 
            const uint DUPLICATE_SAME_ACCESS = 0x00000002;
            IntPtr CP = Process.GetCurrentProcess().Handle;
            IntPtr hThread = IntPtr.Zero;
            if (DuplicateHandle(CP, GetCurrentThread(),
            CP, out hThread, DUPLICATE_SAME_ACCESS, false,
            DUPLICATE_SAME_ACCESS))
            {
                if (!CheckForGPPExEntryPoint())
                {
                    UIntPtr Foo = SetThreadAffinityMask(hThread,
                    new UIntPtr((ulong)Math.Pow(2, lpInfo.AbsoluteProcessor)));
                }
                else
                {
                    _GROUP_AFFINITY PreviousGA = new _GROUP_AFFINITY();
                    _GROUP_AFFINITY pGA = new _GROUP_AFFINITY();
                    pGA.Group = lpInfo.GroupNumber;
                    pGA.Mask = lpInfo.ProcessorMask;
                    pGA.Reserved = new ushort[3];
                    PreviousGA.Reserved = new ushort[3];
                    bool rv = false;
                    rv = SetThreadGroupAffinity(hThread, ref pGA, ref PreviousGA);
                }
            }
            bool rv1 = CloseHandle(hThread);
        }
        #endregion

        #region Interop Structures
        private const int ERROR_INSUFFICIENT_BUFFER = 122;

        private enum _LOGICAL_PROCESSOR_RELATIONSHIP
        {
            RelationProcessorCore,
            RelationNumaNode,
            RelationCache,
            RelationProcessorPackage,
            RelationGroup,
            RelationAll = 0xffff
        }
        private enum _PROCESSOR_CACHE_TYPE
        {
            CacheUnified,
            CacheInstruction,
            CacheData,
            CacheTrace
        }
        private enum RelationProcessorCore
        {
            RelationProcessorCore = 0,
            RelationNumaNode = 1,
            RelationCache = 2,
            RelationProcessorPackage = 3
        }
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct _GROUP_AFFINITY
        {
            public UIntPtr Mask;
            [MarshalAs(UnmanagedType.U2)]
            public ushort Group;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3, ArraySubType = UnmanagedType.U2)]
            public ushort[] Reserved;
        }
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct _NUMA_NODE_RELATIONSHIP
        {
            [MarshalAs(UnmanagedType.U4)]
            public uint NodeNumber;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 20, ArraySubType = UnmanagedType.U1)]
            public byte[] Reserved;
            [MarshalAs(UnmanagedType.Struct)]
            public _GROUP_AFFINITY GroupMask;
        }
        private const int ANYSIZE_ARRAY = 1;
        private const int ALL_PROCESSOR_GROUPS = 0xffff;
        private const int MAXIMUM_PROC_PER_GROUP = 64;
        private const int MAXIMUM_PROCESSORS = MAXIMUM_PROC_PER_GROUP;
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct _PROCESSOR_RELATIONSHIP
        {
            [MarshalAs(UnmanagedType.U1)]
            public byte Flags;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 21, ArraySubType = UnmanagedType.U1)]
            public byte[] Reserved;
            [MarshalAs(UnmanagedType.U2)]
            public ushort GroupCount;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = ANYSIZE_ARRAY, ArraySubType = UnmanagedType.Struct)]
            public _GROUP_AFFINITY[] GroupMask;
        }
        
        [StructLayout(LayoutKind.Explicit)]
        private struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P
        {
            [FieldOffset(0), MarshalAs(UnmanagedType.U4)]
            public _LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
            [FieldOffset(4), MarshalAs(UnmanagedType.U4)]
            public uint Size;
            [FieldOffset(8), MarshalAs(UnmanagedType.Struct)]
            public _PROCESSOR_RELATIONSHIP Processor;
        }

        [StructLayout(LayoutKind.Explicit)]
        private struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_N
        {
            [FieldOffset(0), MarshalAs(UnmanagedType.U4)]
            public _LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
            [FieldOffset(4), MarshalAs(UnmanagedType.U4)]
            public uint Size;
            [FieldOffset(8), MarshalAs(UnmanagedType.Struct)]
            public _NUMA_NODE_RELATIONSHIP Numa;
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct CACHE_DESCRIPTOR
        {
            public byte Level;
            public byte Associativity;
            public ushort LineSize;
            public uint Size;
            [MarshalAs(UnmanagedType.U4)]
            public _PROCESSOR_CACHE_TYPE Type;
        }
        [StructLayout(LayoutKind.Explicit)]
        private struct SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx86
        {
            [FieldOffset(0)]
            public UIntPtr ProcessorMask;
            [FieldOffset(4), MarshalAs(UnmanagedType.U4)]
            public RelationProcessorCore Relationship;
            [FieldOffset(8)]
            public byte Flags;
            [FieldOffset(8)]
            public CACHE_DESCRIPTOR Cache;
            [FieldOffset(8)]
            public UInt32 NodeNumber;
            [FieldOffset(8)]
            public UInt64 Reserved1;
            [FieldOffset(16)]
            public UInt64 Reserved2;
        }
        [StructLayout(LayoutKind.Explicit)]
        private struct SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx64
        {
            [FieldOffset(0)]
            public UIntPtr ProcessorMask;
            [FieldOffset(8), MarshalAs(UnmanagedType.U4)]
            public RelationProcessorCore Relationship;
            [FieldOffset(12)]
            public byte Flags;
            [FieldOffset(12)]
            public CACHE_DESCRIPTOR Cache;
            [FieldOffset(12)]
            public UInt32 NodeNumber;
            [FieldOffset(12)]
            public UInt64 Reserved1;
            [FieldOffset(20)]
            public UInt64 Reserved2;
        }
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct _PROCESSOR_NUMBER
        {
            [MarshalAs(UnmanagedType.U2)]
            public ushort Group;
            [MarshalAs(UnmanagedType.U1)]
            public byte Number;
            [MarshalAs(UnmanagedType.U1)]
            public byte Reserved;
        }
        #endregion

        #region Interop Methods
        [DllImport("kernel32", SetLastError = true)]
        private static extern bool GetNumaNodeProcessorMaskEx(
        [In] ushort Node, ref _GROUP_AFFINITY ProcessorMask);
        [DllImport("kernel32", SetLastError = true)]
        private static extern int GetMaximumProcessorCount([In] ushort GroupNumber);
        [DllImport("kernel32", SetLastError = true)]
        private static extern int GetActiveProcessorCount([In] ushort GroupNumber);
        [DllImport("kernel32", SetLastError = true)]
        private static extern ushort GetMaximumProcessorGroupCount();
        [DllImport("kernel32", SetLastError = true)]
        private static extern ushort GetActiveProcessorGroupCount();
        [DllImport("kernel32", SetLastError = true)]
        private static extern bool GetThreadGroupAffinity(IntPtr hThread,
        ref _GROUP_AFFINITY GroupAffinity);
        [DllImport("kernel32", SetLastError = true)]
        private static extern Boolean SetThreadGroupAffinity(IntPtr hThread,
        ref _GROUP_AFFINITY GroupAffinity,
        ref _GROUP_AFFINITY PreviousGroupAffinity);
        [DllImport("kernel32", SetLastError = true)]
        private static extern bool GetLogicalProcessorInformationEx(
        [In] _LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType,
        [Out] _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P[] Buffer,
        ref uint ReturnedLength);
        [DllImport("kernel32", SetLastError = true)]
        private static extern bool GetLogicalProcessorInformationEx(
        [In] _LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType,
        [Out] _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_N[] Buffer,
        ref uint ReturnedLength);
        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool GetLogicalProcessorInformation(
        [Out] SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx86[] infos,
        ref uint infoSize);
        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool GetLogicalProcessorInformation(
        [Out] SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx64[] infos,
        ref uint infoSize);
        [DllImport("kernel32", SetLastError = true)]
        private static extern IntPtr GetCurrentThread();
        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool DuplicateHandle(IntPtr hSourceProcessHandle,
        IntPtr hSourceHandle, IntPtr hTargetProcessHandle, out IntPtr lpTargetHandle,
        uint dwDesiredAccess, [MarshalAs(UnmanagedType.Bool)] bool bInheritHandle, uint dwOptions);
        [DllImport("kernel32", SetLastError = true)]
        private static extern UIntPtr SetThreadAffinityMask(IntPtr hThread,
        UIntPtr dwThreadAffinityMask);
        [DllImport("kernel32", SetLastError = true)]
        private static extern uint GetCurrentProcessorNumber();
        [DllImport("kernel32", SetLastError = true)]
        private static extern uint GetCurrentProcessorNumberEx(
        ref _PROCESSOR_NUMBER ProcNumber);
        [DllImport("kernel32", SetLastError = true)]
        private static extern bool CloseHandle([In] IntPtr hObject);
        #endregion

        #region Private Classes
        private class ProcessorInfo
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            // Variables
            //----------------------------------------------
            private RelationProcessorCore _Relationship;
            private byte _Flags;
            private UIntPtr _ProcessorMask;
            //----------------------------------------------
            // Constructor
            //----------------------------------------------
            public ProcessorInfo(RelationProcessorCore relationShip,
            byte flags, UIntPtr processorMask)
            {
                //------------------------------------------ 
                // Intel Corporation 
                // Copyright © 2009/2010 - All Rights Reserved 
                // Department : SST/NTE 
                // Written by : Bill Hines - william.l.hines@intel.com 
                // Modified by : N/A 
                // Date : 12/4/2009 
                //------------------------------------------
                _Relationship = relationShip;
                _Flags = flags;
                _ProcessorMask = processorMask;
            }
            //----------------------------------------------
            // Properties
            //----------------------------------------------
            public RelationProcessorCore Relationship
            {
                //------------------------------------------ 
                // Intel Corporation 
                // Copyright © 2009/2010 - All Rights Reserved 
                // Department : SST/NTE 
                // Written by : Bill Hines - william.l.hines@intel.com 
                // Modified by : N/A 
                // Date : 12/4/2009 
                //------------------------------------------
                get
                {
                    return _Relationship;
                }
            }
            public byte Flags
            {
                //------------------------------------------ 
                // Intel Corporation 
                // Copyright © 2009/2010 - All Rights Reserved 
                // Department : SST/NTE 
                // Written by : Bill Hines - william.l.hines@intel.com 
                // Modified by : N/A 
                // Date : 12/4/2009 
                //------------------------------------------
                get
                {
                    return _Flags;
                }
            }
            public UIntPtr ProcessorMask
            {
                //------------------------------------------ 
                // Intel Corporation 
                // Copyright © 2009/2010 - All Rights Reserved 
                // Department : SST/NTE 
                // Written by : Bill Hines - william.l.hines@intel.com 
                // Modified by : N/A 
                // Date : 12/4/2009 
                //------------------------------------------
                get
                {
                    return _ProcessorMask;
                }
            }
        }
        #endregion

        #region Private Methods
        private static bool CheckForGPPExEntryPoint()
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            bool rvReturn = false;
            try
            {
                uint returnLength = 0;
                _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P[] oDummy = null;
                bool bResult = GetLogicalProcessorInformationEx(
                _LOGICAL_PROCESSOR_RELATIONSHIP.RelationAll, oDummy, ref returnLength);
                rvReturn = true;
            }
            catch (DllNotFoundException)
            {
                rvReturn = false;
            }
            catch (EntryPointNotFoundException)
            {
                rvReturn = false;
            }
            return rvReturn;
        }

        private static bool GetLogicalProcessorInformationExWrapper(_LOGICAL_PROCESSOR_RELATIONSHIP _LOGICAL_PROCESSOR_RELATIONSHIP, _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P[] oDummy, ref uint returnLength)
        {
            bool ret = true;

#if false // DEBUG
            const int PKG_COUNT = 2;
            const int CORES_PER_PKG = 18;
            const int THREADS_PER_CORE = 2;
            switch (_LOGICAL_PROCESSOR_RELATIONSHIP)
            {
                case ProcessorQueryAndControl._LOGICAL_PROCESSOR_RELATIONSHIP.RelationProcessorPackage:
                    {
                        Func<ushort, ulong, _PROCESSOR_RELATIONSHIP> CreateProcessor = (group, mask) =>
                            {
                                _PROCESSOR_RELATIONSHIP p = new _PROCESSOR_RELATIONSHIP();
                                p.Flags = 0;
                                p.GroupCount = 1;
                                p.GroupMask = new _GROUP_AFFINITY[1];
                                p.GroupMask[0].Mask = new UIntPtr(mask);
                                p.GroupMask[0].Group = group;
                                return p;
                            };

                        if (oDummy != null && oDummy.Length >= PKG_COUNT)
                        {
                            oDummy[0].Relationship = _LOGICAL_PROCESSOR_RELATIONSHIP;
                            //oDummy[0].Processor = CreateProcessor(0, 0xFFFF);
                            oDummy[0].Processor = CreateProcessor(0, 0xFFFFFFFFF);
                            oDummy[0].Size = (uint)Marshal.SizeOf(typeof(_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P));

                            oDummy[1].Relationship = _LOGICAL_PROCESSOR_RELATIONSHIP;
                            //oDummy[1].Processor = CreateProcessor(0, 0xFFFF0000);
                            oDummy[1].Processor = CreateProcessor(1, 0xFFFFFFFFF);
                            oDummy[1].Size = (uint)Marshal.SizeOf(typeof(_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P));

                            /*
                            oDummy[2].Relationship = _LOGICAL_PROCESSOR_RELATIONSHIP;
                            //oDummy[2].Processor = CreateProcessor(0, 0xFFFF00000000);
                            oDummy[2].Processor = CreateProcessor(1, 0x3FFFFFFF);
                            oDummy[2].Size = (uint)Marshal.SizeOf(typeof(_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P));

                            oDummy[3].Relationship = _LOGICAL_PROCESSOR_RELATIONSHIP;
                            //oDummy[3].Processor = CreateProcessor(0, 0xFFFF000000000000);
                            oDummy[3].Processor = CreateProcessor(1, 0xFFFFFFFC00000000);
                            oDummy[3].Size = (uint)Marshal.SizeOf(typeof(_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P));
                            */
                            returnLength = (uint)(oDummy.Length * Marshal.SizeOf(typeof(_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P)));
                            ret = true;
                        }
                        else
                        {
                            returnLength = (uint)(PKG_COUNT * Marshal.SizeOf(typeof(_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P)));
                            ret = false;
                        }
                    }
                    break;
                case ProcessorQueryAndControl._LOGICAL_PROCESSOR_RELATIONSHIP.RelationProcessorCore:
                    {
                        //oDummy = new _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P[PKG_COUNT * CORES_PER_PKG];

                        Func<ushort, ulong, _PROCESSOR_RELATIONSHIP> CreateCore = (group, mask) =>
                        {
                            _PROCESSOR_RELATIONSHIP p = new _PROCESSOR_RELATIONSHIP();
                            p.Flags = 1;
                            p.GroupCount = 1;
                            p.GroupMask = new _GROUP_AFFINITY[1];
                            p.GroupMask[0].Mask = new UIntPtr(mask);
                            p.GroupMask[0].Group = group;
                            return p;
                        };

                        if (oDummy != null && oDummy.Length >= (PKG_COUNT * CORES_PER_PKG))
                        {
                            for (int i = 0; i < (2 * CORES_PER_PKG); i++)
                            {
                                oDummy[i].Relationship = _LOGICAL_PROCESSOR_RELATIONSHIP;
                                oDummy[i].Processor = CreateCore((ushort)((i * 2) / 64), (0x3UL << ((i * 2) % 64)));
                                oDummy[i].Size = (uint)Marshal.SizeOf(typeof(_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P));
                            }

                            for (int i = 0; i < (2 * CORES_PER_PKG); i++)
                            {
                                oDummy[35 - i].Relationship = _LOGICAL_PROCESSOR_RELATIONSHIP;
                                oDummy[35 - i].Processor = CreateCore((ushort)((i * 2) / 64), (0xC000000000000000UL >> ((i * 2) % 64)));
                                oDummy[35 - i].Size = (uint)Marshal.SizeOf(typeof(_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P));
                            }

                            returnLength = (uint)(oDummy.Length * Marshal.SizeOf(typeof(_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P)));
                            ret = true;
                        }
                        else
                        {
                            returnLength = (uint)(PKG_COUNT * CORES_PER_PKG * Marshal.SizeOf(typeof(_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P)));
                            ret = false;
                        }
                    }
                    break;
                default:
                    throw new ArgumentException();
            }

            
#else
            ret = GetLogicalProcessorInformationEx(_LOGICAL_PROCESSOR_RELATIONSHIP, oDummy, ref returnLength);
#endif
            try
            {
                if (oDummy != null)
                {
                    int index = 0;
                    foreach (_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P s in oDummy)
                    {
                        ErrorLog.WriteLine(_LOGICAL_PROCESSOR_RELATIONSHIP.ToString() + " - " + index++ + ": ");
                        _PROCESSOR_RELATIONSHIP pr = s.Processor;
                        _LOGICAL_PROCESSOR_RELATIONSHIP lpr = s.Relationship;
                        ErrorLog.WriteLine("  Size: " + s.Size);
                        ErrorLog.WriteLine("  Flags: " + pr.Flags);
                        ErrorLog.WriteLine("  GroupCount: " + pr.GroupCount);
                        foreach (_GROUP_AFFINITY ga in pr.GroupMask)
                        {
                            ErrorLog.WriteLine("  GroupMask: ");
                            ErrorLog.WriteLine("    Group: " + ga.Group.ToString("X2"));
                            ErrorLog.WriteLine("    Mask: " + ((ulong)ga.Mask).ToString("X2"));
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            return ret;
        }
        
        private static uint CountNumberOfBitsSetInMask(UIntPtr bitMask)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            uint LSHIFT = (uint)((UIntPtr.Size * 8) - 1);
            uint bitSetCount = 0;
            UIntPtr bitTest = (UIntPtr)Math.Pow(2, (int)LSHIFT);
            for (uint i = 0; i <= LSHIFT; ++i)
            {
                bitSetCount +=
                (uint)(Convert.ToBoolean(bitMask.ToUInt64() & bitTest.ToUInt64()) ? 1 : 0);
                bitTest = (UIntPtr)(bitTest.ToUInt64() / 2);
            }
            return bitSetCount;
        }

        
        private static Exception Fail(params string[] data)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            return new NotSupportedException(
            "GetPhysicalProcessorCount unexpectedly failed " +
            "(" + String.Join(", ", data) + ")");
        }
        private static List<ProcessorInfo> GetProcessorInfo86()
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            // First we're going to execute GetLogicalProcessorInformation 
            // once to make sure that we determine the size of the data 
            // that it is going to return.
            // This call should fail with error ERROR_INSUFFICIENT_BUFFER.
            //----------------------------------------------
            uint iReturnLength = 0;
            SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx86[] oDummy = null;
            bool bResult = GetLogicalProcessorInformation(oDummy,
            ref iReturnLength);
            if (bResult)
            {
                throw Fail("GetLogicalProcessorInformation failed.", "x86");
            }
            //----------------------------------------------
            // Making sure that the error code that we got back isn't that 
            // there is insufficient space in the buffer.
            //----------------------------------------------
            int iError = Marshal.GetLastWin32Error();
            if (iError != ERROR_INSUFFICIENT_BUFFER)
            {
                throw Fail(
                "Insufficient space in the buffer.",
                "x86", iError.ToString());
            }
            //----------------------------------------------
            // Now that we know how much space we should reserve, 
            // we're going to reserve it and call 
            // GetLogicalProcessorInformation again.
            //----------------------------------------------
            uint iBaseSize = (uint)Marshal.SizeOf(
            typeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx86));
            uint iNumberOfElements = iReturnLength / iBaseSize;
            SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx86[] oData =
            new SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx86[iNumberOfElements];
            uint iAllocatedSize = iNumberOfElements * iBaseSize;
            if (!GetLogicalProcessorInformation(oData, ref iAllocatedSize))
            {
                throw Fail(
                "GetLogicalProcessorInformation failed",
                "x86", Marshal.GetLastWin32Error().ToString());
            }
            //----------------------------------------------
            // Convert data to a list that can be interpreted.
            //----------------------------------------------
            List<ProcessorInfo> oList = new List<ProcessorInfo>();
            foreach (SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx86 oInfo in oData)
            {
                oList.Add(new ProcessorInfo(oInfo.Relationship,
                oInfo.Flags, oInfo.ProcessorMask));
            }
            return oList;
        }
        private static List<ProcessorInfo> GetProcessorInfo64()
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            // First we're going to execute GetLogicalProcessorInformation 
            // once to make sure that we determine the size of the data 
            // that it is going to return.
            // This call should fail with error ERROR_INSUFFICIENT_BUFFER.
            //----------------------------------------------
            uint iReturnLength = 0;
            SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx64[] oDummy = null;
            bool bResult = GetLogicalProcessorInformation(oDummy,
            ref iReturnLength);
            if (bResult)
            {
                throw Fail("GetLogicalProcessorInformation failed.", "x64");
            }
            //----------------------------------------------
            // Making sure that the error code that we got back is not  
            // that there is in sufficient space in the buffer.
            //----------------------------------------------
            int iError = Marshal.GetLastWin32Error();
            if (iError != ERROR_INSUFFICIENT_BUFFER)
            {
                throw Fail(
                "Insufficient space in the buffer.",
                "x64", iError.ToString());
            }
            //----------------------------------------------
            // Now that we know how much space we should reserve, 
            // we're going to reserve it and call 
            // GetLogicalProcessorInformation again.
            //----------------------------------------------
            uint iBaseSize = (uint)Marshal.SizeOf(
            typeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx64));
            uint iNumberOfElements = iReturnLength / iBaseSize;
            SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx64[] oData =
            new SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx64[iNumberOfElements];
            uint iAllocatedSize = iNumberOfElements * iBaseSize;
            if (!GetLogicalProcessorInformation(oData, ref iAllocatedSize))
            {
                throw Fail("GetLogicalProcessorInformation failed",
                "x64", Marshal.GetLastWin32Error().ToString());
            }
            //----------------------------------------------
            // Convert data to a list that can be interpreted.
            //----------------------------------------------
            List<ProcessorInfo> oList = new List<ProcessorInfo>();
            foreach (SYSTEM_LOGICAL_PROCESSOR_INFORMATIONx64 oInfo in oData)
            {
                oList.Add(new ProcessorInfo(
                oInfo.Relationship,
                oInfo.Flags, oInfo.ProcessorMask));
            }
            return oList;
        }

        private static _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P[]
        GetProcessorDataArray(_LOGICAL_PROCESSOR_RELATIONSHIP aType)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            // Get Number of Elements that Exist 
            //----------------------------------------------
            uint returnLength = 0;
            _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P[] oDummy = null;
            bool bResult = GetLogicalProcessorInformationExWrapper(
            aType, oDummy, ref returnLength);
            if (bResult) return null;
            //----------------------------------------------
            // Making sure that the error code that we 
            // got back isn't that there is insufficient 
            // space in the buffer.
            //----------------------------------------------
            int iError = Marshal.GetLastWin32Error();
            if (iError != ERROR_INSUFFICIENT_BUFFER)
                return null;
            //----------------------------------------------
            // Now that we know how much space we should 
            // reserve, we're going to reserve it and call 
            // GetLogicalProcessorInformationEx again.
            //----------------------------------------------
            uint iBaseSize = (uint)Marshal.SizeOf(
            typeof(_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P));
            uint iNumberOfElements = returnLength / iBaseSize;
            _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P[] oData =
            new _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P[iNumberOfElements];
            uint iAllocatedSize = iNumberOfElements * iBaseSize;
            //----------------------------------------------
            // Get The Requested Data
            //----------------------------------------------
            if (!GetLogicalProcessorInformationExWrapper(
            aType, oData, ref iAllocatedSize))
                return null;
            else
                return oData;
        }
        private static _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_N[]
        GetNumaDataArray(_LOGICAL_PROCESSOR_RELATIONSHIP aType)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            // Get Number of Elements that Exist 
            //----------------------------------------------
            uint returnLength = 0;
            _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_N[] oDummy = null;
            bool bResult = GetLogicalProcessorInformationEx(
            aType, oDummy, ref returnLength);
            if (bResult) return null;
            //----------------------------------------------
            // Making sure that the error code that we 
            // got back isn't that there is insufficient 
            // space in the buffer.
            //----------------------------------------------
            int iError = Marshal.GetLastWin32Error();
            if (iError != ERROR_INSUFFICIENT_BUFFER)
                return null;
            //----------------------------------------------
            // Now that we know how much space we should 
            // reserve, we're going to reserve it and call 
            // GetLogicalProcessorInformationEx again.
            //----------------------------------------------
            uint iBaseSize = (uint)Marshal.SizeOf(
            typeof(_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_N));
            uint iNumberOfElements = returnLength / iBaseSize;
            _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_N[] oData =
            new _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_N[iNumberOfElements];
            uint iAllocatedSize = iNumberOfElements * iBaseSize;
            //----------------------------------------------
            // Get The Requested Data
            //----------------------------------------------
            if (!GetLogicalProcessorInformationEx(
            aType, oData, ref iAllocatedSize))
                return null;
            else
                return oData;
        }
        private static string GetCacheType(_PROCESSOR_CACHE_TYPE value)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            switch (value)
            {
                case _PROCESSOR_CACHE_TYPE.CacheUnified: return "CacheUnified";
                case _PROCESSOR_CACHE_TYPE.CacheInstruction: return "CacheInstruction";
                case _PROCESSOR_CACHE_TYPE.CacheData: return "CacheData";
                case _PROCESSOR_CACHE_TYPE.CacheTrace: return "CacheTrace";
                default: return "unknown";
            }
        }

        private static int GetPhysicalProcessorCount(ref int aCores, ref int aLPs,
        ref LogicalProcessorInfo[] aProcessorList)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            if (!Is64BitProcess)
            {
                Version oVersion = Environment.OSVersion.Version;
                if (oVersion < new Version(5, 1, 2600))
                {
                    throw new NotSupportedException(
                    "GetPhysicalProcessorCount is not supported " +
                    "on this operating system.");
                }
                else if (oVersion.Major == 5 && oVersion.Minor == 1 &&
                !Environment.OSVersion.ServicePack.Equals(
                  "Service Pack 3", StringComparison.OrdinalIgnoreCase))
                {
                    throw new NotSupportedException(
                    "GetPhysicalProcessorCount is not supported " +
                    "on this operating system.");
                }
            }
            //----------------------------------------------
            // Getting a list of processor information
            //----------------------------------------------
            List<ProcessorInfo> oList;
            if (Is64BitProcess)
                oList = GetProcessorInfo64();
            else
                oList = GetProcessorInfo86();
            //----------------------------------------------
            // The list will basically contain something like this at this point:
            //
            // E.g. for a 2 x single core
            // Relationship              Flags      ProcessorMask
            //----------------------------------------------
            // RelationProcessorCore     0          1
            // RelationProcessorCore     0          2
            // RelationNumaNode          0          3
            //
            // E.g. for a 2 x dual core
            // Relationship              Flags      ProcessorMask
            //----------------------------------------------
            // RelationProcessorCore     1          5
            // RelationProcessorCore     1          10
            // RelationNumaNode          0          15
            //
            // E.g. for a 1 x quad core
            // Relationship              Flags      ProcessorMask
            //----------------------------------------------
            // RelationProcessorCore     1          15
            // RelationNumaNode          0          15
            //
            // E.g. for a 1 x dual core
            // Relationship              Flags      ProcessorMask  
            //----------------------------------------------
            // RelationProcessorCore     0          1              
            // RelationCache             1          1              
            // RelationCache             1          1              
            // RelationProcessorPackage  0          3              
            // RelationProcessorCore     0          2              
            // RelationCache             1          2              
            // RelationCache             1          2              
            // RelationCache             2          3              
            // RelationNumaNode          0          3
            // 
            // Vista or higher will return one RelationProcessorPackage 
            // line per socket. On other operating systems we need to 
            // interpret the RelationProcessorCore lines.
            //
            // More information:
            // http://msdn2.microsoft.com/en-us/library/ms683194(VS.85).aspx
            // http://msdn2.microsoft.com/en-us/library/ms686694(VS.85).aspx
            //----------------------------------------------
            // Count the number of RelationProcessorPackage items
            //----------------------------------------------
            int iCount = 0;
            int logicalProcessorCount = 0;
            int coreCount = 0;
            foreach (ProcessorInfo oItem in oList)
            {
                if (oItem.Relationship ==
                RelationProcessorCore.RelationProcessorPackage)
                {
                    iCount++;
                }
                else if (oItem.Relationship ==
                RelationProcessorCore.RelationProcessorCore)
                {
                    coreCount++;
                    logicalProcessorCount += (int)CountNumberOfBitsSetInMask(oItem.ProcessorMask);
                }
            }
            if (iCount > 0)
            {
                aCores = coreCount;
                aLPs = logicalProcessorCount;
                aProcessorList = new LogicalProcessorInfo[aLPs];
                for (int i = 0; i < aLPs; i++)
                {
                    aProcessorList[i] = new LogicalProcessorInfo(
                    (ushort)0, (byte)(i + 0), (byte)(i + 0), (UIntPtr)Math.Pow(2, i));
                }
                return iCount;
            }
            //----------------------------------------------
            // Use the information in RelationProcessorCore.
            //----------------------------------------------
            iCount = 0;
            logicalProcessorCount = 0;
            coreCount = 0;
            foreach (ProcessorInfo oItem in oList)
            {
                if (oItem.Relationship == RelationProcessorCore.RelationProcessorCore)
                {
                    iCount++;
                    coreCount++;
                    logicalProcessorCount += (int)CountNumberOfBitsSetInMask(oItem.ProcessorMask);
                }
            }
            if (iCount > 0)
            {
                aCores = coreCount;
                aLPs = logicalProcessorCount;
                aProcessorList = new LogicalProcessorInfo[aLPs];
                for (int i = 0; i < aLPs; i++)
                {
                    aProcessorList[i] = new LogicalProcessorInfo(
                    (ushort)0, (byte)(i + 0), (byte)(i + 0), (UIntPtr)Math.Pow(2, i));
                }
                return iCount;
            }
            throw Fail("No cpus have been detected.");
        }
        private static int GetPhysicalProcessorCountForGT64LPs(ref int aCores, ref int aLPs,
        ref LogicalProcessorInfo[] aProcessorList)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            bool bNuma = false;
            int piPhysicalProcessors = 0;
            aCores = 0;
            aLPs = 0;
            if (GetCountsForGT64LPs(_LOGICAL_PROCESSOR_RELATIONSHIP.RelationProcessorPackage,
            ref piPhysicalProcessors, ref aProcessorList))
            {
                if (!bNuma)
                {
                    if (GetCountsForGT64LPs(_LOGICAL_PROCESSOR_RELATIONSHIP.RelationProcessorCore,
                    ref aCores, ref aProcessorList))
                    {
                        aLPs = aProcessorList.Length;
                        return piPhysicalProcessors;
                    }
                    else
                        return 0;
                }
                else
                {
                    if (GetCountsForGT64LPs(_LOGICAL_PROCESSOR_RELATIONSHIP.RelationNumaNode,
                    ref aCores, ref aProcessorList))
                    {
                        aLPs = aProcessorList.Length;
                        return piPhysicalProcessors;
                    }
                    else
                        return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        private static bool GetCountsForGT64LPs(_LOGICAL_PROCESSOR_RELATIONSHIP aType, ref int aCount,
        ref LogicalProcessorInfo[] aProcessorList)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            bool rvReturnValue = false;
            switch (aType)
            {
                case _LOGICAL_PROCESSOR_RELATIONSHIP.RelationProcessorPackage:
                    _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P[] oDataPPs = GetProcessorDataArray(aType);
                    aCount = 0;
                    if (oDataPPs != null)
                        aCount = oDataPPs.Length;
                    if (oDataPPs != null)
                    {
                        List<LogicalProcessorInfo> oList = new List<LogicalProcessorInfo>();
                        ushort CurrentGroup = oDataPPs[0].Processor.GroupMask[0].Group;
                        byte AbsoluteProcessorNumber = 0;
                        byte[] GroupProcessorNumberArray = new byte[NUM_PROCESSOR_GROUPS];
                        for (int i = 0; i < NUM_PROCESSOR_GROUPS; i++)
                            GroupProcessorNumberArray[i] = 0;
                        foreach (_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P LPInfo in oDataPPs)
                        {
                            ushort Group = LPInfo.Processor.GroupMask[0].Group;
                            UIntPtr GroupMask = LPInfo.Processor.GroupMask[0].Mask;
                            ulong lg = 1;
                            uint LSHIFT = (uint)((UIntPtr.Size * 8) - 1);

                            for (int i = 0; i <= LSHIFT; i++)
                            {
                                if (((ulong)GroupMask & (ulong)(lg << i)) != 0)
                                {
                                    UIntPtr LPMask = (UIntPtr)((lg << i));
                                    oList.Add(new LogicalProcessorInfo(
                                    Group, GroupProcessorNumberArray[Group], AbsoluteProcessorNumber, LPMask));
                                    GroupProcessorNumberArray[Group]++;
                                    AbsoluteProcessorNumber++;
                                }
                            }
                        }
                        aProcessorList = oList.ToArray();
                        rvReturnValue = true;
                    }
                    else
                        rvReturnValue = false;
                    break;
                case _LOGICAL_PROCESSOR_RELATIONSHIP.RelationProcessorCore:
                    _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P[] oDataCores = GetProcessorDataArray(aType);
                    aCount = 0;
                    if (oDataCores != null)
                    {
                        aCount = oDataCores.Length;
                        rvReturnValue = true;
                    }
                    else
                        rvReturnValue = false;
                    break;
                case _LOGICAL_PROCESSOR_RELATIONSHIP.RelationNumaNode:
                    _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_P[] oDataNumaCores =
                    GetProcessorDataArray(_LOGICAL_PROCESSOR_RELATIONSHIP.RelationProcessorCore);
                    _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_N[] oDataNuma = GetNumaDataArray(aType);
                    aCount = 0;
                    if ((oDataNumaCores != null) && (oDataNuma != null))
                    {
                        aCount = oDataNumaCores.Length;
                        byte[] NumaNodeCounts = new byte[oDataNuma.Length];
                        byte[] NumaIndexOffset = new byte[oDataNuma.Length];
                        for (int i = 0; i < NumaNodeCounts.Length; i++)
                        {
                            byte LPsInNode = (byte)CountNumberOfBitsSetInMask(oDataNuma[i].Numa.GroupMask.Mask);
                            NumaNodeCounts[i] += LPsInNode;
                        }
                        for (int i = 1; i < NumaIndexOffset.Length; i++)
                            NumaIndexOffset[i] += NumaNodeCounts[i - 1];
                        List<LogicalProcessorInfo> oList = new List<LogicalProcessorInfo>();
                        ushort CurrentGroup = oDataNuma[0].Numa.GroupMask.Group;

                        byte AbsoluteProcessorNumber = 0;
                        byte[] GroupProcessorNumberArray = new byte[NUM_PROCESSOR_GROUPS];
                        for (int i = 0; i < NUM_PROCESSOR_GROUPS; i++)
                            GroupProcessorNumberArray[i] = 0;
                        foreach (_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_N numaInfo in oDataNuma)
                        {
                            ushort Group = numaInfo.Numa.GroupMask.Group;
                            UIntPtr GroupMask = numaInfo.Numa.GroupMask.Mask;
                            ulong lg = 1;
                            uint LSHIFT = (uint)((UIntPtr.Size * 8) - 1);
                            for (int i = 0; i <= LSHIFT; i++)
                            {
                                if (((ulong)GroupMask & (ulong)(lg << i)) != 0)
                                {
                                    UIntPtr LPMask = (UIntPtr)((lg << i));
                                    oList.Insert((NumaIndexOffset[Group] + AbsoluteProcessorNumber),
                                    new LogicalProcessorInfo(
                                    Group,
                                    GroupProcessorNumberArray[Group],
                                    AbsoluteProcessorNumber,
                                    LPMask));
                                    GroupProcessorNumberArray[Group]++;
                                    AbsoluteProcessorNumber++;
                                }
                            }
                        }
                        aProcessorList = oList.ToArray();
                        rvReturnValue = true;
                    }
                    else
                        rvReturnValue = false;
                    break;
                default:
                    rvReturnValue = false;
                    break;
            }
            return rvReturnValue;
        }
        
        private static void ReadMPerfAPerf(ref ulong MPERF, ref ulong APERF)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
#if false
            uint pEAX0 = 0; uint pEDX0 = 0;
            uint pEAX1 = 0; uint pEDX1 = 0;
            Papi.ReadMSR(MSR_IA32_MPERF, ref pEAX0, ref pEDX0);
            MPERF = (long)(((long)pEDX0 << 32) | (long)pEAX0);
            Papi.ReadMSR(MSR_IA32_APERF, ref pEAX1, ref pEDX1);
            APERF = (long)(((long)pEDX1 << 32) | (long)pEAX1);
#endif
            Papi.ReadMPerfAPerf(CurrentProperties.MSR_IA32_MPERF,
                CurrentProperties.MSR_IA32_APERF,
                ref MPERF,
                ref APERF);
        }

        private static ulong CreateMask(uint aStart, uint aStop)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //---------------------------------------------- 
            ulong Mask = 0;
            for (uint i = aStart; i <= aStop; i++)
                Mask |= (ulong)(Math.Pow(2, i));
            return Mask;
        }
        #endregion

        #region Helper Methods
        public static ulong GetRegisterValue(uint msr)
        {
            uint LowValue = 0; uint HighValue = 0;
            if (Papi.ReadMSR(msr, ref LowValue, ref HighValue))
                return ((ulong)HighValue << 32) | LowValue;
            else
                return 0;
        }

        public static ulong GetRegisterValue(uint msr, BitRange range)
        {
            uint LowValue = 0; uint HighValue = 0;
            if (Papi.ReadMSR(msr, ref LowValue, ref HighValue))
            {
                ulong Value = (ulong)(((ulong)HighValue << 32) | (ulong)LowValue);
                return Convert.ToUInt64((Value & range.Mask) >> (int)(range.StartBit));
            }
            else
                return 0;
        }

        public static bool GetRegisterValue(uint msr, int bit)
        {
            return (GetRegisterValue(msr) & (ulong)(1 << bit)) != 0;
        }

        public static ulong SetRegisterValue(uint msr, BitRange range, ulong value)
        {
            ulong regValue = GetRegisterValue(msr);
            regValue &= ~range.Mask;
            regValue |= (value << range.StartBit);

            uint LowValue = 0; uint HighValue = 0;
            LowValue = (uint)(regValue & 0xFFFFFFFF);
            HighValue = (uint)((regValue & 0xFFFFFFFF00000000) >> 32);
            if (Papi.WriteMSR(msr, ref LowValue, ref HighValue))
                return ((ulong)HighValue << 32) | LowValue;
            else
                return 0;
        }

        public static bool SetRegisterValue(uint msr, int bit, bool val)
        {
            return SetRegisterValue(msr, BitRange.FromString(bit.ToString() + ":" + bit.ToString()), (ulong)((val == true) ? 1 : 0)) != 0;
        }

        public static bool GetTurboModeDisable(out bool disable)
        {

            ulong mask = CreateMask((uint)CurrentProperties.MSR_IA32_MISC_ENABLE_TURBO_MODE_DISABLE_BIT, (uint)CurrentProperties.MSR_IA32_MISC_ENABLE_TURBO_MODE_DISABLE_BIT);

            uint LowValue = 0; uint HighValue = 0;
            if (Papi.ReadMSR(CurrentProperties.MSR_IA32_MISC_ENABLE, ref LowValue, ref HighValue))
            {
                ulong Value = ((ulong)HighValue << 32) | LowValue;
                disable = ((Value & mask) >> CurrentProperties.MSR_IA32_MISC_ENABLE_TURBO_MODE_DISABLE_BIT) == 1;
                return true;
            }
            else
            {
                disable = false;
                return false;
            }
        }

        public static bool SetTurboModeDisable(bool disable)
        {
            uint LowValue = 0;
            uint HighValue = 0;
            if (Papi.ReadMSR(CurrentProperties.MSR_IA32_MISC_ENABLE, ref LowValue, ref HighValue))
            {
                ulong TurboBoostControlValue =
                (ulong)(((ulong)HighValue << 32) | (ulong)LowValue);
                ulong Mask = CreateMask((uint)CurrentProperties.MSR_IA32_MISC_ENABLE_TURBO_MODE_DISABLE_BIT, (uint)CurrentProperties.MSR_IA32_MISC_ENABLE_TURBO_MODE_DISABLE_BIT);
                if (disable) // disable is set bit to 1
                    TurboBoostControlValue |= (ulong)Mask;
                else
                    TurboBoostControlValue &= ~(ulong)Mask;

                LowValue = Convert.ToUInt32(TurboBoostControlValue & 0xffffffff);
                HighValue = Convert.ToUInt32((TurboBoostControlValue >> 32) & 0xffffffff);
                if (Papi.WriteMSR(CurrentProperties.MSR_IA32_MISC_ENABLE, ref LowValue, ref HighValue))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        public static bool GetCurrentPState(out int PState, out int NumPStates)
        {
            int maxNonTurboRatio = GetMaxNonTurboRatio();
            int minNodeTurboRatio = GetMinNonTurboRatio();

            NumPStates = (maxNonTurboRatio - minNodeTurboRatio) + 2;

            ulong mask = CurrentProperties.MSR_IA32_PERF_STATUS_PSTATE_VALUE.Mask;

            uint LowValue = 0; uint HighValue = 0;
            if (Papi.ReadMSR(CurrentProperties.MSR_IA32_PERF_STATUS, ref LowValue, ref HighValue))
            {
                int Actual = (int)((LowValue & mask) >> CurrentProperties.MSR_IA32_PERF_STATUS_PSTATE_VALUE.StartBit);
                if (Actual >= (maxNonTurboRatio + 1))
                    PState = 0;
                else
                    PState = ((maxNonTurboRatio + 1) - Actual);

                return true;
            }
            else
            {
                PState = 0;
                return false;
            }
        }

        public static bool GetTurboBoostStatus(out bool Enabled, out double Frequency, out int PState, out int NumPStates)
        {
            int maxNonTurboRatio = GetMaxNonTurboRatio();
            int minNodeTurboRatio = GetMinNonTurboRatio();

            NumPStates = (maxNonTurboRatio - minNodeTurboRatio) + 2;

            ulong mask = CurrentProperties.MSR_IA32_PERF_STATUS_PSTATE_VALUE.Mask;

            uint LowValue = 0; uint HighValue = 0;
            if (Papi.ReadMSR(CurrentProperties.MSR_IA32_PERF_STATUS, ref LowValue, ref HighValue))
            {
                int Actual = (int)((LowValue & mask) >> CurrentProperties.MSR_IA32_PERF_STATUS_PSTATE_VALUE.StartBit);
                Frequency = GetCurrentFrequency(maxNonTurboRatio);
                if (Actual >= (maxNonTurboRatio + 1))
                {
                    Enabled = true;
                    PState = 0;
                }
                else
                {
                    //Frequency = Actual * PropertiesTurboBoost.Instance.BaseCoreClockSpeed;
                    Enabled = false;
                    PState = ((maxNonTurboRatio + 1) - Actual);
                }

                return true;
            }
            else
            {
                Enabled = false;
                Frequency = 0;
                PState = 0;
                return false;
            }
        }

        public static int GetMaxNonTurboRatio()
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //---------------------------------------------- 
            uint LowValue = 0;
            uint HighValue = 0;
            if (Papi.ReadMSR(CurrentProperties.MSR_PLATFORM_INFO, ref LowValue, ref HighValue))
            {
                ulong Value = (ulong)(((ulong)HighValue << 32) | (ulong)LowValue);
                return Convert.ToInt32((Value & CurrentProperties.MSR_PLATFORM_INFO_MAX_NON_TURBO_RATIO.Mask) >>
                    (int)(CurrentProperties.MSR_PLATFORM_INFO_MAX_NON_TURBO_RATIO.StartBit));
            }
            else
            {
                return 0;
            }
        }

        public static int GetMinNonTurboRatio()
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //---------------------------------------------- 
            uint LowValue = 0;
            uint HighValue = 0;
            if (Papi.ReadMSR(CurrentProperties.MSR_PLATFORM_INFO, ref LowValue, ref HighValue))
            {
                ulong Value = (ulong)(((ulong)HighValue << 32) | (ulong)LowValue);
                return Convert.ToInt32((Value & CurrentProperties.MSR_PLATFORM_INFO_MAX_EFFICIENCY_RATIO.Mask) >>
                    (int)(CurrentProperties.MSR_PLATFORM_INFO_MAX_EFFICIENCY_RATIO.StartBit));
            }
            else
            {
                return 0;
            }
        }

        public static ulong GetTSC()
        {
            uint LowValue = 0;
            uint HighValue = 0;
            if (Papi.ReadMSR(0x10, ref LowValue, ref HighValue))
            {
                return (ulong)(((ulong)HighValue << 32) | (ulong)LowValue);
            }
            else
            {
                return 0;
            }
        }

        public static float GetPStateVoltage(uint msr, BitRange range)
        {
            //float fValue = (float)Value * (((float)1) / ((float)Math.Pow(2, 13)));
            uint LowValue = 0;
            uint HighValue = 0;
            if (Papi.ReadMSR(msr, ref LowValue, ref HighValue))
            {
                ulong Value = (ulong)(((ulong)HighValue << 32) | (ulong)LowValue);
                ulong val = Convert.ToUInt64((Value & range.Mask) >> (int)(range.StartBit));
                return (float)val * (((float)1) / ((float)Math.Pow(2, 13)));
            }
            else
            {
                return 0;
            }
        }

#if false // TODO: Fix
        public static bool GetRaplPowerUnits(out double powerUnits, out double energyUnits, out double timeUnits)
        {
            uint msr = PropertiesRAPL_RegLocations.Instance.PkgPowerUnit_Power_MSR;
            BitRange bitRange = PropertiesRAPL_RegLocations.Instance.PkgPowerLimit1_BitRange;

            powerUnits = 0; energyUnits = 0; timeUnits = 0;

            ulong Value1 = GetRegisterValue(PropertiesRAPL_RegLocations.Instance.PkgPowerUnit_Power_MSR, 
                PropertiesRAPL_RegLocations.Instance.PkgPowerUnit_Power_BitRange);
            powerUnits = ((double)1 / Math.Pow(2, Value1));

            ulong Value2 = GetRegisterValue(PropertiesRAPL_RegLocations.Instance.PkgPowerUnit_Energy_MSR,
                PropertiesRAPL_RegLocations.Instance.PkgPowerUnit_Energy_BitRange);
            energyUnits = ((double)1 / Math.Pow(2, Value2));

            ulong Value3 = GetRegisterValue(PropertiesRAPL_RegLocations.Instance.PkgPowerUnit_Time_MSR,
                PropertiesRAPL_RegLocations.Instance.PkgPowerUnit_Time_BitRange);
            timeUnits = ((double)1 / Math.Pow(2, Value3));

            return true;
        }

        public static PackagePowerLimit GetPackagePowerLimitValues(PackagePowerLimitIndex idx)
        {
            double pwr, erg, tme;
            GetRaplPowerUnits(out pwr, out erg, out tme);

            PackagePowerLimit pplv = new PackagePowerLimit();
            switch (idx)
            {
                case PackagePowerLimitIndex._1:
                    pplv.PowerLimit = GetPackagePowerLimit1_RegVal() * pwr;
                    pplv.TimeWindowForPowerLimit = DecodeTimeWindowForPowerLimit(GetTimeWindowPowerLimit1_RegVal(), tme);
                    pplv.EnablePowerLimit = GetRegisterValue(PropertiesRAPL_RegLocations.Instance.EnablePowerLimit1_MSR, PropertiesRAPL_RegLocations.Instance.EnablePowerLimit1_Bit);
                    pplv.PackageClampingLimitation = GetRegisterValue(PropertiesRAPL_RegLocations.Instance.PackageClampingLimit1_MSR, PropertiesRAPL_RegLocations.Instance.PackageClampingLimit1_Bit);
                    break;
                case PackagePowerLimitIndex._2:
                    pplv.PowerLimit = GetPackagePowerLimit2_RegVal() * pwr;
                    pplv.TimeWindowForPowerLimit = DecodeTimeWindowForPowerLimit(GetTimeWindowPowerLimit2_RegVal(), tme);
                    pplv.EnablePowerLimit = GetRegisterValue(PropertiesRAPL_RegLocations.Instance.EnablePowerLimit2_MSR, PropertiesRAPL_RegLocations.Instance.EnablePowerLimit2_Bit);
                    pplv.PackageClampingLimitation = GetRegisterValue(PropertiesRAPL_RegLocations.Instance.PackageClampingLimit2_MSR, PropertiesRAPL_RegLocations.Instance.PackageClampingLimit2_Bit);
                    break;
            }
            return pplv;
        }

        public static void SetPackagePowerLimitValues(PackagePowerLimitIndex idx, PackagePowerLimit ppl, PackagePowerLimitMask mask)
        {
            double pwr, erg, tme;
            GetRaplPowerUnits(out pwr, out erg, out tme);

            if ((mask & PackagePowerLimitMask.PowerLimit) == PackagePowerLimitMask.PowerLimit)
            {
                switch (idx)
                {
                    case PackagePowerLimitIndex._1:
                        SetRegisterValue(PropertiesRAPL_RegLocations.Instance.PkgPowerLimit1_MSR, PropertiesRAPL_RegLocations.Instance.PkgPowerLimit1_BitRange, (ulong)(ppl.PowerLimit / pwr));
                        break;
                    case PackagePowerLimitIndex._2:
                        SetRegisterValue(PropertiesRAPL_RegLocations.Instance.PkgPowerLimit2_MSR, PropertiesRAPL_RegLocations.Instance.PkgPowerLimit2_BitRange, (ulong)(ppl.PowerLimit / pwr));
                        break;
                }
            }

            if ((mask & PackagePowerLimitMask.TimeWindowForPowerLimit) == PackagePowerLimitMask.TimeWindowForPowerLimit)
            {
                ulong encodedTimeVal = EncodeTimeWindowForPowerLimit(ppl.TimeWindowForPowerLimit, tme);

                switch (idx)
                {
                    case PackagePowerLimitIndex._1:
                        SetRegisterValue(PropertiesRAPL_RegLocations.Instance.TimeWindowPowerLimit1_MSR, PropertiesRAPL_RegLocations.Instance.TimeWindowPowerLimit1_BitRange, encodedTimeVal);
                        break;
                    case PackagePowerLimitIndex._2:
                        SetRegisterValue(PropertiesRAPL_RegLocations.Instance.TimeWindowPowerLimit2_MSR, PropertiesRAPL_RegLocations.Instance.TimeWindowPowerLimit2_BitRange, encodedTimeVal);
                        break;
                }
            }

            if ((mask & PackagePowerLimitMask.EnablePowerLimit) == PackagePowerLimitMask.EnablePowerLimit)
            {
                switch (idx)
                {
                    case PackagePowerLimitIndex._1:
                        SetRegisterValue(PropertiesRAPL_RegLocations.Instance.EnablePowerLimit1_MSR, PropertiesRAPL_RegLocations.Instance.EnablePowerLimit1_Bit, ppl.EnablePowerLimit);
                        break;
                    case PackagePowerLimitIndex._2:
                        SetRegisterValue(PropertiesRAPL_RegLocations.Instance.EnablePowerLimit2_MSR, PropertiesRAPL_RegLocations.Instance.EnablePowerLimit2_Bit, ppl.EnablePowerLimit);
                        break;
                }
            }

            if ((mask & PackagePowerLimitMask.PackageClampingLimitation) == PackagePowerLimitMask.PackageClampingLimitation)
            {
                switch (idx)
                {
                    case PackagePowerLimitIndex._1:
                        SetRegisterValue(PropertiesRAPL_RegLocations.Instance.PackageClampingLimit1_MSR, PropertiesRAPL_RegLocations.Instance.PackageClampingLimit1_Bit, ppl.PackageClampingLimitation);
                        break;
                    case PackagePowerLimitIndex._2:
                        SetRegisterValue(PropertiesRAPL_RegLocations.Instance.PackageClampingLimit2_MSR, PropertiesRAPL_RegLocations.Instance.PackageClampingLimit2_Bit, ppl.PackageClampingLimitation);
                        break;
                }
            }
        }

        public static double DecodeTimeWindowForPowerLimit(ulong val, double timeUnit)
        {
            int yyyyy = (int)(val & 0x1F);
            BitArray ba = new BitArray(new bool[] { (val & 0x40) == 0x40, (val & 0x20) == 0x20 });
            double frac = FromBinaryFractionFormat(ba);
            return ((1 + frac) * Math.Pow(2, yyyyy) * timeUnit);
        }

        public static ulong EncodeTimeWindowForPowerLimit(double val, double timeUnit)
        {
            // Might be a better way to do this, but for now we just search valid values of
            // the encoding until we find a match.
            for (int xx = 0; xx < 0x3; xx++)
            {
                for (int y = 0; y < 0x1F; y++)
                {
                    BitArray ba = new BitArray(new bool[] { (xx & 0x2) == 0x2, (xx & 0x1) == 0x1 });
                    double frac = FromBinaryFractionFormat(ba);
                    if (((1 + frac) * Math.Pow(2, y) * timeUnit) == val)
                        return (ulong)((xx << 5) | y);
                }
            }

            throw new ArgumentException("EncodeTimeWindowForPowerLimit Exception - Value: " + val.ToString() + ", TimeUnit: " + timeUnit.ToString());
        }
#endif
#if false
        #region Raw Register Values
        private static ulong GetPackagePowerLimit1_RegVal()
        {
            uint msr = PropertiesRAPL_RegLocations.Instance.PkgPowerLimit1_MSR;
            BitRange bitRange = PropertiesRAPL_RegLocations.Instance.PkgPowerLimit1_BitRange;

            return GetRegisterValue(msr, bitRange);
        }

        private static ulong GetTimeWindowPowerLimit1_RegVal()
        {
            uint msr = PropertiesRAPL_RegLocations.Instance.TimeWindowPowerLimit1_MSR;
            BitRange bitRange = PropertiesRAPL_RegLocations.Instance.TimeWindowPowerLimit1_BitRange;

            return GetRegisterValue(msr, bitRange);
        }

        private static ulong GetPackagePowerLimit2_RegVal()
        {
            uint msr = PropertiesRAPL_RegLocations.Instance.PkgPowerLimit2_MSR;
            BitRange bitRange = PropertiesRAPL_RegLocations.Instance.PkgPowerLimit2_BitRange;

            return GetRegisterValue(msr, bitRange);
        }

        private static ulong GetTimeWindowPowerLimit2_RegVal()
        {
            uint msr = PropertiesRAPL_RegLocations.Instance.TimeWindowPowerLimit2_MSR;
            BitRange bitRange = PropertiesRAPL_RegLocations.Instance.TimeWindowPowerLimit2_BitRange;

            return GetRegisterValue(msr, bitRange);
        }
        #endregion
#endif
        public static double FromBinaryFractionFormat(BitArray ba)
        {
            double value = 0;
            for (int i = 0; i < ba.Length; i++)
            {
                bool bitSet = ba[i];
                double mult = Math.Pow(2, (-1) - i);
                value += (bitSet == true) ? mult : 0;
            }
            return value;
        }

        public static double GetCurrentFrequency(int miMaxNonTurboRatio)
        {
            ulong lA = 0; ulong lM = 0; ulong A = 0; ulong M = 0;
            ReadMPerfAPerf(ref lM, ref lA);
            Thread.SpinWait(10000);
            ReadMPerfAPerf(ref M, ref A);
            return Math.Abs(((double)(A - lA) / (double)(M - lM)) * (miMaxNonTurboRatio * CurrentProperties.BASE_CORE_CLOCK_SPEED_MHZ));
        }
        #endregion

        public static IProcessorQueryAndControlProperties CurrentProperties { get; set; }

        public interface IProcessorQueryAndControlProperties
        {
            uint MSR_IA32_MPERF { get; }
            uint MSR_IA32_APERF { get; }
            uint MSR_PLATFORM_INFO { get; }
            uint MSR_IA32_PERF_STATUS { get; }
            uint MSR_IA32_MISC_ENABLE { get; }
            int MSR_IA32_MISC_ENABLE_TURBO_MODE_DISABLE_BIT { get; }
            BitRange MSR_IA32_PERF_STATUS_PSTATE_VALUE { get; }
            BitRange MSR_IA32_PERF_CTL_PSTATE_VALUE { get; }
            BitRange MSR_PLATFORM_INFO_MAX_NON_TURBO_RATIO { get; }
            BitRange MSR_PLATFORM_INFO_MAX_EFFICIENCY_RATIO { get; }
            double BASE_CORE_CLOCK_SPEED_MHZ { get; }
        }
    }

    #region BitRange
    public class BitRange
    {
        protected BitRange()
        {
        }

        protected BitRange(int stopBit, int startBit)
        {
            if (startBit > stopBit)
                throw new FormatException(String.Format("Start bit ({0}) cannot be greater than stop bit ({1})", startBit, stopBit));

            StartBit = startBit;
            StopBit = stopBit;
        }

        public int StartBit
        {
            get;
            set;
        }

        public int StopBit
        {
            get;
            set;
        }

        public int BitCount
        {
            get
            {
                return StopBit - StartBit + 1;
            }
        }

        public ulong Mask
        {
            get
            {
                ulong mask = 0;
                for (int i = StartBit; i <= StopBit; i++)
                    mask |= (ulong)(Math.Pow(2, i));
                return mask;
            }
        }

        public override string ToString()
        {
            return String.Format("{0}:{1}", StopBit, StartBit);
        }

        public static BitRange FromString(string r)
        {
            int startBit; int stopBit;

            string[] r2 = r.Split(new char[] { ':' }, StringSplitOptions.RemoveEmptyEntries);
            if (r2.Length != 2)
                throw new FormatException(String.Format("{0} is not a correctly formatted bit range (Correct Format Example: 15:8)", r));

            try
            {
                stopBit = int.Parse(r2[0]);
            }
            catch (Exception ex)
            {
                throw new FormatException(String.Format("{0} is not a correctly formatted bit range. {1} cannot be converted to an int", r, r2[0]), ex);
            }

            try
            {
                startBit = int.Parse(r2[1]);
            }
            catch (Exception ex)
            {
                throw new FormatException(String.Format("{0} is not a correctly formatted bit range. {1} cannot be converted to an int", r, r2[1]), ex);
            }

            return new BitRange(stopBit, startBit);
        }
    }
    #endregion

    public class BitRangeTypeConverter : TypeConverter
    {
        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            if (sourceType == typeof(string))
                return true;
            else
                return base.CanConvertFrom(context, sourceType);
        }

        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            if (destinationType == typeof(string))
                return true;
            else
                return base.CanConvertTo(context, destinationType);
        }

        public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
        {
            if (destinationType == typeof(string) && value.GetType() == typeof(BitRange))
            {
                return value.ToString();
            }
            else
                return base.ConvertTo(context, culture, value, destinationType);
        }

        public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
        {
            if (value.GetType() == typeof(string))
            {
                try
                {
                    return BitRange.FromString((string)value);
                }
                catch
                {
                    // Put the old value back
                    System.Windows.Forms.GridItem gridItem = context as System.Windows.Forms.GridItem;
                    if (gridItem != null)
                        return (BitRange)gridItem.Value;
                    else
                        return BitRange.FromString("63:0");
                }
            }
            else
            {
                return base.ConvertFrom(context, culture, value);
            }
        }
    }
}
