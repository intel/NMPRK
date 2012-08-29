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



#include "OsSpecificCritSectLinux.h"

namespace TransportPlugin
{
OsSpecificCritSect*
OsSpecificCritSect::CreateCritSectObject()
{
    return new OsSpecificCritSectLinux();
}

OsSpecificCritSectLinux::OsSpecificCritSectLinux()
{
    pthread_mutex_init(&this->mutex, NULL);
    //pthread_mutexattr_t mutexattr;
    //pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE_NP);
    //pthread_mutex_init(&this->mutex, &mutexattr);
    //pthread_mutexattr_destroy(&mutexattr);
}

OsSpecificCritSectLinux::~OsSpecificCritSectLinux()
{
    pthread_mutex_destroy(&this->mutex);
}

void
OsSpecificCritSectLinux::EnterCritSect()
{
    pthread_mutex_lock(&this->mutex);
}

void
OsSpecificCritSectLinux::LeaveCritSect()
{
    pthread_mutex_unlock(&this->mutex);
}
}
