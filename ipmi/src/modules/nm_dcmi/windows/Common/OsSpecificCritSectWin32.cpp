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

#include "OsSpecificCritSectWin32.h"

namespace TransportPlugin
{

OsSpecificCritSect*
OsSpecificCritSect::CreateCritSectObject()
{
    return new OsSpecificCritSectWin32();
}

OsSpecificCritSectWin32::OsSpecificCritSectWin32()
{
    InitializeCriticalSection(&this->criticalSection);
}

OsSpecificCritSectWin32::~OsSpecificCritSectWin32()
{
    DeleteCriticalSection(&this->criticalSection);
}

void
OsSpecificCritSectWin32::EnterCritSect()
{
    EnterCriticalSection(&this->criticalSection);
}

void
OsSpecificCritSectWin32::LeaveCritSect()
{
    LeaveCriticalSection(&this->criticalSection);
}

}
