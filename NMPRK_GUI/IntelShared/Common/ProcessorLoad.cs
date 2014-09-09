using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace IntelShared.Common
{
    class ProcessorLoad
    {
        public const int MAX_PROCESSOR_LOAD = 90;

        ManualResetEvent _stopEvent = null;
        volatile int miDesiredCPULoad = 0;
        int aLPNumber = 0;

        #region CPU Loading Routines
        public delegate void UpdateUsage(int usage);
        public delegate void LoadCpuCallback();

        public int DesiredLoad
        {
            get { return miDesiredCPULoad; }
            set
            {
                if (miDesiredCPULoad != value)
                {
                    ErrorLog.WriteLine("ProcLoad LP " + aLPNumber + ", Load Change, O: " + miDesiredCPULoad + ", N: " + value);
                    if (value < 0)
                        miDesiredCPULoad = 0;
                    else if (value > MAX_PROCESSOR_LOAD)
                        miDesiredCPULoad = MAX_PROCESSOR_LOAD;
                    else
                        miDesiredCPULoad = value;
                }
            }
        }

        public void Initialize()
        {
            _waitSleepStopwatch.Start();
        }

        [DllImport("kernel32", SetLastError = true,
        CharSet = CharSet.Unicode, EntryPoint = "QueryIdleProcessorCycleTimeEx")]
        private static extern int QueryIdleProcessorCycleTimeEx(
            [In, MarshalAs(UnmanagedType.U2)] ushort Group,
            [In, Out, MarshalAs(UnmanagedType.U8)] ref ulong BufferLength,
            [In, Out, MarshalAs(UnmanagedType.LPArray)] ulong[] ProcessorIdleCycleTime);

        [DllImport("kernel32", SetLastError = true,
        CharSet = CharSet.Unicode, EntryPoint = "QueryPerformanceCounter")]
        private static extern bool QueryPerformanceCounter(out ulong lpPerformanceCount);

        [DllImport("kernel32", SetLastError = true,
        CharSet = CharSet.Unicode, EntryPoint = "QueryPerformanceFrequency")]
        private static extern bool QueryPerformanceFrequency(out ulong lpFrequency);

        public void LoadCPU(ProcessorQueryAndControl.LogicalProcessorInfo lpInfo, AutoResetEvent ev, UpdateUsage uu, LoadCpuCallback lcc, ManualResetEvent stopEvent)
        {
            aLPNumber = lpInfo.AbsoluteProcessor;
            ErrorLog.WriteLine("LoadCPU: LPNumber = " + aLPNumber);

            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //---------------------------------------------- 
            // Thread Variables
            //---------------------------------------------- 
            Thread.CurrentThread.Priority = ThreadPriority.Lowest;
            Thread.CurrentThread.IsBackground = true;
            ProcessThread threadCurrentThread = GetCurrentProcessThread();

#if USE_PERFORMANCE_COUNTER
            PerformanceCounter _perfCounter = null;
            try
            {
                _perfCounter = new PerformanceCounter("Processor", "% Processor Time", aLPNumber.ToString());
                ErrorLog.WriteLine("LoadCPU: LPNumber = " + aLPNumber + ", PerformanceCounter.RawValue = " + _perfCounter.RawValue);
            }
            catch (Exception ex)
            {
                ErrorLog.LogException(ex);
                return;
            }
#else
            ulong PERFORMANCE_FREQ = 0;
            QueryPerformanceFrequency(out PERFORMANCE_FREQ);

            ulong LastPerformanceCounter = 0;
            QueryPerformanceCounter(out LastPerformanceCounter);

            ulong IdleProcessorCycleTimeBufferLength = 0;
            ulong[] IdleProcessorCycleTimeBuffer = null;

            // First get the size required for the buffer
            QueryIdleProcessorCycleTimeEx(lpInfo.GroupNumber,
                ref IdleProcessorCycleTimeBufferLength,
                IdleProcessorCycleTimeBuffer);

            // Now create the buffer and get the data
            IdleProcessorCycleTimeBuffer = new ulong[IdleProcessorCycleTimeBufferLength / sizeof(ulong)];
            QueryIdleProcessorCycleTimeEx(lpInfo.GroupNumber,
                ref IdleProcessorCycleTimeBufferLength,
                IdleProcessorCycleTimeBuffer);

            ulong LastIdleTime = IdleProcessorCycleTimeBuffer[lpInfo.GroupProcessor];
#endif
            _stopEvent = stopEvent;
            //----------------------------------------------
            // Constants
            //---------------------------------------------- 
            const ulong FACTORIAL = 10;
            const int iDefaultSleep = 100;
            //---------------------------------------------- 
            // LOcal Variables
            //---------------------------------------------- 
            bool bDebugPrint = false;
            uint Iterations = 5000;
            ulong LastCPUTime = (ulong)threadCurrentThread.TotalProcessorTime.Ticks;
            ulong LastWallTime = (ulong)DateTime.Now.Ticks;
            long LastUsageUpdate = DateTime.Now.Ticks;
            ulong DeltaCPU = 0;
            ulong DeltaWall = 0;
            int _usage = 0;
            //----------------------------------------------
            // Load Current Thread
            //---------------------------------------------- 
            while (!_stopEvent.WaitOne(0))
            {
                try
                {
                    ProcessorQueryAndControl.SetAffinity(lpInfo);

                    int tmpDesiredLoad = miDesiredCPULoad;

                    // calculate the usage
                    if (DateTime.Now.Ticks > LastUsageUpdate + (10000 * 1000))
                    {
#if USE_PERFORMANCE_COUNTER
                        _usage = (int)Math.Round(_perfCounter.NextValue(), 0);
#else
                        ulong CurrentIdleTime = 0;
                        ulong CurrentPerformanceCounter = 0;

                        QueryPerformanceCounter(out CurrentPerformanceCounter);
                        ulong PerformanceCounterDiff = CurrentPerformanceCounter - LastPerformanceCounter;
                        LastPerformanceCounter = CurrentPerformanceCounter;

                        QueryIdleProcessorCycleTimeEx(lpInfo.GroupNumber,
                            ref IdleProcessorCycleTimeBufferLength,
                            IdleProcessorCycleTimeBuffer);

                        CurrentIdleTime = IdleProcessorCycleTimeBuffer[lpInfo.GroupProcessor];
                        ulong IdleTimeDiff = CurrentIdleTime - LastIdleTime;
                        LastIdleTime = CurrentIdleTime;

                        double IdleTime = (double)IdleTimeDiff / PERFORMANCE_FREQ;
                        double IntervalDuration = ((double)PerformanceCounterDiff / PERFORMANCE_FREQ) * 1000;

                        //Console.WriteLine("IdleTimeDiff: " + IdleTimeDiff);
                        //Console.WriteLine("PerformanceCounterDiff: " + PerformanceCounterDiff);
                        //Console.WriteLine("IdleTime: " + IdleTime);
                        //Console.WriteLine("IntervalDuration: " + IntervalDuration);

                        double trueUsage = ((IntervalDuration - IdleTime) * 100) / IntervalDuration;
                        //Console.WriteLine("trueUsage: " + trueUsage);
                        _usage = (int)Math.Round(trueUsage);
                        if (_usage < 0)
                            _usage = 0;
                        else if (_usage > 100)
                            _usage = 100;
                        //Console.WriteLine("Usage: " + _usage);
                        if (_usage < miDesiredCPULoad - 10 || _usage > miDesiredCPULoad + 10)
                        {
                            ErrorLog.WriteLine("ProcLoad LP " + aLPNumber + ", Load Diff, U: " + _usage + ", L: " + miDesiredCPULoad);
                        }
#endif
                        LastUsageUpdate = DateTime.Now.Ticks;
                        if(uu != null)
                            uu(_usage);
                    }

                    // if the usage is higher than requested, adjust the requested usage down
                    if (_usage > tmpDesiredLoad)
                    {
                        tmpDesiredLoad = Math.Max(tmpDesiredLoad - (_usage - tmpDesiredLoad), 0);
                    }

                    // check if we need to process events
                    if (ev.WaitOne(0) == true)
                        lcc();

                    // cpu load code
                    if (((double)tmpDesiredLoad / 100) <= 0.1)
                    {
                        const int TIME = 100;
                        double percentBusy = (double)tmpDesiredLoad / 100.0;
                        double percentIdle = (double)1.0 - percentBusy;
                        if (percentBusy > 0)
                        {
                            DateTime stopTime = DateTime.Now.AddMilliseconds((int)(percentBusy * TIME));
                            while (DateTime.Now < stopTime) { };
                        }
                        WaitSleep((int)(percentIdle * TIME), ev, lcc);
                    }
                    else
                    {
                        double Target = ((double)tmpDesiredLoad / 100);
                        int SleepTime = (int)((1 * iDefaultSleep) * (1 - Target));
                        ulong FactorialNumber = (FACTORIAL - (FACTORIAL - ((ulong)tmpDesiredLoad / FACTORIAL))) + 0;
                        if (StressTheCPU(Iterations, FactorialNumber, ev, lcc))
                        {
                            ulong CurrentCPUTime = (ulong)threadCurrentThread.TotalProcessorTime.Ticks;
                            ulong CurrentWallTime = (ulong)DateTime.Now.Ticks;
                            DeltaCPU = (CurrentCPUTime - LastCPUTime);
                            DeltaWall = (CurrentWallTime - LastWallTime);
                            if (DeltaCPU == 0) DeltaCPU = 1;
                            double Multiplier = Math.Round(((Target * DeltaWall) / DeltaCPU), 2);
                            if (Multiplier > 5.0) Multiplier = 5.0;
                            Iterations = (uint)(Iterations * Multiplier);
                            if (Iterations < 1) Iterations = 1;
                            if (Iterations > (uint.MaxValue - 100000000))
                                Iterations = (uint.MaxValue - 10000000);
                            LastCPUTime = CurrentCPUTime;
                            LastWallTime = CurrentWallTime;
                            if (SleepTime > 0) WaitSleep(SleepTime, ev, lcc);
                            if (bDebugPrint)
                            {
                                string dataString = String.Format(
                                "Iterations = {0}, Multiplier = {1}, Sleep = {2}, LP = {3}, DW = {4}, DCPU = {5}",
                                Iterations, Multiplier, SleepTime, aLPNumber, DeltaWall, DeltaCPU);
                                Debug.WriteLine(dataString);
                            }
                        }
                    }
                    Thread.Sleep(0);
                }
                catch (Exception ex)
                {
                    ErrorLog.LogException(ex);
                }
            }
            Thread.CurrentThread.Priority = ThreadPriority.Normal;
        }

        Stopwatch _waitSleepStopwatch = new Stopwatch();

        private void WaitSleep(int ms, AutoResetEvent ev, LoadCpuCallback lcc)
        {
            double waitMs = ms;
            long tStart = _waitSleepStopwatch.ElapsedMilliseconds;
            do
            {
                if (ev.WaitOne((int)Math.Round(waitMs)) == true)
                    lcc();

                waitMs = waitMs - (_waitSleepStopwatch.ElapsedMilliseconds - tStart);
            }
            while(waitMs > 0);
        }

        [DllImport("kernel32.dll")]
        private static extern uint GetCurrentThreadId();
        private ProcessThread GetCurrentProcessThread()
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //---------------------------------------------- 
            int currentThreadId = (int)GetCurrentThreadId();
            ProcessThreadCollection currentProcessThreads =
            Process.GetCurrentProcess().Threads;
            foreach (ProcessThread t in Process.GetCurrentProcess().Threads)
                if (t.Id == currentThreadId) return t;
            return null;
        }
        private Boolean StressTheCPU(uint aIterations, ulong aFactorialNumber, AutoResetEvent ev, LoadCpuCallback lcc)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //---------------------------------------------- 
            const int SLEEP_ITERATIONS = 2500;
            for (uint i = 0; i < aIterations; i++)
            {
                Factorial(aFactorialNumber);
                if (!_stopEvent.WaitOne(0))
                {
                    if ((aIterations > SLEEP_ITERATIONS) &&
                        (i % SLEEP_ITERATIONS) == 1)
                        Thread.Sleep(0);
                }
                else
                    return false;

                if (i % 100 == 50 && ev.WaitOne(0) == true)
                    lcc();
            }
            return true;
        }
        private ulong Factorial(ulong n)
        {
            //---------------------------------------------- 
            // Intel Corporation 
            // Copyright © 2009/2010 - All Rights Reserved 
            // Department : SST/NTE 
            // Written by : Bill Hines - william.l.hines@intel.com 
            // Modified by : N/A 
            // Date : 12/4/2009 
            //---------------------------------------------- 
            if (n > 1)
                return (n * Factorial(n - 1));
            else return n;
        }
        #endregion

    }
}
