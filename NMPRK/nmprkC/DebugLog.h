/***************************************************************************
 **
 **   DebugLog.h
 **
 **   Copyright Software Forge Inc.  
 **
 **
 **   Proprietary and Confidential
 **   Unauthorized distribution or copying is prohibited
 **   All rights reserved
 **
 ** No part of this computer software may be reprinted, reproduced or
 ** utilized in any form or by any electronic, mechanical, or other means,
 ** now known or hereafter invented, including photocopying and recording, or
 ** using any information storage and retrieval system, without permission in
 ** writing from Software Forge Inc.
 **
 ****************************************************************************
 **
 ** File Name:    DebugLog.h
 **
 ** See Also:
 **
 ** Project:	  
 **
 ** Author:       CJ Parsons
 **
 ** Description:  
 **               
 **              
 ** Documents:
 **
 ** Notes:
 **		
 **
 ****************************************************************************/

#ifndef _DEBUG_LOG_H_
#define _DEBUG_LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include "nmprkTypes.h"

#ifdef WIN32 
#define snprintf _snprintf_s
#endif

extern std::fstream si_fsDebugLog;
extern nmprk_debug_callback debugCallback;

#define MAX_DATE_STR_LEN 10
//!

/*!
  Debug Module values
 */
typedef enum si_debug_module_t
{
    SI_DEBUG_MODULE_NONE = 0x0000, /*!< No debugging */
    SI_DEBUG_MODULE_IPMI_PROXY = 0x0001, /*!< The IPMI Proxy */
	SI_DEBUG_MODULE_RMCPP = 0x0002, /*!< The RMCPP Stuff */
	SI_DEBUG_MODULE_INTERFACE = 0x0003,
    SI_DEBUG_MODULE_ALL = 0xFFFF /*!< All modules */

} si_debug_module_t;

//!

/*!
  Debug Level values
 */
typedef enum si_debug_level_t
{
    SI_DEBUG_LEVEL_TRACE = 1, /*!< Debug at a Trace level (most output) */
    SI_DEBUG_LEVEL_INFO = 2, /*!< Debug at a Info level */
    SI_DEBUG_LEVEL_ERROR = 3, /*!< Debug at a Info level (least output) */
    SI_DEBUG_LEVEL_NONE = 4, /*!< No debugging output */
    SI_DEBUG_LEVEL_ALL = 1 /*!< All debugging output (same as TRACE) */

} si_debug_level_t;

extern si_debug_module_t si_debugModule;
extern si_debug_level_t si_debugLevel;

inline std::string current_time()
{
    char timestr[MAX_DATE_STR_LEN];
#if defined WIN32
    _strtime_s(timestr, MAX_DATE_STR_LEN);
    return timestr;
#else
    time_t mytime = time(NULL);
    strftime(timestr, 9, "%T", localtime(&mytime));
    return timestr;
#endif
}

inline std::string current_date()
{
    char datestr[MAX_DATE_STR_LEN];
    time_t mytime = time(NULL);
	strftime(datestr, 9, "%Y%m%d", localtime(&mytime));
    return datestr;
}

#define SI_DEBUG(a, b, ...) \
{ \
	if((si_debugModule & a) == a && b >= si_debugLevel) { \
		char data[1024]; snprintf(data, 1024, __VA_ARGS__); \
		if(debugCallback != NULL) \
			debugCallback(data); \
		if(si_fsDebugLog.is_open() == true) { \
			si_fsDebugLog << current_time() << " (" << a << "," << b << "): " << data << std::endl; \
		} \
	} \
}

#define SI_DEBUG_TRACE(a, ...) SI_DEBUG(a, SI_DEBUG_LEVEL_TRACE, __VA_ARGS__)
#define SI_DEBUG_INFO(a, ...) SI_DEBUG(a, SI_DEBUG_LEVEL_INFO, __VA_ARGS__)
#define SI_DEBUG_ERROR(a, ...) SI_DEBUG(a, SI_DEBUG_LEVEL_ERROR, __VA_ARGS__)
#endif // _DEBUG_LOG_H_
