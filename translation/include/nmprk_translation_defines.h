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
#ifndef NMPRK_TRANSLATION_DEFINES_H
#define NMPRK_TRANSLATION_DEFINES_H

#include <time.h>

namespace nmprk {

 namespace translation {
  
  typedef enum {
   initLibrary   = 0x00,
   unInitLibrary = 0x01,
   initDevice    = 0x02,
   unInitDevice  = 0x03
  }initType_t;

  typedef enum {
   domainSystem = 0x00,
   domainCpu    = 0x01,
   domainMemory = 0x02
  }subSystemComponentType_t;
  
  typedef enum {
   policyPower   = 0x00,   
   policyThermal = 0x01
  }policyType_t;
 
  typedef enum {
   samplePower   = 0x00,
   sampleThermal = 0x01
  }sampleType_t;

  typedef enum {
   policyEnabled  = 0x00,
   policyDisabled = 0x01,
   policyUnset    = 0x02
  }policyStatusType_t;

  typedef struct {
   nmprk::translation::subSystemComponentType_t component;
   unsigned int policyId;
   nmprk::translation::policyType_t policyType;
   unsigned int policyLimit;
   // Software Forge Inc. --- Start ------------------------------------------
   // TODO: Need a policy trigger type variable
   // Software Forge Inc. --- End --------------------------------------------
   unsigned int policyTriggerLimit;
   bool         sendAlert;
   bool         shutdown;
   unsigned int correctionTime;
   unsigned int statReportingPeriod;
   bool             policyEnabled;
   bool             perSubSystemCompentEnabled;
   bool             globalPolicyControlEnabled;
  }policy_t;

  typedef struct {
   nmprk::translation::sampleType_t sampleType;
   unsigned int cur;                 // the current instant reading value   
   unsigned int min;                 // min value seen over last statReportingPeriod
   unsigned int max;                 // max value seen over last statReportingPeriod
   unsigned int avg;                 // the avg value for the last statReportingPeriod
   tm           timestamp;           // the time stamp from when the reading was took
   unsigned int statReportingPeriod; // the period over which the values are averaged
  }sample_t;
 
  typedef struct {
   // These settings on matter on NM Type systems, ignore them other wise
   unsigned int maxConSettings;       // The total number of connects this device supports
   unsigned int maxTriggerValue;      // max value the device supports for a reading
   unsigned int minTriggerValue;      // min value the device supports for a reading
   unsigned int minCorrectionTime;    //  the min value accepted for how long a system has 
                                      // to lower its power/thermal  below the limit set by a 
                                      // policy before the system takes corrective action
   unsigned int maxCorrectionTime;    // same as previous but the max value
   unsigned int minStatReportPeriod;  // the shortest time supported for averaging stats
   unsigned int maxStatReportPeriod;  // the longest time supported for averaging stats
   // these settings on matter on DCMI type systems, ignore them other wise
   bool supportsPowerManagement;
   bool manageabilityAccess;
   bool oobOverLan;
   bool serialMode;
   bool inBandInterfaceEnabled;
   int  temperatureSamplingFreq;
  }capabilities_t;

 };

};

#endif

