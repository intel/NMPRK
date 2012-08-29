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


#ifndef _OS_SPECIFIC_FUNCTIONS_H
#define _OS_SPECIFIC_FUNCTIONS_H

namespace TransportPlugin
{
/**
 **************************************************************
 * Abstract:
 *   Provides wrappers for some OS-specific functions.
 **************************************************************/
class OsSpecificFunctions
{
public:
    /**
     *************************************************************************
     * Compares two strings case insensitively.
     * @description
     * Wrapper for WIN32 _stricmp function and Linux strncmp function.
     *
     * @param str1 - string 1 to compare
     * @param str2 - string 2 to compare
     *
     * @return comparison result
     *************************************************************************/
    static int stricmp(const char* str1, const char* str2);

    /**
     *************************************************************************
     * Copies one string to another.
     * @description
     * Wrapper for WIN32 strcpy_s function and Linux strcpy function.
     *
     * @param dst - destination string
     * @param dstSize - size of destination string
     * @param src - source string
     *
     * @return error code
     *************************************************************************/
    static int strcpys(char* dst, unsigned int dstSize, const char* src);

    /**
     *************************************************************************
     * Formats string.
     * @description
     * Wrapper for WIN32 sprintf_s function and Linux snprintf function.
     *
     * @param dst - destination string
     * @param dstSize - size of destination string
     * @param format - string containing format of the destination string
     *
     * @return number of characters written or error code
     *************************************************************************/
    static int sprintfs(char* dst, unsigned int dstSize, const char* format, ...);
};
}

#endif // _OS_SPECIFIC_FUNCTIONS_H
