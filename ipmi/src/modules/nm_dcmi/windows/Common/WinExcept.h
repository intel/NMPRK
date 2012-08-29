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


#ifndef _WIN_EXCEPT_H
#define _WIN_EXCEPT_H

#include <winsock2.h>
#include <windows.h>
#include <stdexcept>
#include <string>

/**
 * Base class for exceptions throws on Windows API, Winsock etc. failures.
 * The exception uses <code>FormatMessage()</code> to retrieve error
 * description reported by <code>what()</code>.
 */
class WinError : public std::runtime_error
{
public:
    /**
     * Ctor.
     * 
     * @param func Name of the function that failed.
     * @param err Error code retured by the function <code>func</code>.
     */
    WinError(const std::string& func, DWORD err);

    /**
     * Reports error code and description. The description
     * is retrieved using <code>FormatMessage()</code> function.
     *
     * @return Pointer to the description of exception.
     */
    virtual const char* what() const;

    /**
     * Retrieves the error code passed to the ctor.
     *
     * @return Error code passed to the ctor.
     */
    DWORD getErr() const;

private:
    /**
     * Contains the formatted decription returned by <code>what()</code>.
     * The description is formatted on the first <code>what()</code> invocation.
     */
    mutable std::string msg;

    /// Stores the name of the function that failed.
    std::string func;

    /// Stores the error code.
    DWORD err;
};

/**
 * Specialization of the <code>WinError</code> that uses
 * <code>GetLastError()</code> to provide the default value
 * of the error code to the base class.
 * This exception may be thrown on Win API function failure.
 */
class WinApiError : public WinError
{
public:
    /**
     * Ctor.
     *
     * @param func Name of the function that failed.
     * @param err Error code.
     */
    WinApiError(const std::string& func, DWORD err = GetLastError());
};

#ifdef _WINSOCKAPI_

/**
 * Specialization of the <code>WinError</code> that uses
 * <code>WSAGetLastError()</code> to provide the default value
 * of the error code to the base class.
 * This exeption may be throw on Winsock function failure.
 */
class WinSockError : public WinError
{
public:
    /**
     * Ctor.
     *
     * @param func Name of the function that failed.
     * @param err Error code.
     */
    WinSockError(const std::string& func, DWORD err = WSAGetLastError());
};
#endif

#endif //#ifndef _WIN_EXCEPT_H
