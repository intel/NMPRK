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

#include "modules/nm_dnm/nm_dnm.h"
#include "modules/dcmi/dcmi.h"

#ifdef WIN32
#include "../ipmi/include/nmprk_exception.h"
#include "../ipmi/include/nmprk_errCodes.h"
#include "../translation/include/nmprk_translation.h"
#include "../ipmi/include/nmprk_defines.h"
#include "../ipmi/include/nmprk_alert.h"
#else
#include <ipmi/include/nmprk_alert.h>
#include <ipmi/include/nmprk_exception.h>
#include <ipmi/include/nmprk_errCodes.h>
#include <translation/include/nmprk_translation.h>
#include <ipmi/include/nmprk_defines.h>
#endif

bool nmprk::translation::registerEventHandler(nmprk::ipmi::device* d, nmprk::ipmi::sensorFilter_t sensorF, nmprk::ipmi::eventFilter_t eventF, nmprk::ipmi::eventHandler_t handler) {
 throw new nmprk::nmprkException(NMPRK_NOT_IMPLEMNETED_CODE,NMPRK_NOT_IMPLEMNETED_MSG);
 return false;
}

bool nmprk::translation::unregisterEventHandler(nmprk::ipmi::device* d, nmprk::ipmi::sensorFilter_t sensorF, nmprk::ipmi::eventFilter_t eventF) {
  throw new nmprk::nmprkException(NMPRK_NOT_IMPLEMNETED_CODE,NMPRK_NOT_IMPLEMNETED_MSG);
  return false;
}

bool     nmprk::translation::swSubSystemSetup(nmprk::translation::initType_t init, nmprk::ipmi::device* d) {
 bool ret = false;

 if(init == 0x00 || init == 0x01) {
  // init or unint the library
  // currently no setup is required to start using the library
  // but this spot is held in reserve in case it needs to
  ret = true;
 }else{  // [un]init a device
   if(d != NULL) {
    
    switch(d->type) {
    case 0x01:  // dcmi
     ret = nmprk::translation::dcmi::dcmi_swSubSystemSetup(init,d);
     break;
    case 0x02:  // dnm
    case 0x03:  // NM
     ret = nmprk::translation::nm_dnm::nm_dnm_swSubSystemSetup(init,d);
     break;
    }; 

   }else{
     throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
   }
 }
 
 return ret;
}

nmprk::translation::nmVersion_t* nmprk::translation::getNMVersion(nmprk::ipmi::device* d) 
{
  nmprk::translation::nmVersion_t* ret = NULL;
 
  if(d != NULL) 
  {
    switch(d->type) {
    case 0x01:  // dcmi
    case 0x02:  // dnm
    case 0x03:  // NM
      ret = nmprk::translation::nm_dnm::nm_dnm_getNMVersion(d);
      break;
    };
  }else{
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
  }
  return ret;
}
   
nmprk::translation::capabilities_t* nmprk::translation::getCapabilities(nmprk::ipmi::device* d) 
{
  nmprk::translation::capabilities_t* ret = NULL;
 
  if(d != NULL) {

   switch(d->type) {
    case 0x01:  // dcmi
     ret = nmprk::translation::dcmi::dcmi_getCapabilities(d);
     break;
    case 0x02:  // dnm
    case 0x03:  // NM
     ret = nmprk::translation::nm_dnm::nm_dnm_getCapabilities(d);
     break;
    };

 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return ret;
}

bool    nmprk::translation::resetStatistics(nmprk::ipmi::device* d,nmprk::translation::subSystemComponentType_t domain,nmprk::translation::policy_t* policy) {
 bool ret = false;

 if(d != NULL) {

   switch(d->type) {
    case 0x01:  // dcmi
     ret = nmprk::translation::dcmi::dcmi_resetStatistics(d,domain,policy);
     break;
    case 0x02:  // dnm
    case 0x03:  // NM
     ret = nmprk::translation::nm_dnm::nm_dnm_resetStatistics(d,domain,policy);
     break;
    };

 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 
 return ret;
}

nmprk::translation::sample_t*     nmprk::translation::getSample(nmprk::ipmi::device* d,nmprk::translation::sampleType_t type, nmprk::translation::subSystemComponentType_t domain, nmprk::translation::policy_t* policy) {
 nmprk::translation::sample_t* ret = NULL;

 if(d != NULL) {
  
  switch(d->type) {
    case 0x01:  // dcmi
     ret = nmprk::translation::dcmi::dcmi_getSample(d,type,domain,policy);
     break;
    case 0x02:  // dnm
    case 0x03:  // NM
     ret = nmprk::translation::nm_dnm::nm_dnm_getSample(d,type,domain,policy);
     break;
    };

 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 
 return ret;
}

nmprk::translation::policy_t*    nmprk::translation::getPolicy(nmprk::ipmi::device* d,nmprk::translation::policy_t* policy) {
 nmprk::translation::policy_t* ret = NULL;
 
 if(d != NULL && policy != NULL) {
  
   switch(d->type) {
   case 0x01:  // dcmi
    ret = nmprk::translation::dcmi::dcmi_getPolicy(d,policy);
    break;
   case 0x02:  // dnm
   case 0x03:  // NM
    ret = nmprk::translation::nm_dnm::nm_dnm_getPolicy(d,policy);
    break;
   };

 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 
 return ret;
}

bool     nmprk::translation::setPolicy(nmprk::ipmi::device* d,nmprk::translation::policy_t* policy) {
 bool ret = false;

 if(d != NULL && policy != NULL) {
  
  switch(d->type) {
  case 0x01:  // dcmi
   ret = nmprk::translation::dcmi::dcmi_setPolicy(d,policy);
   break;
  case 0x02:  // dnm
  case 0x03:  // NM
   ret = nmprk::translation::nm_dnm::nm_dnm_setPolicy(d,policy);
   break;
  };
   
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 
 return ret;
}
 
bool     nmprk::translation::delPolicy(nmprk::ipmi::device* d,nmprk::translation::policy_t* policy) {
 bool ret = false;

 if(d != NULL && policy != NULL) {

  switch(d->type) {
  case 0x01:  // dcmi
   ret = nmprk::translation::dcmi::dcmi_delPolicy(d,policy);
   break;
  case 0x02:  // dnm
  case 0x03:  // NM
   ret = nmprk::translation::nm_dnm::nm_dnm_delPolicy(d,policy);
   break;
  };

 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return ret;
}

bool     nmprk::translation::setPolicyStatus(nmprk::ipmi::device* d,nmprk::translation::policy_t* policy,nmprk::translation::policyStatusType_t state) {
 bool ret = false;

 if(d != NULL && policy != NULL) {

  switch(d->type) {
  case 0x01:  // dcmi
   ret = nmprk::translation::dcmi::dcmi_setPolicyStatus(d,policy,state);
   break;
  case 0x02:  // dnm
  case 0x03:  // NM
   ret = nmprk::translation::nm_dnm::nm_dnm_setPolicyStatus(d,policy,state);
   break;
  }; 

 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return ret;
}

