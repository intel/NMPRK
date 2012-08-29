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
