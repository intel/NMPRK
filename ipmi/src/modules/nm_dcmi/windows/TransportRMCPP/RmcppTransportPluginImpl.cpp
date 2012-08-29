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


#include "transport.h"

#include "RmcppTransportPluginImpl.h"
#include "OsSpecificSockets.h"
#include "RmcppIpmiSession.h"
#include "TransportCommon.h"
#include "RemoteLogger.h"

#include <stdarg.h>

using namespace std;
using namespace TransportPlugin;

namespace RmcppTransportPlugin
{

RmcppTransportPluginImpl* RmcppTransportPluginImpl::instance = NULL;

RmcppTransportPluginImpl::RmcppTransportPluginImpl()
                        :ipmiSession(NULL), ip("127.000.000.001"),
                         userName(""),password(""),
                         authType(AUTH_TYPE_RMCPP),
                         authAlgo(AUTH_ALGO_RAKP_NONE), 
                         integAlgo(INTEG_ALGO_NONE), 
                         confidAlgo(CONFID_ALGO_NONE),
                         doPing(false)
                        
{
#ifdef	WIN32
    // Init Windows Sockets
    WSADATA wsaData;
    int rc = WSAStartup(MAKEWORD(REQ_WINSOCK_VER_MAJOR, REQ_WINSOCK_VER_MINOR), &wsaData);
    if (NO_ERROR != rc)
    {
        const char* errorMsg = "Uknown WinSock intialization error";
        if (WSASYSNOTREADY == rc)
        {
            errorMsg = "Network subsystem is not ready for communication";
        }
        else if (WSAVERNOTSUPPORTED == rc)
        {
            errorMsg = "Requested version of Windows Sockets is not supported";
        }
        throw runtime_error(errorMsg);
    }
    //prevent debug builds from filling 0xFD
    _CrtSetDebugFillThreshold( 0);
#endif // WIN32
}

RmcppTransportPluginImpl::~RmcppTransportPluginImpl()
{
    delete ipmiSession;

#ifdef	WIN32
    // Clean up Windows Sockets
    WSACleanup();
#endif // WIN32
}

RmcppTransportPluginImpl*
RmcppTransportPluginImpl::getInstance()
{
    if (NULL == instance)
    {
        instance = new RmcppTransportPluginImpl;
    }
    return instance;
}

void
RmcppTransportPluginImpl::initialize()
{
    getInstance();
}

void
RmcppTransportPluginImpl::destroy()
    throw()
{
    try
    {
        if (instance)
        {
            delete instance;
            instance = NULL;
        }
    }
    catch (...)
    {
    }
}

int
RmcppTransportPluginImpl::start(const dcmi_init_info_t* initInfo)
{
    if (initInfo == NULL)
    {
        return E_OTHER;
    }

    RemoteLogger::setLogPrefix("RMCPP  transport");
    RemoteLogger::setLoggerFunc(initInfo->loggerFun);

    return E_OK;
}

int
RmcppTransportPluginImpl::shutdown()
{
	/*1. Memories have been freed as and when they get created. 
	     So it is not necessary to free the memory in Shutdown().
	  2. RMCPP does not use libraries, and unloading itself does not make sense, 
	     so there are none to unload.*/
		return E_OK;
}

IPMI_SESSION_HANDLE 
RmcppTransportPluginImpl::createSession()
{
	static int sessionNum = 0;

	sessionNum++;

	RmcppIpmiSession *s = new RmcppIpmiSession(ip.c_str(), userName.c_str(), password.c_str(), authAlgo, integAlgo, confidAlgo);
	sessionMap[sessionNum] = s;

	return sessionNum;
}

void 
RmcppTransportPluginImpl::destroySession(IPMI_SESSION_HANDLE h)
{
	std::map<unsigned int, RmcppIpmiSession *>::iterator it = sessionMap.find(h);
	if(it != sessionMap.end())
	{
		delete it->second;
		sessionMap.erase(it);
	}
}

IpmiSession *
RmcppTransportPluginImpl::validateIpmiSessionHandle(IPMI_SESSION_HANDLE h)
{
	IpmiSession *theSession = NULL;

	if(h == 0)
	{
		if(ipmiSession == NULL)
		{
			RmcppIpmiSession* rmcppSes = new RmcppIpmiSession(ip.c_str(), userName.c_str(), password.c_str(), authAlgo, integAlgo, confidAlgo);
            if (!kG.empty())
            {
                LOG_HIDEBUG << "Using KG: " << kG;
                rmcppSes->setKg(kG);
            }
            
            ipmiSession = rmcppSes;
		}

		theSession = ipmiSession;
	}
	else
	{
		std::map<unsigned int, RmcppIpmiSession *>::iterator it = sessionMap.find(h);
		if(it != sessionMap.end())
		{
			theSession = it->second;
		}
	}

	if(theSession == NULL)
	{
		std::stringstream ss;
		ss << "Invalid session handle: " << h;
		throw std::exception(ss.str().c_str());
	}

	return theSession;
}

int
RmcppTransportPluginImpl::configure(IPMI_SESSION_HANDLE h, const char* paramName, const char* value)
    throw()
{
	IpmiSession *theSession = validateIpmiSessionHandle(h);
	RmcppIpmiSession *theRmcppSession = dynamic_cast<RmcppIpmiSession *>(theSession);

    int optionNumber = match(paramName, 10, 
                             "targetip", "username", "password", 
                             "confid", "integ", "format", 
                             "bmcKey", "ping","ciphersuite","send_sol",
                             NULL);
	int RetVal = E_OK;

	if (value == NULL)
	{
		LOG_HIDEBUG<<"configure called with NULL value\n";
		return E_INVALID_VALUE;
	}

    switch (optionNumber)
    {
    case 0:
        //If TargetIP is valid the assign the IP to make a connection. Else return an error.
        RetVal = CheckTargetIP(value);//const_cast<char *>(value));
		if(RetVal == E_OK)
			ip = value;
		else
			return RetVal;
		
		theSession->setTargetIp(ip.c_str());
        break;

    case 1:
        // If username is valid then assign the username.
		RetVal = CheckUserName(value);
		if(RetVal == E_OK)
		{
			userName = value;
		}
		else
		{
			LOG_HIDEBUG<<"UserName is empty, using NULL user account";
			return RetVal;
		}

		theSession->setUsername(userName.c_str());
        break;

    case 2:
        //If Password is valid then assign the password.
		RetVal = CheckPassword(value);
		if(RetVal == E_OK)
			password = value;
		else
			return RetVal;
        
		theSession->setPassword(password.c_str());
        break;

    case 3:
		//Confidentiality algorithm to use for RMCP+ session encryption.
        optionNumber = match(value, 2, "NONE", "AES_CBC_128", NULL);
        switch (optionNumber)
        {
        case 0:
            confidAlgo = CONFID_ALGO_NONE;
            break;

        case 1:
            confidAlgo = CONFID_ALGO_AES_CBC_128;
            break;

        default:
			LOG_HIDEBUG<<"ERROR: Invalid Confidentiality algorithm to "
				"use for RMCP+ session encryption.\n";
			LOG_HIDEBUG<<"Expected Confidentiality algorithm is 'NONE' and 'AES_CBC_128'\n";
			return E_INVALID_VALUE;
        }

		if(theRmcppSession != NULL)
			theRmcppSession->setConfidAlgo(confidAlgo);
        break;

    case 4:
		//Integrity algorithm to use for RMCP+ session integrity.
        optionNumber = match(value, 4, "NONE", "HMAC_SHA1_96", "HMAC_MD5_128", "MD5_128", NULL);
        switch (optionNumber)
        {
        case 0:
            integAlgo = INTEG_ALGO_NONE;
            break;

        case 1:
            integAlgo = INTEG_ALGO_HMAC_SHA1_96;
            break;

        case 2:
            integAlgo = INTEG_ALGO_HMAC_MD5_128;
            break;

        case 3:
            integAlgo = INTEG_ALGO_MD5_128;
            break;

        default:
			LOG_HIDEBUG<<"ERROR: Invalid Integrity algorithm to use for RMCP+ session integrity.\n";
			LOG_HIDEBUG<<"Expected Integrity algorithm: 'NONE', 'HMAC_SHA1_96', 'HMAC_MD5_128', 'MD5_128'\n";
			return E_INVALID_VALUE;
        }

		if(theRmcppSession != NULL)
			theRmcppSession->setIntegAlgo(integAlgo);
        break;

    case 5:
        //format		
        optionNumber = match(value, 4, "RMCPP_MD5",
                             "RMCPP_SHA", "RMCPP_NONE", "SOL_NONE", NULL);
        switch (optionNumber)
        {
        case 0:
            //RMCPP_MD5 - IPMI 2.0
            authType = AUTH_TYPE_RMCPP;
            authAlgo = AUTH_ALGO_RAKP_HMAC_MD5;
            break;

        case 1:
            //RMCPP_SHA
            authType = AUTH_TYPE_RMCPP;
            authAlgo = AUTH_ALGO_RAKP_HMAC_SHA1;
            break;

        case 2:
            //RMCPP_NONE
            authType = AUTH_TYPE_RMCPP;
            authAlgo = AUTH_ALGO_RAKP_NONE;
            break;

        case 3:
			return E_INVALID_VALUE;
            break;

        default:
			LOG_HIDEBUG<<"ERROR: Invalid Format Option to use for RMCP+ session.\n";
			LOG_HIDEBUG<<"Expected Format Options: 'RMCPP_MD5', 'RMCPP_SHA', 'RMCPP_NONE'\n";

			return E_INVALID_VALUE;

        }

		if(theRmcppSession != NULL)
			theRmcppSession->setAuthAlgo(authAlgo);
        break;

    case 6:
        {
            if (strlen(value) != 40)
                return E_INVALID_VALUE; //expecting hex string representing 20 bytes
            const char* pVal = value;
            for (int i = 0; i < 20; ++i)
            {
                char cv[3];
                cv[0] = *pVal++;
                cv[1] = *pVal++;
                cv[2] = '\0';
                char* e;
                byte_t nextByte = static_cast<byte_t>(strtoul(cv, &e, 16));
                kG += nextByte;
            }

			if(theRmcppSession != NULL)
				theRmcppSession->setKg(kG);
        }
        break;

    case 7:
        doPing = strcmp(value, "true") == 0;
        break;

	case 8:
		//CipherSuite
		//DCMI Table 4-1 Supported Cipher Suites
		//this could be done better,  but its clear

		optionNumber = match(value, 13, "0","1","2","3","4","5","6",
											"7","8","9","10","11","12", NULL); 
		
	
		switch (optionNumber)
		{
			case 0:
				authType =    AUTH_TYPE_RMCPP;
				authAlgo = AUTH_ALGO_RAKP_NONE;
				integAlgo = INTEG_ALGO_NONE;
				confidAlgo = CONFID_ALGO_NONE;
				break;
			case 1:
				authType =    AUTH_TYPE_RMCPP;
				authAlgo = AUTH_ALGO_RAKP_HMAC_SHA1;
				integAlgo = INTEG_ALGO_NONE;
				confidAlgo = CONFID_ALGO_NONE;
				break;
			case 2:
				authType =    AUTH_TYPE_RMCPP;
				authAlgo = AUTH_ALGO_RAKP_HMAC_SHA1;
				integAlgo = INTEG_ALGO_HMAC_SHA1_96;
				confidAlgo = CONFID_ALGO_NONE;
				break;
			case 3:
				authType =    AUTH_TYPE_RMCPP;
				authAlgo = AUTH_ALGO_RAKP_HMAC_SHA1;
				integAlgo = INTEG_ALGO_HMAC_SHA1_96;
				confidAlgo = CONFID_ALGO_AES_CBC_128;
				break;
			case 4:
				LOG_HIDEBUG<<"CipherSuite 4 is not a supported option.\n";
				return E_INVALID_VALUE;
				break;
			case 5:
				LOG_HIDEBUG<<"CipherSuite 5 is not a supported option.\n";
				return E_INVALID_VALUE;
				break;
			case 6:
				authType =    AUTH_TYPE_RMCPP;
				authAlgo = AUTH_ALGO_RAKP_HMAC_MD5;
				integAlgo = INTEG_ALGO_NONE;
				confidAlgo = CONFID_ALGO_NONE;
				break;
			case 7:
				authType =    AUTH_TYPE_RMCPP;
				authAlgo = AUTH_ALGO_RAKP_HMAC_MD5;
				integAlgo = INTEG_ALGO_HMAC_MD5_128;
				confidAlgo = CONFID_ALGO_NONE;
				break;
			case 8:
				authType =    AUTH_TYPE_RMCPP;
				authAlgo = AUTH_ALGO_RAKP_HMAC_MD5;
				integAlgo = INTEG_ALGO_HMAC_MD5_128;
				confidAlgo = CONFID_ALGO_AES_CBC_128;
				break;
			case 9:
				LOG_HIDEBUG<<"CipherSuite 9 is not a supported option.\n";
				return E_INVALID_VALUE;
				break;
			case 10:
				LOG_HIDEBUG<<"CipherSuite 10 is not a supported option.\n";
				return E_INVALID_VALUE;
				break;
			case 11:
				authType =    AUTH_TYPE_RMCPP;
				authAlgo = AUTH_ALGO_RAKP_HMAC_MD5;
				integAlgo = INTEG_ALGO_MD5_128;
				confidAlgo = CONFID_ALGO_NONE;
				break;
			case 12:
				authType =    AUTH_TYPE_RMCPP;
				authAlgo = AUTH_ALGO_RAKP_HMAC_MD5;
				integAlgo = INTEG_ALGO_MD5_128;
				confidAlgo = CONFID_ALGO_AES_CBC_128;
				break;
			default:
				LOG_HIDEBUG<<"CipherSuite option is Out-Of-Range. "
					"Supported options are '0, 1, 2, 3, 6, 7, 8, 11, 12'.\n";
				return E_INVALID_VALUE;
		}

		if(theRmcppSession != NULL)
		{
			theRmcppSession->setAuthAlgo(authAlgo);
			theRmcppSession->setIntegAlgo(integAlgo);
			theRmcppSession->setConfidAlgo(confidAlgo);
		}
		break;
	case 9:
		//SEND_SOL
		if (theSession != NULL)
		{
			theSession->session_send_sol = strcmp(value, "true") == 0;
		}
		break;
	
	default:
			LOG_HIDEBUG<<"Invalid ParamName. Please enter valid setting for ParamName.\n";
        	return E_NAME_UNKNOWN;

    }
    return E_OK;
}

int
RmcppTransportPluginImpl::connect(IPMI_SESSION_HANDLE h)
    throw()
{
	IpmiSession *theSession = validateIpmiSessionHandle(h);

	//Checking for "Empty" in CheckTargetIP() Added here also in case if user calls connect with configure.
	if (ip.empty())
	{
		LOG_ERR << "ERROR: connect(): IP not configured";
		return E_NOT_CONFIGURED;
	}

#ifdef CIPHER_AUTOCHECK
	if (theSession != NULL)
    {
		//throw logic_error("Connection is already established. ");
		LOG_HIDEBUG << "Connection is already established. Another session "
			"can be established only after the current session has been disconnected.\n";
		return E_OTHER;
    }
#endif
    try
    {
        LOG_HIDEBUG << "Connecting to " << ip << " as " << userName;
		/*
		if (authType == AUTH_TYPE_RMCPP)//RMCPP session
        {
            RmcppIpmiSession* rmcppSes = new RmcppIpmiSession(ip.c_str(), userName.c_str(), password.c_str(), authAlgo, integAlgo, confidAlgo);
            if (!kG.empty())
            {
                LOG_HIDEBUG << "Using KG: " << kG;
                rmcppSes->setKg(kG);
            }
            
			
            ipmiSession = rmcppSes;
        }
        else
        {
			throw runtime_error("RMCPP: incorrect auth type in response header");
            //ipmiSession = new IolIpmiSession(ip.c_str(), userName.c_str(), password.c_str(), authType);

        }
		*/

        theSession->open();
    }
    catch (exception& e)
    {
        LOG_HIDEBUG << "\nERROR: connect(): " << e.what() << endl;
        return E_NETWORK_PROBLEM;
    }
    catch (...)
    {
        return E_OTHER;
    }
    return E_OK;
}

int
RmcppTransportPluginImpl::sendIpmiCmd(IPMI_SESSION_HANDLE h, const dcmi_req_t* req, dcmi_rsp_t* rsp)
    throw()
{
	IpmiSession *theSession = validateIpmiSessionHandle(h);

    if (req == NULL)
    {
        LOG_ERR << "\nERROR: cannot handle NULL request" << endl;
        return E_OTHER;
    }
    if (rsp == NULL)
    {
        LOG_ERR << "\nERROR: cannot copy to NULL response" << endl;
        return E_OTHER;
    }
    try
    {
		
        theSession->sendRecv(*req, *rsp);
    }
    catch (exception& e)
    {
        LOG_HIDEBUG << "\nERROR: SendIPMICmd: " << e.what() << endl;
        return E_NETWORK_PROBLEM;
    }
    catch (...)
    {
        return E_OTHER;
    }
    return E_OK;
}


int
RmcppTransportPluginImpl::disconnect(IPMI_SESSION_HANDLE h)
    throw()
{
	IpmiSession *theSession = validateIpmiSessionHandle(h);

	//Check for the connection exists. If not return Error.
	if (theSession == NULL)
	{
		LOG_HIDEBUG<<"Connection has not been established. Unable to Disconnect.";
		return E_NOT_CONNECTED;
	}
	
    try
    {
        theSession->close();
        //delete ipmiSession;
#ifdef CIPHER_AUTOCHECK
		//ipmiSession = NULL;
#endif
    }
    catch (exception& e)
    {
        LOG_HIDEBUG << "\nERROR: disconnect(): " << e.what() << endl;
		//LOG_HIDEBUG << "\nUnable to close session, exiting test suite"<<endl;
        //delete ipmiSession;
        //ipmiSession = NULL;
        return E_NETWORK_PROBLEM;
    }
    catch (...)
    {
    	//ipmiSession = NULL;
        return E_OTHER;
    }
    return E_OK;
}

int
RmcppTransportPluginImpl::match(const char* base, int MAX_STRINGS, const char* strings, ...)
{
    const char* compare;
    int stringNumber;
    va_list list;

    //The base string to compare with other strings (quit if NULL)
    if (base == NULL)
        return -1;

    //compare all the other strings (quit when NULL)
    va_start(list, strings);
    compare = (char *)strings;
    stringNumber = 0;
    while ((compare != NULL) && stringNumber < MAX_STRINGS)
    {
        if (strcmp(base, compare) == 0)
        {
            return stringNumber;
        }
        //no luck, try the next one
        compare = va_arg(list, char*);
        stringNumber++;
    }
    return -1;
}

int RmcppTransportPluginImpl::CheckTargetIP(const char* value)
{
	int Int_Value = 0, Ele_Cnt = 0;
	size_t IP_Length = 0;
	char value_Local[IP_MAX_LEN]; //Maximum IP Address element allowed for IPv4 is 15 i.e xxx.xxx.xxx.xxx
	char IP_Delim[] = "."; //Delimiter "." for splitting the IP address
	char *valuePtr = NULL;
	
	
	//Initialize array variables to 0
	memset ( value_Local, NULL, IP_MAX_LEN*sizeof (char) );

	if(value==NULL)
	{
		LOG_HIDEBUG<<"ERROR: IP Address cannot be initialised with 'NULL'."
			"Expected format is [0-255].[0-255].[0-255].[0-255]\n";
		return E_INVALID_VALUE;
	}
	else
	{
		IP_Length = strlen(value); //Warning for conversion from 'size_t' to 'int', possible loss of data.
		
		//IP Address string is "Empty"
		if(IP_Length == 0)
		{
			LOG_HIDEBUG<<"ERROR: IP Address cannot be Empty. Expected format "
				"is [0-255].[0-255].[0-255].[0-255]\n";
			return E_INVALID_VALUE;
		}
		if(IP_Length <= IP_MAX_LEN)//"xxx.xxx.xxx.xxx\0"
		{
			//Copy the parameter value to local array to verify the IP address.
			strcpy(value_Local, value);
		
			//Find the substring seperated by delimiter "."
  			valuePtr = strtok(value_Local, IP_Delim);

			if(valuePtr == NULL)
			{
				LOG_HIDEBUG<<"IP Address cannot be parsed. Please enter valid IP Address.\n";
				return E_INVALID_VALUE;
			}
			else
			{
			
			while(valuePtr != NULL && Ele_Cnt < 4)
			{
				//Check the IP address if it has any character. 
				for(size_t i = 0;i < strlen(valuePtr);i++)
				{
					if (isdigit(valuePtr[i]) == 0)
					{
						LOG_HIDEBUG<<"ERROR: IP Address cannot contain characters.\n";
						return E_INVALID_VALUE;
					}
				}
			
				Int_Value =  atoi(valuePtr);
				
				Ele_Cnt++; //Counts the number of Octets in IP address
				if(Int_Value < 0  || Int_Value > 255)
				{
					LOG_HIDEBUG<<"\nERROR: IP Address is Out-Of-Range. Expected between "
						"0 - 255 for each octet.\n";
					return E_INVALID_VALUE;
				}
									
				valuePtr = strtok(NULL, IP_Delim);
			}
    
			// Four octets for IP i.e x.x.x.x) and if all four octets are zeroes
			if (valuePtr != NULL )
			{
				LOG_HIDEBUG<<"\nERROR: IP ADdress has more than 4 octets. "
					"Expected IP Address format: [0-255].[0-255].[0-255].[0-255]\n";
				return E_INVALID_VALUE;
			}
			}
		
		}
		else
		{
			LOG_HIDEBUG<<"ERROR: IP Address length is bigger. Expected IP Address - xxx.xxx.xxx.xxx\n";
			return E_INVALID_VALUE;
		}
        return E_OK;
	}
}



int RmcppTransportPluginImpl::CheckUserName(const char* value)
{
	//Check the username for maximum length of 16 bytes.
	//username for string should be ASCII.
	
	//Check for the "NULL" username
	if(value == NULL)
	{
		LOG_HIDEBUG << "ERROR: Username has been initialised with 'NULL'. Please enter valid username.";
		return E_INVALID_VALUE;
	}
	else
	{
		//Check for the length of the username entered for maximum of 16 bytes.
		if ((int)strlen(value) > USERNAME_MAX_LEN)
		{
			//Username is not in valid format
			LOG_HIDEBUG<<"\nERROR: Invalid Username Length. Maximum allowed is 16 characters. ";
		    return E_INVALID_VALUE;
		}
		else
		{
			while(*value != '\0') 
			{
				if(*value < 48 || *value > 122) // 'a' and 'Z'
				{
					LOG_HIDEBUG<<"ERROR: Username has invalid ASCII character.\n";
					return E_INVALID_VALUE;
				}
				*value++;
			}
			//debug - need to remove 
			//LOG_HIDEBUG<<"Valid UserName";
		}

	}
	return E_OK;

}

int RmcppTransportPluginImpl::CheckPassword(const char* value)
{
	//Password should be max of 20 bytes.

	//Password is NULL
	if(value == NULL)
	{
		LOG_HIDEBUG << "ERROR: Password has been initialised with 'NULL'. Please enter valid password.";
		return E_INVALID_VALUE;
	}
	else
	{
		//Password length greater than 20 bytes.
		if ((int)strlen(value) > PASSWORD_MAX_LEN)//#define 
		{
			LOG_HIDEBUG<<"\nERROR: Invalid Password Length. Maximum allowed is 20 characters.";
		    return E_INVALID_VALUE;
		}
	}
	
	return E_OK;
}

int RmcppTransportPluginImpl::sendSessionPayload(IPMI_SESSION_HANDLE h, const dcmi_req_t* req)
    throw()
{
	IpmiSession *theSession = validateIpmiSessionHandle(h);

    if (req == NULL)
    {
        LOG_ERR << "ERROR: cannot handle NULL request" << endl;
        return E_OTHER;
    }
    try
    {
		dcmi_rsp_t dummy; 
		bool timeoutDummy;
		timeval tvDummy;
		tvDummy.tv_sec = 0;
		tvDummy.tv_usec = 0;
        theSession->sendRecv(tvDummy, true, 
					 false, timeoutDummy, 
					 *req, dummy);	
    }
    catch (exception& e)
    {
        LOG_ERR << "ERROR: Sending: " << e.what() << endl;
        return E_NETWORK_PROBLEM;
    }
    catch (...)
    {
        return E_OTHER;
    }
    return E_OK;
}

int
RmcppTransportPluginImpl::recvSessionPayload(IPMI_SESSION_HANDLE h, const timeval timeoutVal, bool *timeout, dcmi_rsp_t* rsp)
    throw()
{
	IpmiSession *theSession = validateIpmiSessionHandle(h);

    if (rsp == NULL || timeout == NULL)
    {
        LOG_ERR << "ERROR: cannot copy to NULL response" << endl;
        return E_OTHER;
    }
    try
    {
		dcmi_req_t dummy; 
        theSession->sendRecv(timeoutVal, false, 
					 true, *timeout, 
					 dummy, *rsp);
    }
    catch (exception& e)
    {
        LOG_ERR << "ERROR: Receiving: " << e.what() << endl;
        return E_NETWORK_PROBLEM;
    }
    catch (...)
    {
        return E_OTHER;
    }
    return E_OK;
}

}

