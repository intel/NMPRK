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


#ifndef _LOG_SINK_ODS_H
#define _LOG_SINK_ODS_H

#include "Logger.h"

namespace TransportPlugin
{

/**
 * Implementation of the LogSink that logs messages
 * using OutputDebugString() Windows API.
 */
class LogSinkOds : public LogSink
{
public:
    /**
     * Ctor.
     * @param logLevel Logging verbosity level.
     */
    LogSinkOds(int logLevel);

    /**
     * Logs a message to the debugger.
     *
     * @param msgLevel Log level of the current message.
     * @param msg Message to log.
     */
    virtual void log(unsigned int msgLevel, const char* msg);
};

}

#endif //_LOG_SINK_ODS_H
