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

#ifndef _OS_SPECIFIC_SOCKETS_H
#define _OS_SPECIFIC_SOCKETS_H

#ifdef WIN32

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#include "WinExcept.h"

#define OsSockError								WinSockError

#define REQ_WINSOCK_VER_MAJOR   2
#define REQ_WINSOCK_VER_MINOR   2

#else

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

typedef int SOCKET;
typedef struct sockaddr SOCKADDR;

#define closesocket								close
#define	INVALID_SOCKET							(-1)
#define	SOCKET_ERROR							(-1)
#define NO_ERROR								0
#define	SD_SEND									SHUT_WR
#define OsSockError								runtime_error

#endif

#endif // _OS_SPECIFIC_SOCKETS_H
