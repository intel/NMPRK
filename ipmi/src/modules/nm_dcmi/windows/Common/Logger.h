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


#ifndef _INBAND_TRANSPORT_PLUGIN_LOGGER_H
#define _INBAND_TRANSPORT_PLUGIN_LOGGER_H

#include <sstream>
#include <vector>
#include <string>

#include "LogLevel.h"
#include "OsSpecificCritSect.h"

namespace TransportPlugin
{

/**
 * Defines an interface for a log sink used by the Logger class.
 *
 * There is a log level associated with each message and a verbosity level
 * associated with each sink. Message is logged if its log level is smaller
 * or equal to sink's verbosity level.
 */
class LogSink
{
public:
    /**
     * Ctor.
     *
     * @param logLevel Logging verbosity level.
     */
    explicit LogSink(int logLevel);

    /**
     * Virtual dtor to avoid memory leaks.
     */
    virtual ~LogSink();

    /**
     * Logs a message. If the implementation is used
     * only with the Logger, it does not need to perform
     * thread synchronization as the Logger does it.
     * 
     * @param msgLevel Log level of the current message.
     * @param msg Message to log.
     */
    virtual void log(unsigned int msgLevel, const char* msg) = 0;

    /**
     * Modifies logging verbosity level.
     * 
     * @param logLevel Logging verbosity level.
     */
    void setLogLevel(unsigned int logLevel);

protected:
    /// Logging verbosity level.
    unsigned int logLevel;
};

/**
 * Implementation of the LogSink that wraps STL std::ostream
 * instance.
 */
class LogSinkOstream : public LogSink
{
public:
    /**
     * Ctor.
     * @param os Output stream that is used to log the messages.
     * @param logLevel Logging verbosity level.
     */
    LogSinkOstream(std::ostream& os, int logLevel, std::string streamName);

    /**
     * Logs a message to the associated output stream.
     * 
     * @param msgLevel Log level of the current message.
     * @param msg Message to log.
     */
    virtual void log(unsigned int msgLevel, const char* msg);

private:
    /// Reference to the associated output stream.
    std::ostream& os;

	std::string streamName;
};

/**
 * Logs a single message to the associated sinks.
 * Note that setLogPrefix(), setOutputStream(), addLogSink(), setLogLevel()
 * are static and configure the logging mechanism, not a single log entry.
 */
class Logger : public std::ostringstream
{
public:
    /**
     * Ctor
     */
    Logger();

    /**
     * Dtor that does the message logging job.
     */
    ~Logger();

    /**
     * Sets the severity of the message being logged by the current instance of the logger.
     * See the LOG_* macros for examples on the log levels that are in use.
     *
     * @param msgLevel Specifies message level of the current log message.
     */
    inline Logger& operator ()(unsigned int msgLevel);

    /**
     * Assignes an STL output stream to the Logger. This method is added for simplicity,
     * internally a LogSinkOstream instance is created and added by the addLogSink() API.
     * setOutputStream() may be called many times for various output streams.
     *
     * @param os Output stream to log messages to.
     * @param logLevel Logging verbosity level
     */
    static void setOutputStream(std::ostream& os, unsigned int logLevel, std::string streamName = "");

    /**
     * Adds a log sink to the logging facility.
     * Note that LogSink is owned by the Logger, it will free the memory.
     *
     * @param logSink Pointer to the log sink. Do not delete the object once
     *    it is added to the Logger.
     */
    static void addLogSink(LogSink* logSink);

	/**
     * Allows to configure prefix text which is added to each log message.
     *
     * @param logPrefix prefix to be defined.
     */
    static void setLogPrefix(const char* logPrefix);

    /**
     * Modifies logging verbosity level for all sinks.
     * 
     * @param logLevel Logging verbosity level.
     */
    static void setLogLevel(unsigned int logLevel);

    /**
     * Enables logging of message which is already
     * formatted by external entity.
     * @param msgLevel Log level of message to be logged.
     * @param msg Message to be logged.
     * @retval 0 Successful logging
     * @retval <>0 Other error.
     */
    static int log(int msgLevel, const char* msg);

private:
    /// Log level of the current message.
    unsigned int msgLevel;

    /**
     * Static member that keeps  all logging facility settings together.
     */
    static struct LogCore
    {
        /**
         * Ctor.
         */
        LogCore();

        /**
         * Dtor deletes all associated log sinkgs.
         */
        ~LogCore();

        /**
         * Logs a message to all associated sinks.
         * This method is synchronized.
         *
         * @param msgLevel Log level of the current message.
         * @param msg Message to log.
         */
        void log(unsigned int msgLevel, const char* msg);

        /// Critical section used to synchronize <code>log()</code>
        TransportPlugin::OsSpecificCritSect *pCritSect;

        /// Collection of sinks.
        std::vector<LogSink*> sinks;

	    /// Prefix which is added to each log message.
        std::string logPrefix;
    } logCore;
};

inline
Logger&
Logger::operator ()(unsigned int msgLevel)
{
    this->msgLevel = msgLevel;
    return *this;
}

}

/// Creates a log message that is logged on error level.
#define LOG_ERR TransportPlugin::Logger()((unsigned int)TransportPlugin::logLevelErrors)

/// Creates a log message that is logged on warning level.
#define LOG_WARN TransportPlugin::Logger()((unsigned int)TransportPlugin::logLevelWarnings)

/// Creates a log message that is logged on summary information level.
#define LOG_SUMMARY TransportPlugin::Logger()((unsigned int)TransportPlugin::logLevelSummary)

/// Creates a log message that is logged on detailed information level.
#define LOG_DETAIL TransportPlugin::Logger()((unsigned int)TransportPlugin::logLevelDetails)

/// Creates a log message that is logged on high-level debug information level.
#define LOG_HIDEBUG TransportPlugin::Logger()((unsigned int)TransportPlugin::logLevelHiDebug)

/// Creates a log message that is logged on low-level debug information level.
#define LOG_LODEBUG TransportPlugin::Logger()((unsigned int)TransportPlugin::logLevelLowDebug)

#endif //_INBAND_TRANSPORT_PLUGIN_LOGGER_H
