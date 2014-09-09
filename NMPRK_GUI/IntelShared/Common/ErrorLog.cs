using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Reflection;
using System.Security.AccessControl;
using System.Diagnostics;
using System.Runtime.CompilerServices;

namespace IntelShared.Common
{
    class ErrorLog
    {
        private static StreamWriter _logStreamWriter = null;
        private static string _logFilePath = null;
        private static readonly object _syncObject = new object();

        static ErrorLog()
        {
            try
            {
                // Setup the log file
                _logFilePath = IntelShared.ProjectInfo.CommonAppDataPath;
                if (Directory.Exists(_logFilePath) != true)
                    Directory.CreateDirectory(_logFilePath);
                _logFilePath += "\\log_" + DateTime.Now.ToString("yyMMdd") + ".txt";

                try
                {
                    _logStreamWriter = new StreamWriter(_logFilePath, true);
                }
                catch (UnauthorizedAccessException)
                {
                    // give access
                    FileStream fs = File.Create(_logFilePath);
                    FileSecurity fSecurity = File.GetAccessControl(_logFilePath);
                    fSecurity.AddAccessRule(new FileSystemAccessRule("Everyone", FileSystemRights.FullControl, AccessControlType.Allow));
                    File.SetAccessControl(_logFilePath, fSecurity);
                    fs.Close();

                    _logStreamWriter = new StreamWriter(_logFilePath, true);
                }

                _logStreamWriter.AutoFlush = true;

                _logStreamWriter.Write("\n\n");
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        public static string LogFilePath
        {
            get { return _logFilePath; }
        }

        private static string GetLineFormatted(string line, string method = "")
        {
            return "(" + DateTime.Now.ToString("HH:mm:ss") + ":" + System.Threading.Thread.CurrentThread.ManagedThreadId.ToString("000") + ") " + method + " - " + line;
        }

        public static void WriteLine(string line)
        {
            lock (_syncObject)
            {
                if (_logStreamWriter != null)
                {
                    _logStreamWriter.WriteLine(GetLineFormatted(line, GetCurrentMethod(1)));
                }
            }
        }

        public static bool EnableTracing = false;

        public static void TraceEntry(string txt = "")
        {
            if (EnableTracing == true)
            {
                lock (_syncObject)
                {
                    if (_logStreamWriter != null)
                    {
                        _logStreamWriter.WriteLine(GetLineFormatted("Entry" + ((txt != "") ? ": " + txt : ""), GetCurrentMethod(1)));
                    }
                }
            }
        }

        public static void TraceExit(string txt = "")
        {
            if (EnableTracing == true)
            {
                lock (_syncObject)
                {
                    if (_logStreamWriter != null)
                    {
                        _logStreamWriter.WriteLine(GetLineFormatted("Exit" + ((txt != "") ? ": " + txt : ""), GetCurrentMethod(1)));
                    }
                }
            }
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        public static string GetCurrentMethod(int numFrames)
        {
            StackTrace st = new StackTrace();
            StackFrame sf = st.GetFrame(numFrames + 1); // +1 is for this method

            return sf.GetMethod().DeclaringType.Name + ":" + sf.GetMethod().Name;
        }

        public static void LogException(Exception ex)
        {
            lock (_syncObject)
            {
                if (_logStreamWriter != null)
                {
                    _logStreamWriter.WriteLine(GetLineFormatted("Exception Caught: " + GetCurrentMethod(1)));
                    _logStreamWriter.WriteLine(GetLineFormatted("Source: " + ex.Source));
                    _logStreamWriter.WriteLine(GetLineFormatted("Type: " + ex.GetType().ToString()));
                    _logStreamWriter.WriteLine(GetLineFormatted("Message: " + ex.Message));
                    _logStreamWriter.WriteLine(GetLineFormatted("Target Site:\n" + ex.TargetSite.ToString()));
                    _logStreamWriter.WriteLine(GetLineFormatted("Stack Trace:\n" + ex.StackTrace));
                    Exception inner = ex.InnerException;
                    while (inner != null)
                    {
                        _logStreamWriter.WriteLine(GetLineFormatted("Message: " + inner.Message));
                        _logStreamWriter.WriteLine(GetLineFormatted("Stack Trace:\n" + inner.StackTrace));
                        inner = inner.InnerException;
                    }

#if ERROR_LOG_HANDLE_SOAP_EXCEPTION
                    if (ex is System.Web.Services.Protocols.SoapException)
                    {
                        System.Web.Services.Protocols.SoapException se = ex as System.Web.Services.Protocols.SoapException;
                        _logStreamWriter.WriteLine(GetLineFormatted("Actor: " + se.Actor));
                        if (se.Code != null)
                            _logStreamWriter.WriteLine(GetLineFormatted("Code: " + se.Code));
                        if(se.Detail != null)
                            _logStreamWriter.WriteLine(GetLineFormatted("Detail: " + se.Detail.OuterXml));
                    }
#endif
                }
            }
        }

    }
}
