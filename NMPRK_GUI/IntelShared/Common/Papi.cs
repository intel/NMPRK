using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace IntelShared.Common
{
    class Papi
    {
        public static uint PAPI_SUCCESS = 0;
        //------------------------------------------------- 
        // Variable to use papi.dll calls
        //------------------------------------------------- 
        private static bool mbUsePAPI = false;
        //------------------------------------------------- 
        // Structures for Processor Status
        //------------------------------------------------- 
        #region Structures and Wrappers for CPU Status Calls
        private struct PLATFORM_CPU_PACKAGE_INFO
        {
            public UInt32 numCoresInThisPackage;
            public UInt32 PackageID;
            public Boolean bPackageValid;
        };
        //---------------------------------------------- 
        // Structure to get CPU details
        //---------------------------------------------- 
        [StructLayout(LayoutKind.Sequential)]
        private struct PLATFORM_CPU_INFO
        {
            public Boolean bIsx86;                    /* regular apicid */
            public UInt64 cpuAPICID;                  /* if platform supports x2apic */
            public Boolean bx2APICSupported;          /* if x2apic is enabled in the platform */
            public Boolean bx2APICEnabled;            /* if x2APIC enabled */
            public Boolean bIsBSP;                    /* boot service processor */
            public UInt32 VAAddressSize;              /* virtual address length */
            public UInt32 PAAddressSize;              /* physical address size */
            public UInt32 PackageID;
            public UInt32 CoreID;
            public UInt32 ThreadID;
            public UInt32 NumPackagesInPlatform;
        }
        //---------------------------------------------- 
        // Pinvokes for Processor Status
        //---------------------------------------------- 
        [DllImport("papi", EntryPoint = "PAPI_CPU_Query_Num_Packages_In_Platform")]
        [return: MarshalAs(UnmanagedType.U4)]
        private static extern UInt32 cPhysicalCPUs(ref UInt32 uiNumberOfPhysicalCPUs);
        [DllImport("papi", EntryPoint = "PAPI_CPU_Query_Package_Info")]
        [return: MarshalAs(UnmanagedType.U4)]
        private static extern UInt32 cCPUPackageInformation(UInt32 PackageIndex,
        ref PLATFORM_CPU_PACKAGE_INFO PackageInfo);
        [DllImport("papi", EntryPoint = "PAPI_CPU_Query_Platform_CPU_Data")]
        [return: MarshalAs(UnmanagedType.U4)]
        private static extern UInt32 cCPUPlatformCPUData(ref UInt32 arrayLength,
        [In, Out]  PLATFORM_CPU_INFO[] PackageInfo);
        #endregion
        //------------------------------------------------- 
        // Structures for MSR Reads/Writes
        //------------------------------------------------- 
        #region MSR Structures and Wrappers
        private struct MSR_STRUCT
        {
            public UInt32 ecx;    // MSR number
            public UInt32 eax;    // IN (write) or OUT (read): Low 32 bits of MSR
            public UInt32 edx;    // IN (write) or OUT (read): High 32 bits of MSR
        };
        //---------------------------------------------- 
        // Pinvokes for MSR Reads/Writes
        //---------------------------------------------- 
        [DllImport("papi", EntryPoint = "Read_IA32_MSR")]
        [return: MarshalAs(UnmanagedType.U4)]
        private static extern UInt32 cReadMSR(ref MSR_STRUCT Data);
        [DllImport("papi", EntryPoint = "Write_IA32_MSR")]
        [return: MarshalAs(UnmanagedType.U4)]
        private static extern UInt32 cWriteMSR(ref MSR_STRUCT Data);
        #endregion
        //------------------------------------------------- 
        // Constants and Enums for CPUID Features
        //------------------------------------------------- 
        #region CPUID Constants
        // CPUID, EAX=1, Returned in ECX
        private const int CPU_FEATURE_SSE3_FLAG = 0x1;
        private const int CPU_FEATURE_MON_FLAG = 0x8;
        private const int CPU_FEATURE_DSCPL_FLAG = 0x10;
        private const int CPU_FEATURE_VMX_FLAG = 0x20;
        private const int CPU_FEATURE_EST_FLAG = 0x80;
        private const int CPU_FEATURE_TM2_FLAG = 0x100;
        private const int CPU_FEATURE_CID_FLAG = 0x400;
        private const int CPU_FEATURE_CX16_FLAG = 0x1000;
        private const int CPU_FEATURE_ETPRD_FLAG = 0x2000;
        // CPUID, EAX=1, Returned in EDX
        private const int CPU_FEATURE_FPU_FLAG = 0x1;
        private const int CPU_FEATURE_VME_FLAG = 0x2;
        private const int CPU_FEATURE_DE_FLAG = 0x4;
        private const int CPU_FEATURE_PSE_FLAG = 0x8;
        private const int CPU_FEATURE_TSE_FLAG = 0x10;
        private const int CPU_FEATURE_MSR_FLAG = 0x20;
        private const int CPU_FEATURE_PAE_FLAG = 0x40;
        private const int CPU_FEATURE_MCE_FLAG = 0x80;
        private const int CPU_FEATURE_CMPXCHG8B_FLAG = 0x100;
        private const int CPU_FEATURE_APIC_FLAG = 0x200;
        private const int CPU_FEATURE_SYSENTER_FLAG = 0x800;
        private const int CPU_FEATURE_MTRR_FLAG = 0x1000;
        private const int CPU_FEATURE_PGE_FLAG = 0x2000;
        private const int CPU_FEATURE_MCA_FLAG = 0x4000;
        private const int CPU_FEATURE_CMOV_FLAG = 0x8000;
        private const int CPU_FEATURE_PAT_FLAG = 0x10000;
        private const int CPU_FEATURE_PSE36_FLAG = 0x20000;
        private const int CPU_FEATURE_PSN_FLAG = 0x40000;
        private const int CPU_FEATURE_CLFL_FLAG = 0x80000;
        private const int CPU_FEATURE_DTES_FLAG = 0x200000;
        private const int CPU_FEATURE_ACPI_FLAG = 0x400000;
        private const int CPU_FEATURE_MMX_FLAG = 0x800000;
        private const int CPU_FEATURE_FXSR_FLAG = 0x1000000;
        private const int CPU_FEATURE_SSE_FLAG = 0x2000000;
        private const int CPU_FEATURE_SSE2_FLAG = 0x4000000;
        private const int CPU_FEATURE_SS_FLAG = 0x8000000;
        private const int CPU_FEATURE_HTT_FLAG = 0x10000000;
        private const int CPU_FEATURE_TM1_FLAG = 0x20000000;
        private const int CPU_FEATURE_IA64_FLAG = 0x40000000;
        private const uint CPU_FEATURE_PBE_FLAG = 0x80000000;
        private const int CPU_STEPPING_FLAG = 0xF;
        private const int CPU_TYPE_FLAG = 0xF00;
        private const int CPU_FAMILY_FLAG = 0x780;
        private const int CPU_EXTENDED_FAMILY_FLAG = 0xFF00000;
        private const int CPU_MODEL_FLAG = 0xF0;
        private const int CPU_EXTENDED_MODEL_FLAG = 0xF0000;
        private const int CPU_BRAND_ID_FLAG = 0xFF;
        private const int CPU_CLFLUSH_FLAG = 0xFF00;
        private const int CPU_COUNT_FLAG = 0xFF0000;
        private const uint CORE_COUNT_FLAG = 0xFC000000;
        private const uint CPU_APIC_FLAG = 0xFF008000;
        private const string VendorUMC = "UMC UMC UMC ";
        private const string VendorAMD = "AuthenticAMD";
        private const string VendorCyrix = "CyrixInstead";
        private const string VendorNextGen = "NexGenDriven";
        private const string VendorCentaur = "CentaurHauls";
        private const string VendorRise = "RiseRiseRise";
        private const string VendorSiS = "SiS SiS SiS ";
        private const string VendorTransmeta = "GenuineTMx86";
        private const string VendorNSC = "Geode by NSC";
        private const string VendorIntel = "GenuineIntel";
        // CPUID, EAX=6, Returned in EAX
        private const int TURBO_MODE_SUPPORTED = 0x2;
        #endregion
        //------------------------------------------------- 
        // Structure and Wrappers for CPUID Calls
        //------------------------------------------------- 
        #region CPUID Structures and Wrappers
        public struct CPUID_STRUCT
        {
            public UInt32 eax;
            public UInt32 ebx;
            public UInt32 ecx;
            public UInt32 edx;
        };
        //------------------------------------------------- 
        // Pinvokes for CPUID calls
        //------------------------------------------------- 
        [DllImport("papi", EntryPoint = "IssueIA32_CPUID")]
        [return: MarshalAs(UnmanagedType.U4)]
        private static extern UInt32 cpuid(ref CPUID_STRUCT regs);
        #endregion

        #region Read Config Space
        [DllImport("papi", EntryPoint = "PAPI_PCI_Read_Cfg_Space")]
        [return: MarshalAs(UnmanagedType.U4)]
        private static extern UInt32 cReadConfigSpace(
        [In, MarshalAs(UnmanagedType.U8)] ulong device,
        [In, MarshalAs(UnmanagedType.U2)] ushort register,
        [In, Out, MarshalAs(UnmanagedType.U2)] ref ushort bufferlength,
        [In, Out, MarshalAs(UnmanagedType.LPArray, SizeConst = 4)] byte[] buffer,
        [In, MarshalAs(UnmanagedType.U1)] byte flags);
        #endregion

        [DllImport("papi", EntryPoint = "GetPAPIv2Version", SetLastError=true)]
        [return: MarshalAs(UnmanagedType.U4)]
        private static extern UInt32 cGetPapiVersion(
        [In, Out, MarshalAs(UnmanagedType.LPTStr)] StringBuilder dllVersion,
        [In, MarshalAs(UnmanagedType.U4)] uint nDllVersion,
        [In, Out, MarshalAs(UnmanagedType.LPTStr)] StringBuilder drvVersion,
        [In, MarshalAs(UnmanagedType.U4)] uint nDrvVersion);

        [DllImport("papi", EntryPoint = "PAPIv2DriverInstalled", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.U4)]
        private static extern UInt32 cPAPIv2DriverInstalled();

        [DllImport("papi", EntryPoint = "PAPI_CPU_Read_MPERF_APERF")]
        [return: MarshalAs(UnmanagedType.U4)]
        private static extern UInt32 cReadMPerfAPerf(
        [In, MarshalAs(UnmanagedType.U4)] uint mPerfMsr,
        [In, MarshalAs(UnmanagedType.U4)] uint aPerfMsr,
        [In, Out, MarshalAs(UnmanagedType.U8)] ref ulong mPerf,
        [In, Out, MarshalAs(UnmanagedType.U8)] ref ulong aPerf);        

        [DllImport("papi", EntryPoint = "PAPI_CPU_Read_MSRs")]
        [return: MarshalAs(UnmanagedType.U4)]
        public static extern UInt32 cPAPI_CPU_Read_MSRs(
            [In, MarshalAs(UnmanagedType.LPArray)] uint[] msrArray,
            [In, MarshalAs(UnmanagedType.I4)] int msrArraySize,
            [In, Out, MarshalAs(UnmanagedType.LPArray)] ulong[] msrOutArray,
            [In, MarshalAs(UnmanagedType.I4)] int msrOutArraySize);

        [StructLayout(LayoutKind.Sequential)]
        public struct PAPI_CPU_CSTATE_TEST_SEQUENCE_ITEM
        {
            public uint eaxForMWait;
	        public ulong timeMs;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct PAPI_CPU_CSTATE_TEST_THREAD_INFO
        {
            public ushort affinityGroup;
            public ulong affinityMask;
        }

        [DllImport("papi", EntryPoint = "PAPI_CPU_CStateTestStart")]
        [return: MarshalAs(UnmanagedType.U4)]
        public static extern UInt32 cPAPI_CPU_CStateTestStart(
            [In, MarshalAs(UnmanagedType.LPArray)] PAPI_CPU_CSTATE_TEST_THREAD_INFO[] threads,
            [In, MarshalAs(UnmanagedType.I4)] int threadsCount,
            [In, MarshalAs(UnmanagedType.LPArray)] PAPI_CPU_CSTATE_TEST_SEQUENCE_ITEM[] seqItems,
            [In, MarshalAs(UnmanagedType.I4)] int seqItemsCount,
            [In, MarshalAs(UnmanagedType.Bool)] bool continuous,
            [In, MarshalAs(UnmanagedType.LPWStr)] string finishedEventName,
            [In, MarshalAs(UnmanagedType.LPWStr)] string seqIncrementEventName,
            [In, Out, MarshalAs(UnmanagedType.I4)] ref int handle);

        [DllImport("papi", EntryPoint = "PAPI_CPU_CStateTestStop")]
        [return: MarshalAs(UnmanagedType.U4)]
        public static extern UInt32 cPAPI_CPU_CStateTestStop(
            [In, MarshalAs(UnmanagedType.I4)] int handle);

        //------------------------------------------------- 
        // General Purpose Routines
        //------------------------------------------------- 
        #region Routines
        public static string GetCPUVendor()
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            string rv = "Invalid Vendor ID";
            CPUID_STRUCT pRegs;
            pRegs.eax = 0; pRegs.ebx = 0; pRegs.ecx = 0; pRegs.edx = 0;
            if (GetCPUID(ref pRegs))
            {
                rv = ConvertDWORDToString(pRegs.ebx) +
                ConvertDWORDToString(pRegs.edx) + ConvertDWORDToString(pRegs.ecx);
            }
            return rv;
        }
        public static bool IsCPUNehalemOrGreater()
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            bool rv = false;
            uint Stepping;
            uint Model;
            uint Family;
            uint Type;
            uint ExtendedModel;
            uint ExtendedFamily;
            uint FinalFamily;
            uint FinalModel;
            const int SSE4_1_FLAG = 0x080000;
            const int SSE4_2_FLAG = 0x100000;
            const int CHECKBITS = SSE4_1_FLAG | SSE4_2_FLAG;
            // For function Number 1, edx bit 5 = MSR available
            CPUID_STRUCT pRegs;
            pRegs.eax = 1; pRegs.ebx = 0; pRegs.ecx = 0; pRegs.edx = 0;
            if (GetCPUID(ref pRegs))
            {
                Stepping = ((pRegs.eax >> 0) & 0xf);
                Model = ((pRegs.eax >> 4) & 0xf);
                Family = ((pRegs.eax >> 8) & 0xf);
                Type = ((pRegs.eax >> 12) & 0x3);
                ExtendedModel = ((pRegs.eax >> 16) & 0xf);
                ExtendedFamily = ((pRegs.eax >> 20) & 0xff);
                FinalFamily = (ExtendedFamily + Family);
                FinalModel = ((ExtendedModel << 4) + Model);
                if ((pRegs.ecx & CHECKBITS) == CHECKBITS) rv = true;
            }
            return rv;
        }
        public static string GetCPUName()
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            CPUID_STRUCT pRegs;
            string CPUName = string.Empty;
            for (uint i = 0x80000002; i <= 0x80000004; i++)
            {
                pRegs.eax = i; pRegs.ebx = 0; pRegs.ecx = 0; pRegs.edx = 0;
                if (GetCPUID(ref pRegs))
                {
                    CPUName += (ConvertDWORDToString(pRegs.eax) + ConvertDWORDToString(pRegs.ebx) +
                    ConvertDWORDToString(pRegs.ecx) + ConvertDWORDToString(pRegs.edx));
                }
            }
            CPUName = CPUName.Replace("  ", " ");
            CPUName = CPUName.Replace("\0", "");
            return CPUName.Trim();
        }

        public static uint GetCpuSignature()
        {
            CPUID_STRUCT pRegs;
            pRegs.eax = 1; pRegs.ebx = 0; pRegs.ecx = 0; pRegs.edx = 0;
            if (GetCPUID(ref pRegs))
            {
                return pRegs.eax;
            }

            return 0;
        }

        public static bool GetCpuSignature(out uint stepping, out uint model, out uint family, out uint type)
        {
            stepping = 0; model = 0; family = 0; type = 0;

            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            uint Model;
            uint Family;
            uint ExtendedModel;
            uint ExtendedFamily;
            // For function Number 1, edx bit 5 = MSR available
            CPUID_STRUCT pRegs;
            pRegs.eax = 1; pRegs.ebx = 0; pRegs.ecx = 0; pRegs.edx = 0;
            if (GetCPUID(ref pRegs))
            {
                Model = ((pRegs.eax >> 4) & 0xf);
                Family = ((pRegs.eax >> 8) & 0xf);
                ExtendedModel = ((pRegs.eax >> 16) & 0xf);
                ExtendedFamily = ((pRegs.eax >> 20) & 0xff);

                stepping = ((pRegs.eax >> 0) & 0xf);
                type = ((pRegs.eax >> 12) & 0x3);
                family = (ExtendedFamily + Family);
                model = ((ExtendedModel << 4) + Model);
                return true;
            }
            else
                return false;
        }

        public static bool IsTurboSupported(uint turboControlMsr, uint turboControlBit)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            bool rv = false;
            // For function Number 1, edx bit 5 = MSR available
            CPUID_STRUCT pRegs;
            pRegs.eax = 1; pRegs.ebx = 0; pRegs.ecx = 0; pRegs.edx = 0;
            if (GetCPUID(ref pRegs))
            {
                if ((pRegs.edx & CPU_FEATURE_MSR_FLAG) != 0)
                {
                    // For function Number 6, eax bit 1 = Intel Turbo Boost Technology
                    pRegs.eax = 6; pRegs.ebx = 0; pRegs.ecx = 0; pRegs.edx = 0;
                    if (GetCPUID(ref pRegs))
                    {
                        if ((pRegs.eax & TURBO_MODE_SUPPORTED) != 0)
                            rv = true;
                        else
                        {
                            uint LowValue = 0;
                            uint HighValue = 0;
                            if (Convert.ToBoolean(ReadMSR(turboControlMsr, ref LowValue, ref HighValue)))
                            {
                                ulong TurboBoostControlResult =
                                (ulong)(((ulong)HighValue << 32) | (ulong)LowValue);
                                double Mask = Math.Pow(2, (int)turboControlBit);
                                rv = ((TurboBoostControlResult & (ulong)Mask) != 0);
                            }
                        }
                    }
                }
            }
            return rv;
        }
        public static bool IsHTSupported()
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            bool rv = false;
            if ((IsCPUNehalemOrGreater()) && (GetCPUVendor().ToLower() == "genuineintel"))
            {
                // For function Number 1, edx bit 28 = HT Supported available
                CPUID_STRUCT pRegs;
                pRegs.eax = 1; pRegs.ebx = 0; pRegs.ecx = 0; pRegs.edx = 0;
                if (GetCPUID(ref pRegs))
                    rv = Convert.ToBoolean((pRegs.edx & CPU_FEATURE_HTT_FLAG) >> 28);
            }
            return rv;
        }

        public static uint MaxCoresPerPackage()
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            uint rv = (uint)0;
            // For function Number 0, eax contains Max Cores/PP
            CPUID_STRUCT pRegs;
            pRegs.eax = 0; pRegs.ebx = 0; pRegs.ecx = 0; pRegs.edx = 0;
            if (GetCPUID(ref pRegs))
            {
                if (pRegs.eax >= 4)
                {
                    pRegs.eax = 4; pRegs.ebx = 0; pRegs.ecx = 0; pRegs.edx = 0;
                    if (GetCPUID(ref pRegs))
                        rv = ((pRegs.eax & CORE_COUNT_FLAG) >> 26) + 1;
                    if (rv == (uint)0) rv = (uint)1;
                }
                else
                    rv = 1;
            }
            return rv;
        }
        public static uint MaxLogicalProcessorsPerPackage()
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            uint rv = (uint)0;
            // For function Number 1, ebx contains Max LP/PP
            CPUID_STRUCT pRegs;
            pRegs.eax = 1; pRegs.ebx = 0; pRegs.ecx = 0; pRegs.edx = 0;
            if (GetCPUID(ref pRegs))
                rv = ((pRegs.ebx & CPU_COUNT_FLAG) >> 16);
            if (rv == (uint)0) rv = (uint)1;
            return rv;
        }
        
        public static bool GetCPUID(ref CPUID_STRUCT regs)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //---------------------------------------------- 
            if (!Convert.ToBoolean(cpuid(ref regs))) return true;
            else return false;
        }
        private static string ConvertDWORDToString(UInt32 aDW)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //---------------------------------------------- 
            int dxl = (int)(aDW & 0xff);
            int dxh = (int)(aDW & 0xff00) >> 8;
            int dlx = (int)(aDW & 0xff0000) >> 16;
            int dhx = (int)((uint)(aDW & 0xff000000) >> 24);
            return (((char)dxl).ToString() + ((char)dxh).ToString() +
            ((char)dlx).ToString() + ((char)dhx).ToString());
        }
        public static bool ReadMSR(UInt32 ulECX, ref UInt32 pulEAX, ref UInt32 pulEDX)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //----------------------------------------------
            UInt32 rv = 1;
            MSR_STRUCT msr;
            msr.ecx = ulECX;
            msr.eax = 0;
            msr.edx = 0;
            rv = cReadMSR(ref msr);
            pulEAX = msr.eax;
            pulEDX = msr.edx;
            return !Convert.ToBoolean(rv);
        }
        public static bool ReadMSR(uint msr, out ulong value)
        {
            uint LowValue = 0; uint HighValue = 0;
            bool ret = Papi.ReadMSR(msr, ref LowValue, ref HighValue);
            if(ret == true)
                value = (ulong)(((ulong)HighValue << 32) | (ulong)LowValue);
            else
                value = 0;
            return ret;
        }
        public static bool WriteMSR(UInt32 ulECX, ref UInt32 pulEAX, ref UInt32 pulEDX)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //---------------------------------------------- 
            UInt32 rv = 1;
            MSR_STRUCT msr;
            msr.ecx = ulECX;
            msr.eax = pulEAX;
            msr.edx = pulEDX;
            rv = cWriteMSR(ref msr);
            return !Convert.ToBoolean(rv);
        }
        public static uint ReadPCIConfigSpaceDWord(uint aBus, uint aDevice, uint aFunction, uint aRegister)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2011 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 8/17/2010 
            //----------------------------------------------
            // Constants
            //----------------------------------------------
            //const byte PAPI_PCI_CFG_USE_LEGACY = 0x1;
            const byte PAPI_PCI_CFG_USE_EXTENDED = 0x2;
            const byte CL_SUCCESS = 0x0;
            //----------------------------------------------
            // Get BDF
            //----------------------------------------------
            UInt64 BDF = (UInt64)GetBDF(0, aBus, aDevice, aFunction);
            //----------------------------------------------
            // Perform the PCIE config Read
            //----------------------------------------------
            uint status;
            byte[] buf = new byte[4];
            uint retval = 0xFFFFFFFF;
            ushort length = 4;
            ushort register = (ushort)aRegister;
            //------------------------------------------
            // Lock Array in Memory
            //------------------------------------------
            status = cReadConfigSpace(BDF, register, ref length, buf, PAPI_PCI_CFG_USE_EXTENDED);
            // Check Return Value
            if (status != CL_SUCCESS)
            {
                //TODO:  break this out with explicit BDF details.
                return (retval);
            }
            else
            {
                retval = (uint)(((buf[3] << 24) | (buf[2] << 16) | buf[1] << 8) | (buf[0] << 0));
                return (retval);
            }
        }
        private static ulong GetBDF(uint aSegment, uint aBus, uint aDevice, uint aFunction)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2011 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 8/17/2010 
            //----------------------------------------------
            return ((UInt64)((aSegment << 24) | (aBus << 16) |
            (aDevice << 8) | (aFunction)));
        }
        #endregion

        public static uint ReadMPerfAPerf(uint mPerfMsr, uint aPerfMsr, ref ulong mPerf, ref ulong aPerf)
        {
            return cReadMPerfAPerf(mPerfMsr, aPerfMsr, ref mPerf, ref aPerf);
        }

        public static uint GetPapiVersion(out string dllVersion, out string drvVersion)
        {
            StringBuilder dllV = new StringBuilder(256);
            StringBuilder drvV = new StringBuilder(256);
            uint ret = cGetPapiVersion(dllV, 256, drvV, 256);
            dllVersion = dllV.ToString();
            drvVersion = drvV.ToString();
            return ret;
        }

        public class PapiException : Exception
        {
            public PapiException(string msg)
                : base(msg)
            {
            }
        }

        public static void UninstallPAPIv2Driver()
        {
            if (File.Exists("Uninstall.cmd") == false)
            {
                throw new PapiException("Could not find Uninstall.cmd (PAPI Uninstall Script) in local directory!");
            }

            Process.Start("Uninstall.cmd");
        }

        public static bool IsValidPapiVersion(string requiredDllVersion, string requiredDrvVersion, out string dllVersion, out string drvVersion)
        {
            dllVersion = "Unknown";
            drvVersion = "Unknown";

            uint ret = uint.MaxValue;
            try
            {
                ret = Papi.GetPapiVersion(out dllVersion, out drvVersion);
            }
            catch { }

            if (ret == Papi.PAPI_SUCCESS)
            {
                ErrorLog.WriteLine("PAPI DLL Version: " + dllVersion);
                ErrorLog.WriteLine("PAPI DRV Version: " + drvVersion);
            }
            else
            {
                ErrorLog.WriteLine("GetPapiVersion Failed!");
            }

            if (ret != Papi.PAPI_SUCCESS ||
                IsAtLeastRequiredVersion(requiredDllVersion, dllVersion) != true ||
                IsAtLeastRequiredVersion(requiredDrvVersion, drvVersion) != true)
            {
                if (dllVersion == "0.0.0.0" || drvVersion == "0.0.0.0")
                {
                    ErrorLog.WriteLine("Using Debug Version of PAPI! Proceed with Caution!");
                }

                return false;
            }
            else
            {
                return true;
            }
        }

        public static bool IsAtLeastRequiredVersion(string required, string actual)
        {
            try
            {
                string[] requiredVersion = required.Split('.');
                string[] actualVersion = actual.Split('.');
                bool status = false;
                if (requiredVersion.Length >= 2 && actualVersion.Length >= 2)
                {
                    for (int i = 0; i < requiredVersion.Length && i < actualVersion.Length; i++)
                    {
                        int act = System.Convert.ToInt32(actualVersion[i]);
                        int req = System.Convert.ToInt32(requiredVersion[i]);
                        status = (act >= req);
                        if (status == false)
                            break;
                    }
                }
                return status;
            }
            catch
            {
                return false;
            }
        }
    }
}
