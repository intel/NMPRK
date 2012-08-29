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
#ifndef _C_EXCEPT_H
#define _C_EXCEPT_H

#include <stdexcept>
#include <string>
#include <errno.h>

/**
 * Base class for exceptions thrown on Linux.
 * The exception uses <code>strerror()</code> to retrieve error
 * description reported by <code>what()</code>.
 */
class CError : public std::runtime_error
{
public:
    /**
     * Ctor.
     * 
     * @param func Name of the function that failed.
     * @param err Error code retured by the function <code>func</code>.
     */
    CError(const std::string& func, int err = errno);

    /**
     * Dtor.
     */
    ~CError() throw();

    /**
     * Reports error code and description. The description
     * is retrieved using <code>strerror()</code> function.
     *
     * @return Pointer to the description of exception.
     */
    const char* what() const throw();

    /**
     * Retrieves the error code passed to the ctor.
     *
     * @return Error code passed to the ctor.
     */
    int getErr() const;

private:
    /**
     * Contains the formatted decription returned by <code>what()</code>.
     * The description is formatted on the first <code>what()</code> invocation.
     */
    mutable std::string msg;

    /// Stores the name of the function that failed.
    std::string func;

    /// Stores the error code.
    int err;
};

#endif //#ifndef _C_EXCEPT_H
