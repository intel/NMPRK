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
 * Author: Software Forge, Inc											  *
 **************************************************************************/

#if !defined NMPRK_MUTEX_H_
#define NMPRK_MUTEX_H_

#include "config.h"
#include <cassert>

#if defined WIN32
#include "Windows.h"
#else
extern "C"
{
#include <pthread.h>
}
#endif

class Mutex
{
public:
    Mutex(bool isRecursive = true) {
#if defined WIN32
        if (isRecursive) {
            InitializeCriticalSectionAndSpinCount(&m_Mutex, 4000);
        } else {
            assert(0 && "Not implemented yet");
        }
#else
        pthread_mutexattr_t attr;
            
        pthread_mutexattr_init(&attr);
        if (isRecursive) {
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        } else {
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
        }
        pthread_mutex_init(&m_Mutex, &attr);
#endif
    }
    virtual ~Mutex() {
#if defined WIN32
        DeleteCriticalSection(&m_Mutex);
#else
        pthread_mutex_destroy(&m_Mutex);
#endif
    }

public:
    void Lock(void) {
#if defined WIN32
        EnterCriticalSection(&m_Mutex);
#else
        pthread_mutex_lock(&m_Mutex);
#endif
    }
    void Unlock(void) {
#if defined WIN32
        LeaveCriticalSection(&m_Mutex);
#else
        pthread_mutex_unlock(&m_Mutex);
#endif
    }

private:
    Mutex(const Mutex&);
    Mutex& operator=(const Mutex&);

private:
#if defined WIN32
    CRITICAL_SECTION m_Mutex;
#else
    pthread_mutex_t  m_Mutex;
#endif
};

class AutoMutex
{
public:
    AutoMutex(Mutex& rMutex)
        : m_rMutex(rMutex) {m_rMutex.Lock();}
    ~AutoMutex() {m_rMutex.Unlock();}

private:
    AutoMutex(const AutoMutex&);
    AutoMutex& operator=(const AutoMutex&);
        
private:
    Mutex& m_rMutex;
};

#endif


