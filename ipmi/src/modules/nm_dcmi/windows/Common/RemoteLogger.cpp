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

#include "RemoteLogger.h"

using namespace std;

namespace TransportPlugin
{

dcmi_logger_fun_t RemoteLogger::pLoggerFun = NULL;
std::string RemoteLogger::logPrefix;

RemoteLogger::RemoteLogger()
{
    // If static member logPrefix is defined, set initial value of the message.
    if (!logPrefix.empty())
    {
        *this << logPrefix << " ";
    }
}

RemoteLogger::~RemoteLogger()
{
    try
    {
        if (pLoggerFun != NULL)
        {
            *this << endl;
            (*pLoggerFun)(msgLevel, str().c_str());
        }
    }
    catch (...)
    {};
}

void 
RemoteLogger::setLoggerFunc(dcmi_logger_fun_t loggerFun)
{
    RemoteLogger::pLoggerFun = loggerFun;
}

void
RemoteLogger::setLogPrefix(const char* newLogPrefix)
{
    stringstream s;
    s << "[" << newLogPrefix << "]:";
    logPrefix = s.str();
}

}
