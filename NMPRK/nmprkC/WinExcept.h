/*******************************************************************************
********************************************************************************
***                                                                           **
***							INTEL CONFIDENTIAL								  **
***                    COPYRIGHT 2008 INTEL CORPORATION                       **
***							All Rights Reserved								  **
***                                                                           **
***                INTEL CORPORATION PROPRIETARY INFORMATION                  **
***                                                                           **
***		The source code contained or described herein and all documents		  **
***		related to the source code ("Material") are owned by Intel			  **
***		Corporation or its suppliers or licensors. Title to the Material	  **
***		remains with Intel Corporation or its suppliers and licensors.		  **
***		The Material contains trade secrets and proprietary and confidential  **
***		information of Intel or its suppliers and licensors.				  **
***		The Material is protected by worldwide copyright and trade secret	  **
***		laws and treaty provisions. No part of the Material may be used,	  **
***		copied, reproduced, modified, published, uploaded, posted,			  **
***		transmitted, distributed, or disclosed in any way without Intel’s	  **
***		prior express written permission.									  **
***																			  **
***		No license under any patent, copyright, trade secret or other		  **
***		intellectual property right is granted to or conferred upon you by	  **
***		disclosure or delivery of the Materials, either expressly, by		  **
***		implication, inducement, estoppel or otherwise. Any license under	  **
***		such intellectual property rights must be express and approved by	  **
***		Intel in writing.													  **
********************************************************************************/

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
