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


#ifndef _REMOTE_LOGGER_H
#define _REMOTE_LOGGER_H

#include <sstream>
#include <vector>
#include <string>

#include "LogLevel.h"
#include "..\TransportKCS\transport.h"
namespace TransportPlugin
{

/**
 * Logs a single message using configured callback function.
 * Note that setLogPrefix(), setLoggerFunc() are static
 * and configures the logging mechanism, not a single log entry.
 */
class RemoteLogger : public std::ostringstream
{
public:
    /**
     * Ctor
     */
    RemoteLogger();

    /**
     * Dtor that does the message logging job.
     */
    ~RemoteLogger();

    /**
     * Sets the severity of the message being logged by the current instance of the logger.
     * See the LOG_* macros for examples on the log levels that are in use.
     *
     * @param msgLevel Specifies message level of the current log message.
     */
    inline RemoteLogger& operator ()(unsigned int msgLevel);

    /**
     * Configures logging callback function. Each message is logged using the function
     * configured with this method.
     *
     * @param loggerFun Logging function pointer.
     */
    static void setLoggerFunc(dcmi_logger_fun_t loggerFun);

    /**
     * Allows to configure prefix text which is added to each log message.
     *
     * @param logPrefix prefix to be defined.
     */
    static void setLogPrefix(const char* logPrefix);

private:
    /// Log level of the current message.
    unsigned int msgLevel;
    /// Pointer to the function which performs actual logging.
    static dcmi_logger_fun_t pLoggerFun;
    /// Prefix which is added to each log message.
    static std::string logPrefix;
};

inline
RemoteLogger&
RemoteLogger::operator ()(unsigned int msgLevel)
{
    this->msgLevel = msgLevel;
    return *this;
}

}

/// Creates a log message that is logged on error level.
#define LOG_ERR TransportPlugin::RemoteLogger()((unsigned int)TransportPlugin::logLevelErrors)

/// Creates a log message that is logged on warning level.
#define LOG_WARN TransportPlugin::RemoteLogger()((unsigned int)TransportPlugin::logLevelWarnings)

/// Creates a log message that is logged on summary information level.
#define LOG_SUMMARY TransportPlugin::RemoteLogger()((unsigned int)TransportPlugin::logLevelSummary)

/// Creates a log message that is logged on detailed information level.
#define LOG_DETAIL TransportPlugin::RemoteLogger()((unsigned int)TransportPlugin::logLevelDetails)

/// Creates a log message that is logged on high-level debug information level.
#define LOG_HIDEBUG TransportPlugin::RemoteLogger()((unsigned int)TransportPlugin::logLevelHiDebug)

/// Creates a log message that is logged on low-level debug information level.
#define LOG_LODEBUG TransportPlugin::RemoteLogger()((unsigned int)TransportPlugin::logLevelLowDebug)

// TBD: the definitions below are for the RMCPP plugin which has not been updated
// to recent changes in the logging functionality yet.
// Remove them.
#define TP_LOG LOG_ERR
#define TP_LOG_V LOG_HIDEBUG

#endif //_REMOTE_LOGGER_H
