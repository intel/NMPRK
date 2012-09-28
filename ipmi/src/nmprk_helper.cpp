/***************************************************************************
 * Copyright 2012 Intel Corporation                                        *
 *Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 * http://www.apache.org/licenses/LICENSE-2.0                              *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/
/**************************************************************************
 * Author: Stewart Dale <IASI NM TEAM>                                    *
 * Updates:							                                      *
 * 4/30: prep for initial external release                                *
 **************************************************************************/
#include "../include/nmprk_helper.h"
#include "../../ipmi/include/nmprk_errCodes.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string.h>
int nmprk::helper::hexStr2Int(std::string str) {
  int x;
  std::stringstream ss;
  ss << std::hex << str;
  ss >> x;
  return x;
}

std::string nmprk::helper::int2HexStr(int i) {
 std::stringstream stream;
 stream << std::hex << i;
 return stream.str();
}

// Note that the resulting string is in LSB format, so byte[0] is == to string[0] instead of string[7]
// this is perfect for when your checking bits
std::string nmprk::helper::int2BinStr(int n) {
 std::string res;
 while (n)  {
  res.push_back((n & 1) + '0');
  n >>= 1;
 }
 std::reverse(res.begin(), res.end());
 int toFill = 8 - res.length();
 std::string fill;
 for(int i=0;i<toFill;i++)
   fill += "0";
 fill += res;
 std::reverse(fill.begin(), fill.end());
 return fill;
}

int nmprk::helper::binStr2Int(std::string s) {
 int  k;
 int  len, sum = 0;
 	
 // Software Forge Inc. --- Start ------------------------------------------
 // This function didn't appear to be implemented correctly
 len = strlen(s.c_str());
 for(k = 0; k < len; k++)
	 sum |= ((s[k] - '0') << k);
 // Software Forge Inc. --- End --------------------------------------------
 return(sum);
}

// Software Forge Inc. --- Start ------------------------------------------
// Added another helper function
std::string nmprk::helper::getByteStr(unsigned long long val, int byteIndex) {
	std::stringstream ss;
	ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << ((val >> (8 * byteIndex)) & 0x00000000000000FF);
	return ss.str();
}
// Software Forge Inc. --- End --------------------------------------------
