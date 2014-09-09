using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace NMPRK_GUI
{
    class NMPRKReader
    {
        private static Dictionary<string, string> FunctionDictionary = new Dictionary<string, string>();
        private static Dictionary<string, string> StructureDictionary = new Dictionary<string, string>();

        public static void ReadFunctionsHeaderFile()
        {
            string[] lines = File.ReadAllLines("nmprkC.h");

            string signature = string.Empty;
            string functionName = string.Empty;

            foreach (string line in lines)
            {
                if (line.StartsWith("NMPRKC_API") == true)
                {
                    signature += (line + Environment.NewLine);
                }
                else if (signature != string.Empty)
                {
                    signature += (line + Environment.NewLine);

                    // capture function name
                    if (line.Trim().Contains("(") == true)
                        functionName = line.Trim().Substring(0, line.Trim().IndexOf('('));

                    // end of signature
                    if (line.Trim().Contains(";") == true)
                    {
                        // save signature
                        if (signature == string.Empty || functionName == string.Empty)
                            throw new Exception("Empty Signature or FunctionName!");

                        FunctionDictionary.Add(functionName.Trim(), signature.Trim());
                        signature = string.Empty;
                        functionName = string.Empty;
                    }
                }
            }
        }

        public static void ReadTypesHeaderFile()
        {
            string[] lines = File.ReadAllLines("nmprkTypes.h");

            string structure = string.Empty;
            string structName = string.Empty;
            int openBraceCount = 0;

            foreach (string line in lines)
            {
                if (line.StartsWith("typedef struct") == true)
                {
                    structure += (line + Environment.NewLine);

                    if (line.Contains("{") == true)
                        openBraceCount++;
                    if (line.Contains("}") == true)
                        openBraceCount--;

                    string restOfLine = line.Trim().Substring("typedef struct".Length).Trim();
                    string[] restOfLineParts = restOfLine.Split(new char[] { ' ', '{' });
                    if (restOfLineParts.Length > 1)
                        structName = restOfLineParts[0];
                    else
                        structName = restOfLine;
                }
                else if (structure != string.Empty)
                {
                    structure += (line + Environment.NewLine);

                    if (line.Contains("{") == true)
                        openBraceCount++;
                    if (line.Contains("}") == true)
                        openBraceCount--;

                    if (openBraceCount == 0 && line.Trim().Contains(";") == true)
                    {
                        // end of struct
                        if (structure == string.Empty || structName == string.Empty)
                            throw new Exception("Empty Structure or StructureName!");

                        StructureDictionary.Add(structName.Trim(), structure.Trim());
                        structure = string.Empty;
                        structName = string.Empty;
                    }
                }
                
            }
        }

        public static string GetFunctionSignature(string func)
        {
            try
            {
                return FunctionDictionary[func];
            }
            catch
            {
                return "Could not find signature for " + func;
            }
        }

        public static string GetStructure(string stru)
        {
            try
            {
                return StructureDictionary[stru];
            }
            catch
            {
                return null;
            }
        }

        public class FunctionInputOutput
        {
            public string Input = string.Empty;
            public string Output = string.Empty;
        }
        public static FunctionInputOutput GetFunctionInputOutput(string func)
        {
            try
            {
                FunctionInputOutput fio = new FunctionInputOutput();

                string signature = GetFunctionSignature(func);
                int openBraceIndex = signature.IndexOf('(');
                int closeBraceIndex = signature.IndexOf(')');

                if (openBraceIndex == -1 || closeBraceIndex == -1)
                    return fio;

                string variableString = signature.Substring(openBraceIndex + 1, (closeBraceIndex - openBraceIndex) - 1).Trim();
                string[] variables = variableString.Trim().Split(',');

                foreach (string s in variables)
                {
                    bool input = false;
                    bool output = false;
                    string type = string.Empty;

                    string[] parts = s.Trim().Split(' ');
                    foreach (string p in parts)
                    {
                        if (p == "IN")
                            input = true;
                        else if (p == "OUT")
                            output = true;
                        else if (type == string.Empty)
                        {
                            type = p;
                            if (type == "nmprk_conn_handle_t")
                                continue;

                            if (input == true)
                            {
                                string stru = GetStructure(type);
                                if (stru == null)
                                    fio.Input += (s.Substring(s.IndexOf(type)) + Environment.NewLine);
                                else
                                    fio.Input += (stru + Environment.NewLine);
                            }
                            if (output == true)
                            {
                                string stru = GetStructure(type);
                                if (stru == null)
                                    fio.Output += (s.Substring(s.IndexOf(type)) + Environment.NewLine);
                                else
                                    fio.Output += (stru + Environment.NewLine);
                            }
                            break;
                        }
                    }
                }

                return fio;
            }
            catch (Exception ex)
            {
                throw;
            }
        }
    }
}
