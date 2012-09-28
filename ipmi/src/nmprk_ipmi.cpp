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

bool nmprk::ipmi::connectDevice(device* d) 
{
  bool ret = false;
  if(d != NULL) 
  {
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
      if(found != std::string::npos) 
      { // do local
		    if(nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_connectToDevice(d)) 
        {
			    ret = true;
			    break;
		    }
        else if(nmprk::ipmi::modules::dnm::dnm_local_connectToDevice(d)) 
        {
			    ret = true;
			    break;
		    }
        else if(nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_connectToDevice(d)) 
        {
			    ret = true;
			    break;
		    }
	    }
      else
      {
		    if(nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_connectToDevice(d)) 
        {
			    ret = true;
			    break;
		    }
        else if(nmprk::ipmi::modules::dnm::dnm_local_connectToDevice(d)) 
        {
			    ret = true;
			    break;
		    }
        else if(nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_connectToDevice(d)) 
        {
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

/************************************************************************************************************
* Name: ipmi_rspCodes
*
* Description: Selects and throws an exception using one of the standard IPMI error codes.
*
************************************************************************************************************/

bool nmprk::ipmi::ipmi_rspCodes(int rspCode)
{
  switch(rspCode) {
  case NMPRK_NM_INVALID_POLICY_ID:
    throw new nmprk::nmprkException(rspCode,NMPRK_NM_INVALID_POLICY_ID_MSG);
    break;
  case NMPRK_NM_INVALID_DOMAIN:
    throw new nmprk::nmprkException(rspCode,NMPRK_NM_INVALID_DOMAIN_MSG);
    break;
  case NMPRK_NM_INVALID_POLICY_TRIG:
    throw new nmprk::nmprkException(rspCode,NMPRK_NM_INVALID_POLICY_TRIG_MSG);
    break;
  case NMPRK_NM_UNKNOWN_POLICY_TYPE:
    throw new nmprk::nmprkException(rspCode,NMPRK_NM_UNKNOWN_POLICY_TYPE_MSG);
    break;
  case NMPRK_NM_PWR_LIMIT_OUT_RANGE:
    throw new nmprk::nmprkException(rspCode,NMPRK_NM_PWR_LIMIT_OUT_RANGE_MSG);
    break;
  case NMPRK_NM_COR_TIME_OUT_RANGE:
    throw new nmprk::nmprkException(rspCode,NMPRK_NM_COR_TIME_OUT_RANGE_MSG);
    break;
  case NMPRK_NM_POL_TRIG_OUT_RANGE:
    throw new nmprk::nmprkException(rspCode,NMPRK_NM_POL_TRIG_OUT_RANGE_MSG);
    break;
  case NMPRK_NM_INVALID_MODE:
    throw new nmprk::nmprkException(rspCode,NMPRK_NM_INVALID_MODE_MSG);
    break;
  case NMPRK_NM_STAT_REPORT_OUT_RANGE:
    throw new nmprk::nmprkException(rspCode,NMPRK_NM_STAT_REPORT_OUT_RANGE_MSG);
    break;
  case NMPRK_NM_INVALID_AGGRESSIVE_BIT:
    throw new nmprk::nmprkException(rspCode,NMPRK_NM_INVALID_AGGRESSIVE_BIT_MSG);
    break;
  case IPMI_NODE_BUSY:
    throw new nmprk::nmprkException(rspCode,IPMI_NODE_BUSY_MSG );
    break;
  case IPMI_INVALID_COMMAND:
    throw new nmprk::nmprkException(rspCode,IPMI_INVALID_COMMAND_MSG);
    break;
  case IPMI_INVALID_LUN:
    throw new nmprk::nmprkException(rspCode,IPMI_INVALID_LUN_MSG);
    break;
  case IPMI_TIMEOUT_PROCESS:
    throw new nmprk::nmprkException(rspCode,IPMI_TIMEOUT_PROCESS_MSG);
    break;
  case IPMI_LACK_OF_STORAGE:
    throw new nmprk::nmprkException(rspCode,IPMI_LACK_OF_STORAGE_MSG);
    break;
  case IPMI_RESERVATION_CANCELED:
    throw new nmprk::nmprkException(rspCode,IPMI_RESERVATION_CANCELED_MSG);
    break;
  case IPMI_REQ_DATA_TRUNCATED:
    throw new nmprk::nmprkException(rspCode,IPMI_REQ_DATA_TRUNCATED_MSG);
    break;
  case IPMI_DATA_LENGTH_INVALID:
    throw new nmprk::nmprkException(rspCode,IPMI_DATA_LENGTH_INVALID_MSG);
    break;
  case IPMI_DATA_LENGTH_EXCEEDED:
    throw new nmprk::nmprkException(rspCode,IPMI_DATA_LENGTH_EXCEEDED_MSG);
    break;
  case IPMI_PARAMETER_OUT_OF_RANGE:
    throw new nmprk::nmprkException(rspCode,IPMI_PARAMETER_OUT_OF_RANGE_MSG);
    break;
  case IPMI_CANNOT_RETURN_DATA:
    throw new nmprk::nmprkException(rspCode,IPMI_CANNOT_RETURN_DATA_MSG);
    break;
  case IPMI_SENSOR_NOT_PRESENT:
    throw new nmprk::nmprkException(rspCode,IPMI_SENSOR_NOT_PRESENT_MSG);
    break;
  case IPMI_INVALID_DATA_FIELD:
    throw new nmprk::nmprkException(rspCode,IPMI_INVALID_DATA_FIELD_MSG);
    break;
  case IPMI_ILLEGAL_FOR_SENSOR:
    throw new nmprk::nmprkException(rspCode,IPMI_ILLEGAL_FOR_SENSOR_MSG);
    break;
  case IPMI_COULD_NOT_BE_PROVIDED:
    throw new nmprk::nmprkException(rspCode,IPMI_COULD_NOT_BE_PROVIDED_MSG);
    break;
  case IPMI_DUPLICATE_REQUEST:
    throw new nmprk::nmprkException(rspCode,IPMI_DUPLICATE_REQUEST_MSG);
    break;
  case IPMI_SDR_IN_UPDATE_MODE:
    throw new nmprk::nmprkException(rspCode,IPMI_SDR_IN_UPDATE_MODE_MSG);
    break;
  case IPMI_FW_IN_UPDATE_MODE:
    throw new nmprk::nmprkException(rspCode,IPMI_FW_IN_UPDATE_MODE_MSG);
    break;
  case IPMI_BMC_INITIALIZATION:
    throw new nmprk::nmprkException(rspCode,IPMI_BMC_INITIALIZATION_MSG);
    break;
  case IPMI_DESTINATION_UNAVAIL:
    throw new nmprk::nmprkException(rspCode,IPMI_DESTINATION_UNAVAIL_MSG);
    break;
  case IPMI_INSUFFICIENT_PRIV:
    throw new nmprk::nmprkException(rspCode,IPMI_INSUFFICIENT_PRIV_MSG);
    break;
  case IPMI_UNSUPPORT_PRES_STATE:
    throw new nmprk::nmprkException(rspCode,IPMI_UNSUPPORT_PRES_STATE_MSG);
    break;
  case IPMI_SUB_FUNC_DISABLED:
    throw new nmprk::nmprkException(rspCode,IPMI_SUB_FUNC_DISABLED_MSG);
    break; 
  default:
	  throw new nmprk::nmprkException(rspCode,NMPRK_CMD_FAILED_MSG);
  }
  return true;
}

