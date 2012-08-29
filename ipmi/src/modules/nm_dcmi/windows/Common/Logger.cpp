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

#include "Logger.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

using namespace std;

namespace TransportPlugin
{

Logger::LogCore Logger::logCore;

LogSink::LogSink(int logLevel)
        :logLevel(logLevel)
{
}

LogSink::~LogSink()
{
}

void
LogSink::setLogLevel(unsigned int logLevel)
{
    this->logLevel = logLevel;
}

LogSinkOstream::LogSinkOstream(std::ostream& os, int logLevel, std::string name)
              :os(os), LogSink(logLevel), streamName(name)
{
}

void
LogSinkOstream::log(unsigned int msgLevel, const char* msg)
{
    if (msgLevel <= this->logLevel)
    {
		if(streamName != "")
		{
#ifdef WIN32
			struct _stat64i32 filestatus;
			int ret = _stat(streamName.c_str(), &filestatus);
#else
			struct stat filestatus;
			int ret = stat(streamName.c_str(), &filestatus);		
#endif
			if(ret == 0 && filestatus.st_size > 100000000) // 100 MB
			{
				std::ofstream *ofs = (std::ofstream *)&os;
				ofs->close();
				ofs->open(streamName.c_str(), std::ios::trunc);
			}
		}
        os << msg;
        os.flush();
    }
}

Logger::LogCore::LogCore()
{
    pCritSect = OsSpecificCritSect::CreateCritSectObject();
}

Logger::LogCore::~LogCore()
{
    for (size_t i = 0; i < sinks.size(); ++i)
    {
        delete sinks[i];
    }
    sinks.clear();

    if(pCritSect != NULL)
    {
        delete pCritSect;
        pCritSect = NULL;
    }
}

void
Logger::LogCore::log(unsigned int msgLevel, const char* msg)
{
    pCritSect->EnterCritSect();
    try
    {
        for (size_t i = 0; i < sinks.size(); ++i)
        {
            sinks[i]->log(msgLevel, msg);
        }
    }
    catch (...)
    {};
    pCritSect->LeaveCritSect();
}

Logger::Logger()
{
    if (!logCore.logPrefix.empty())
    {
        *this << logCore.logPrefix << " ";
    }
}

Logger::~Logger()
{
    *this;//SM: Removed endl to display Asset Tag in a single line, Didn't wanted endl hardcoded.
    logCore.log(this->msgLevel, str().c_str());
}

void
Logger::setOutputStream(std::ostream& os, unsigned int logLevel, std::string streamName)
{
    addLogSink(new LogSinkOstream(os, logLevel, streamName));
}

void
Logger::addLogSink(LogSink* logSink)
{
    logCore.sinks.push_back(logSink);
}

void
Logger::setLogPrefix(const char* logPrefix)
{
    stringstream s;
    s << "[" << logPrefix << "]:";
    logCore.logPrefix = s.str();
}

void
Logger::setLogLevel(unsigned int logLevel)
{
    for (size_t i = 0; i < logCore.sinks.size(); ++i)
    {
        logCore.sinks[i]->setLogLevel(logLevel);
    }
}

int
Logger::log(int msgLevel, const char* msg)
{
    try
    {
        logCore.log(msgLevel, msg);
        return 0;
    }
    catch (exception&)
    {
        return 1;
    }
}

}
