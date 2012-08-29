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
#include <nmprk_ipmi.h>
#include <nmprk_defines.h>
#include "modules/nm_dcmi/nm_dcmi.h"
#include "modules/dnm/dnm.h"
#include <nmprk_exception.h>
#include <nmprk_errCodes.h>
#include <stdio.h>
#include <string>

bool nmprk::ipmi::connectDevice(device* d) {
 bool ret = false;
 if(d != NULL) {
  size_t found;
  switch(d->type) {
    case 1: // dcmi
      found = d->address.find("local");
      if(found != std::string::npos) // do local
       ret =  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_connectToDevice(d);
      else
       ret =  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_connectToDevice(d);
      break;
     case 2: // DNM
      found = d->address.find("local");
      if(found != std::string::npos) // do local
       ret =  nmprk::ipmi::modules::dnm::dnm_local_connectToDevice(d);
      else
       ret =  nmprk::ipmi::modules::dnm::dnm_remote_connectToDevice(d);
      break;
     case 3: // NM
      found = d->address.find("local");
      if(found != std::string::npos) // do local
       ret =  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_connectToDevice(d);
      else
       ret =  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_connectToDevice(d);
      break;
	case 0: // auto detect
	  found = d->address.find("local");
      if(found != std::string::npos) { // do local
		if(nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_connectToDevice(d)) {
			ret = true;
			break;
		}else if(nmprk::ipmi::modules::dnm::dnm_local_connectToDevice(d)) {
			ret = true;
			break;
		}else if(nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_connectToDevice(d)) {
			ret = true;
			break;
		}
	  }else{
		  if(nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_connectToDevice(d)) {
			ret = true;
			break;
		}else if(nmprk::ipmi::modules::dnm::dnm_local_connectToDevice(d)) {
			ret = true;
			break;
		}else if(nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_connectToDevice(d)) {
			ret = true;
			break;
		}
	  }
	  break;
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 return ret;
}
  
bool nmprk::ipmi::disconnectDevice(device* d) {
 bool ret = false;
 if(d != NULL) {
  size_t found;
  switch(d->type) {
  //case 0: // auto detect
  // break;
    case 1: // dcmi
      found = d->address.find("local");
      if(found != std::string::npos) // do local
       ret =  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_disconnectDevice(d);
      else
       ret =  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_disconnectDevice(d);
      break;
     case 2: // DNM
      found = d->address.find("local");
      if(found != std::string::npos) // do local
	ret =  nmprk::ipmi::modules::dnm::dnm_local_disconnectDevice(d);
      else
	ret =  nmprk::ipmi::modules::dnm::dnm_remote_disconnectDevice(d);
      break;
     case 3: // NM
      found = d->address.find("local");
      if(found != std::string::npos) // do local
       ret =  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_disconnectDevice(d);
      else
       ret =  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_disconnectDevice(d);
      break;
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 return ret;
}

bool nmprk::ipmi::runIpmiCommand(device* d, commandReq_t* req,commandRsp_t* rsp) {
 bool ret = false; 
 if(d != NULL) {
  size_t found;
  switch(d->type) {
  //case 0: // auto detect
  // break;
    case 1: // dcmi
      found = d->address.find("local");
      if(found != std::string::npos) // do local    
       ret =  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_runIpmiCommand(d,req,rsp);
      else
       ret =  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_runIpmiCommand(d,req,rsp); 
      break;
     case 2: // DNM
      found = d->address.find("local");
      if(found != std::string::npos) // do local
       ret =  nmprk::ipmi::modules::dnm::dnm_local_runIpmiCommand(d,req,rsp);
      else
       ret =  nmprk::ipmi::modules::dnm::dnm_remote_runIpmiCommand(d,req,rsp);
      break;
     case 3: // NM
      found = d->address.find("local");
      if(found != std::string::npos) // do local
       ret =  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_runIpmiCommand(d,req,rsp);
      else
       ret =  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_runIpmiCommand(d,req,rsp);
      break;
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 } 
 return ret;
}

bool nmprk::ipmi::initSystemForLocal() {
 // Software Forge Inc. --- Start ------------------------------------------
 // TODO: Implement this!!!
 bool ret = true;
 // Software Forge Inc. --- End --------------------------------------------
 return ret;
}

