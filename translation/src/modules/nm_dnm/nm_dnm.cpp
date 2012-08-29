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
#include "nm_dnm.h"

#include "../../../include/nmprk_translation_defines.h"
#include "../../../../ipmi/include/nmprk_ipmi.h"
#include "../../../../ipmi/include/nmprk_helper.h"
#include "../../../../ipmi/include/nmprk_exception.h"
#include "../../../../ipmi/include/nmprk_errCodes.h"

#include <string>
#include <iostream>
// Software Forge Inc. --- Start ------------------------------------------
// Added necessary headers
#include <iomanip>
#include <sstream>
// Software Forge Inc. --- End --------------------------------------------
#include <vector>

#define ADD_INTEL_ID(x) {                             \
                          ##x.data.push_back("0x57"); \
                          ##x.data.push_back("0x01"); \
                          ##x.data.push_back("0x00"); \
                         }

bool     nmprk::translation::nm_dnm::nm_dnm_swSubSystemSetup(nmprk::translation::initType_t type, nmprk::ipmi::device* d) { 
 bool ret = false;
 
 if(d != NULL) { 
  if(type == 0x02) { // init the device
   size_t found = d->address.find("local");
   // Software Forge Inc. --- Start ------------------------------------------
   // These cases were switched around so remote was where local should be 
   // and visa versa. We have fixed it.
   if(found != std::string::npos) { 
    // we need to make sure to enable kcs and that all the required drivers are loaded
     ret = nmprk::ipmi::initSystemForLocal();
     if(ret == true) 
	   ret = nmprk::ipmi::connectDevice(d);
   }else{
     // Remote system, no setup needed
     ret = nmprk::ipmi::connectDevice(d);
   }
   // Software Forge Inc. --- End --------------------------------------------
  }else if(type == 0x03) { // unInit the device, no steps needed
     // 
     ret = true;
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);  
 }

 return ret;
}

// 0x2e 0xc9 0x57 0x01 0x00 domainId policyType 
nmprk::translation::capabilities_t* nmprk::translation::nm_dnm::nm_dnm_getCapabilities(nmprk::ipmi::device* d) {
 nmprk::translation::capabilities_t* ret = NULL;
 
 if(d != NULL) {
  ret = new nmprk::translation::capabilities_t();
  if(ret != NULL) {
   nmprk::ipmi::commandReq_t req;
   req.data.push_back("0x2e");
   req.data.push_back("0xc9");
   req.data.push_back("0x57"); 
   req.data.push_back("0x01"); 
   req.data.push_back("0x00"); 
   req.data.push_back("0x00");
   req.data.push_back("0x10");
   nmprk::ipmi::commandRsp_t rsp;
   if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
    /*
	  unsigned int maxConSettings;       // The total number of connects this device supports
 	  unsigned int maxTriggerValue;      // max value the device supports for a reading
	  unsigned int minTriggerValue;      // min value the device supports for a reading
	  unsigned int minCorrectionTime;    //  the min value accepted for how long a system has
	  unsigned int maxCorrectionTime;    // same as previous but the max value
	  unsigned int minStatReportPeriod;  // the shortest time supported for averaging stats
	  unsigned int maxStatReportPeriod;  // the longest time supported for averaging stats
     */
     if(rsp.rspCode == 0) {
     	ret->maxConSettings = nmprk::helper::hexStr2Int(rsp.data[3]);
	std::string e("0x");
    	e += rsp.data[5];
     	e += rsp.data[4];
     	ret->maxTriggerValue = nmprk::helper::hexStr2Int(e);
     	e = "0x";
     	e += rsp.data[7];
     	e += rsp.data[6];
     	ret->minTriggerValue = nmprk::helper::hexStr2Int(e);
     	e = "0x";
     	e += rsp.data[11];
     	e += rsp.data[10];
     	e += rsp.data[9];
     	e += rsp.data[8];
     	ret->minCorrectionTime = nmprk::helper::hexStr2Int(e);
     	e = "0x";
     	e += rsp.data[15];
     	e += rsp.data[14];
     	e += rsp.data[13];
     	e += rsp.data[12];
     	ret->maxCorrectionTime = nmprk::helper::hexStr2Int(e);
     	e = "0x";
     	e += rsp.data[17];
     	e += rsp.data[16];
     	ret->minStatReportPeriod =  nmprk::helper::hexStr2Int(e);
     	e = "0x";
     	e += rsp.data[19];
     	e += rsp.data[18];
     	ret->maxStatReportPeriod = nmprk::helper::hexStr2Int(e);
     }else{
	switch(rsp.rspCode){
        case 0xC1:    // Invalid command, most likely bridge and transport where not set
 	 throw new nmprk::nmprkException(NMPRK_INVALID_CMD_CODE,NMPRK_INVALID_CMD_MSG);
	 break;
        case 0x81:   // Invalid Domain, mostly likely shouldn't be thrown but then again, you never know
         throw new nmprk::nmprkException(NMPRK_NM_INVALID_DOMAIN_CODE,NMPRK_NM_INVALID_DOMAIN_MSG);
         break;
        case 0x82:   // invalid policy trigger
	 throw new nmprk::nmprkException(NMPRK_NM_INVALID_POLICY_TRIG_CODE,NMPRK_NM_INVALID_POLICY_TRIG_MSG);
	 break;
        case 0x83:  // invalid policy type
         throw new nmprk::nmprkException(NMPRK_NM_INVALID_POLICY_TYPE_CODE,NMPRK_NM_INVALID_POLICY_TYPE_MSG);
	 break;
	default:
		// Software Forge Inc. --- Start ------------------------------------------
		// Improved the error message
		std::stringstream ss;
		ss << NMPRK_CMD_FAILED_MSG << ": 0x" << std::setfill ('0') << std::setw(2) << std::hex << (int)rsp.rspCode;
        throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE, ss.str());
		// Software Forge Inc. --- End --------------------------------------------
        }
     }
   }else{
     delete ret;
     ret = NULL;
     throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG);
   }
  }else{
    throw new nmprk::nmprkException(NMPRK_FAILED_ALLOC_CODE,NMPRK_FAILED_ALLOC_MSG);
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return ret;
}

// 0x2e 0xc7 0x57 0x01 0x00 mode domain policyId
bool    nmprk::translation::nm_dnm::nm_dnm_resetStatistics(nmprk::ipmi::device* d,nmprk::translation::subSystemComponentType_t domain, nmprk::translation::policy_t* policy) {
 bool ret = false;
 bool doPol = (policy != NULL) ? true : false;

 if(d != NULL) {
  nmprk::ipmi::commandReq_t req;
  req.data.push_back("0x2e");
  req.data.push_back("0xc7");
  req.data.push_back("0x57");
  req.data.push_back("0x01");
  req.data.push_back("0x00");
  if(doPol == true)
   req.data.push_back("0x01");   // reset per policy stats
  else
   req.data.push_back("0x00");   // reset global stats
  
  switch(domain) {
  case 0x00:  // global
   req.data.push_back("0x00");
   break;
  case 0x01:  // cpu domain
   req.data.push_back("0x01");
   break;
  case 0x02:  // mem domain
   req.data.push_back("0x02");
  }  

  if(doPol == true)
   req.data.push_back(nmprk::helper::int2HexStr(policy->policyId));
  else
   req.data.push_back("0x00");

  nmprk::ipmi::commandRsp_t rsp;
  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
    if(rsp.rspCode > 0 ) {
     switch(rsp.rspCode) {
     case 0x80:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_POLICY_ID_CODE,NMPRK_NM_INVALID_POLICY_ID_MSG);
        break;
     case 0x81:
	throw new nmprk::nmprkException(NMPRK_NM_INVALID_DOMAIN_CODE,NMPRK_NM_INVALID_DOMAIN_MSG);
	break;
     case 0x88:
	throw new nmprk::nmprkException(NMPRK_NM_INVALID_MODE_CODE,NMPRK_NM_INVALID_MODE_MSG);
        break;
     default:
	// Software Forge Inc. --- Start ------------------------------------------
	// Improved the error message
	std::stringstream ss;
	ss << NMPRK_CMD_FAILED_MSG << ": 0x" << std::setfill ('0') << std::setw(2) << std::hex << (int)rsp.rspCode;
    throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE, ss.str());
	// Software Forge Inc. --- End --------------------------------------------
     }
    }else   ret = true;
  }else{
    ret = false;
    throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG);
  }

 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 return ret;
}

//  0x2e 0xc8 0x57 0x01 0x00 mode domain policy
nmprk::translation::sample_t*     nmprk::translation::nm_dnm::nm_dnm_getSample(nmprk::ipmi::device* d,nmprk::translation::sampleType_t type, nmprk::translation::subSystemComponentType_t domain, nmprk::translation::policy_t* policy) {
 nmprk::translation::sample_t* sample;
 if(d != NULL) {
  sample = new nmprk::translation::sample_t();
  if(sample != NULL) {
   nmprk::ipmi::commandReq_t req;
   req.data.push_back("0x2e");
   req.data.push_back("0xc8");
   req.data.push_back("0x57");
   req.data.push_back("0x01");
   req.data.push_back("0x00");
   // we need to set the mode, either global power/ global thermal/per policy power/per policy thermal
   if(type == 0x00)  { // power reading
    if(policy == NULL)
     req.data.push_back("0x01"); // GLOBAL POWER READING
    else
     req.data.push_back("0x11"); // Per Policy power Reading
   }else{              // thermal reading
    if(policy == NULL)
     req.data.push_back("0x02"); // GLOBAL THERMAL READING
    else
     req.data.push_back("0x12"); // PER Policy Thermal reading
   }
   // set which domain to pull the stats from
   switch(domain) {
   case 0x00:
    req.data.push_back("0x00"); // for the whole domain
    break;
   case 0x01:
    req.data.push_back("0x01"); // for the cpu domain
    break;
   case 0x02:
    req.data.push_back("0x02"); // for the mem domain
    break;
   default:
    throw new nmprk::nmprkException(NMPRK_INVALID_DOMAIN_CODE,NMPRK_INVALID_DOMAIN_MSG); 
   };   
 
   if(policy == NULL)
    req.data.push_back("0x00");
   else
    req.data.push_back(nmprk::helper::int2HexStr(policy->policyId));

   nmprk::ipmi::commandRsp_t rsp;
   if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
    /*
           sampleType_t sampleType;
	   unsigned int cur;                 // the current instant reading value   
	   unsigned int min;                 // min value seen over last statReportingPeriod
	   unsigned int max;                 // max value seen over last statReportingPeriod
	   unsigned int avg;                 // the avg value for the last statReportingPeriod
	   tm           timestamp;           // the time stamp from when the reading was took
	   unsigned int statReportingPeriod; // the period over which the values are averaged
     */
     if(rsp.rspCode > 0 ) {
      switch(rsp.rspCode) {
      case 0x80:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_POLICY_ID_CODE,NMPRK_NM_INVALID_POLICY_ID_MSG);
        break;
      case 0x81:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_DOMAIN_CODE,NMPRK_NM_INVALID_DOMAIN_MSG);
        break;
      case 0x88:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_MODE_CODE,NMPRK_NM_INVALID_MODE_MSG);
        break;
      default:
        // Software Forge Inc. --- Start ------------------------------------------
		// Improved the error message
		std::stringstream ss;
		ss << NMPRK_CMD_FAILED_MSG << ": 0x" << std::setfill ('0') << std::setw(2) << std::hex << (int)rsp.rspCode;
		throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE, ss.str());
		// Software Forge Inc. --- End --------------------------------------------
      }
     }
     sample->sampleType = type;
     std::string e("0x");
     e += rsp.data[4];
     e += rsp.data[3];
     sample->cur = nmprk::helper::hexStr2Int(e);
     e = "0x";
     e += rsp.data[6];
     e += rsp.data[5];
     sample->min = nmprk::helper::hexStr2Int(e);
     e = "0x";
     e += rsp.data[8];
     e += rsp.data[7];
     sample->max = nmprk::helper::hexStr2Int(e);
     e = "0x";
     e += rsp.data[10];
     e += rsp.data[9];
     sample->avg = nmprk::helper::hexStr2Int(e);
     e = "0x";
     e += rsp.data[14];
     e += rsp.data[13];
     e += rsp.data[12];
     e += rsp.data[11];
     time_t ui = (time_t) nmprk::helper::hexStr2Int(e);
     struct tm* t = localtime((time_t*)&ui);
     if(t == NULL) {
	// if we can't generate a correct time stamp then
        // generate one based on the current time and put it in there
         time_t result = time(NULL);
         t = localtime(&result);
         std::cout << "Had to put our own time stamp\n";
         std::cout.flush();
     }else  sample->timestamp = *t;
     e = "0x";
     e += rsp.data[18];
     e += rsp.data[17];
     e += rsp.data[16];
     e += rsp.data[15];
     sample->statReportingPeriod = nmprk::helper::hexStr2Int(e);
   }else{
     delete sample;
     sample = NULL;
   }
  }else{
    throw new nmprk::nmprkException(NMPRK_FAILED_ALLOC_CODE,NMPRK_FAILED_ALLOC_MSG); 
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 } 
  
 return sample;
} 

//  0x2e 0xc2 0x57 0x01 0x00 domain policyId
nmprk::translation::policy_t*     nmprk::translation::nm_dnm::nm_dnm_getPolicy(nmprk::ipmi::device* d,nmprk::translation::policy_t* policy) {
 if(d != NULL && policy != NULL) {
  nmprk::ipmi::commandReq_t req;
  req.data.push_back("0x2e");
  req.data.push_back("0xc2");
  req.data.push_back("0x57");
  req.data.push_back("0x01");
  req.data.push_back("0x00");
  switch(policy->component) {
  case 0x00:  // global
   req.data.push_back("0x00");
   break;
  case 0x01: // per cpu domain
   req.data.push_back("0x01");
   break;
  case 0x02: // per mem domain
   req.data.push_back("0x02");
   break;
  default: 
   throw new nmprk::nmprkException(NMPRK_INVALID_DOMAIN_CODE,NMPRK_INVALID_DOMAIN_MSG);
  };
  
  req.data.push_back(nmprk::helper::int2HexStr(policy->policyId));

  nmprk::ipmi::commandRsp_t rsp;
  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
   if(rsp.rspCode > 0) {
    switch(rsp.rspCode) {
	  // Software Forge Inc. --- Start ------------------------------------------
      case 0x80:
      case 0x81:
		// TODO: These are valid error conditions, need to get this information 
		// back to user
        // break;
	  // Software Forge Inc. --- End --------------------------------------------
      default:
        // Software Forge Inc. --- Start ------------------------------------------
		// Improved the error message
		std::stringstream ss;
		ss << NMPRK_CMD_FAILED_MSG << ": 0x" << std::setfill ('0') << std::setw(2) << std::hex << (int)rsp.rspCode;
		throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE, ss.str());
		// Software Forge Inc. --- End --------------------------------------------
      }
   }   
   std::string bits = nmprk::helper::int2BinStr(nmprk::helper::hexStr2Int(rsp.data[3]));
   if(bits[4] == '1')
    policy->policyEnabled = true;
   else
    policy->policyEnabled = false;
   if(bits[5] == '1')
    policy->perSubSystemCompentEnabled = true;
   else
    policy->perSubSystemCompentEnabled = false;
   if(bits[6] == '1')
    policy->globalPolicyControlEnabled = true;
   else
    policy->globalPolicyControlEnabled = false;

   bits = nmprk::helper::int2BinStr(nmprk::helper::hexStr2Int(rsp.data[5]));
   if(bits[0] == '1')
    policy->sendAlert = true;
   else
    policy->sendAlert = false;
   if(bits[1] == '1')
    policy->shutdown = true;
   else
    policy->shutdown = false;
   
   std::string e("0x");
   e += rsp.data[7];
   e += rsp.data[6];
   policy->policyLimit = nmprk::helper::hexStr2Int(e);
  
   e = "0x";
   e += rsp.data[11];
   e += rsp.data[10];
   e += rsp.data[9];
   e += rsp.data[8];
   policy->correctionTime = nmprk::helper::hexStr2Int(e);

   e = "0x";
   e += rsp.data[15];
   e += rsp.data[14];
   policy->statReportingPeriod = nmprk::helper::hexStr2Int(e);
   
  }else{
    return NULL;
  }  
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 
 return policy;
}

// 0x2e 0xc1 0x57 0x01 0x00 domain[0:3] enabled[4] policyId policyType policyAction policyTargetLimit[8&9] correctionTime[10-13] policyTrigger[14&15] reportPeriod[16&17] 
bool     nmprk::translation::nm_dnm::nm_dnm_setPolicy(nmprk::ipmi::device* d,nmprk::translation::policy_t* policy) {
 bool ret = false;
 if(d != NULL && policy != NULL) {
#if 0
  // Software Forge Inc. --- Start ------------------------------------------
  // TODO: Not sure we really want this here, it breaks when user tries to
  // create new policy because the getPolicy command fails
  nmprk::translation::policy_t* p = nmprk::translation::nm_dnm::nm_dnm_getPolicy(d,policy);
  if(p->policyEnabled == true) {
   if(!nmprk::translation::nm_dnm::nm_dnm_setPolicyStatus(d,policy,nmprk::translation::policyDisabled)) {
    throw new nmprk::nmprkException(NMPRK_NM_UNABLE_DISABLE_POL_CODE,NMPRK_NM_UNABLE_DISABLE_POL_MSG);
    return false;
   }
  }
  // Software Forge Inc. --- End --------------------------------------------
#endif
  nmprk::ipmi::commandReq_t req;
  req.data.push_back("0x2e");
  req.data.push_back("0xc1");
  req.data.push_back("0x57");
  req.data.push_back("0x01");
  req.data.push_back("0x00");  
  int domain;
  switch(policy->component) {
  case 0x00:  // global
   domain = 0x00;
   break;
  case 0x01: // per cpu domain
   domain = 0x01;
   break;
  case 0x02: // per mem domain
   domain = 0x02;
   break;
  default:
   throw new nmprk::nmprkException(NMPRK_INVALID_DOMAIN_CODE,NMPRK_INVALID_DOMAIN_MSG);
  };
  // byte 4 bits [0:3] are the domain id, so first we take our domain id
  // and then convert it to a binary string because byte4[4] == 1 means to enable policy
  // and == 0 means disable, so we then set the bit in our string and then add it to the reqest data
  std::string bits = nmprk::helper::int2BinStr(domain);
  if(policy->policyEnabled == true)
   bits[3] = '1';
  else
   bits[3] = '0';
  req.data.push_back(std::string("0x") + nmprk::helper::int2HexStr(nmprk::helper::binStr2Int(bits)));
  req.data.push_back(std::string("0x") + nmprk::helper::int2HexStr(policy->policyId));
  // Software Forge Inc. --- Start ------------------------------------------
  // Not sure why the following line was here
  // throw int(1);
  // Software Forge Inc. --- End --------------------------------------------
  // once again byte 6 (policy type | trigger type) differnt bits mean stuff, so we do what we did last time
  int t;
  if(policy->policyType == 0x00) { // power policy
   t = 0x00;
  }else { // thermal policy
   t = 0x01;
  }
  bits = nmprk::helper::int2BinStr(t);
  bits[4] = '1';  // policy configuration action, 1 == add power policy
  bits[5] = '0';  // 5:6 are aggresive cpu power correction, we leave this to default
  bits[6] = '0';
  req.data.push_back(std::string("0x") + nmprk::helper::int2HexStr(nmprk::helper::binStr2Int(bits)));
  
  // byte 7 policy exception actions
  bits = nmprk::helper::int2BinStr(0x00);
  if(policy->sendAlert == true)
   bits[0] = '1';
  else
   bits[0] = '0';
  if(policy->shutdown == true)
   bits[1] = '1';
  else
   bits[1] = '0';
  req.data.push_back(std::string("0x") + nmprk::helper::int2HexStr(nmprk::helper::binStr2Int(bits)));
  
  // Software Forge Inc. --- Start ------------------------------------------
  // Replaced the code that was here with something more practical
  req.data.push_back(nmprk::helper::getByteStr(policy->policyLimit, 0)); // byte 8 first have of policy limit
  req.data.push_back(nmprk::helper::getByteStr(policy->policyLimit, 1)); // byte 9, second half
  req.data.push_back(nmprk::helper::getByteStr(policy->correctionTime, 0)); // byte 10
  req.data.push_back(nmprk::helper::getByteStr(policy->correctionTime, 1)); // byte 11
  req.data.push_back(nmprk::helper::getByteStr(policy->correctionTime, 2)); // byte 12
  req.data.push_back(nmprk::helper::getByteStr(policy->correctionTime, 3)); // byte 13
  req.data.push_back(nmprk::helper::getByteStr(policy->policyTriggerLimit, 0)); // byte 14 first have of policy trigger limit
  req.data.push_back(nmprk::helper::getByteStr(policy->policyTriggerLimit, 1)); // byte 15, second half
  req.data.push_back(nmprk::helper::getByteStr(policy->statReportingPeriod, 0));  // byte 16 first half of stat reporting period
  req.data.push_back(nmprk::helper::getByteStr(policy->statReportingPeriod, 1));  // byte 17, second half
  // Software Forge Inc. --- End --------------------------------------------
  
  nmprk::ipmi::commandRsp_t rsp;
  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
    if(rsp.rspCode > 0 ) {
     switch(rsp.rspCode) {
     case 0x80:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_POLICY_ID_CODE,NMPRK_NM_INVALID_POLICY_ID_MSG);
        break;
     case 0x81:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_DOMAIN_CODE,NMPRK_NM_INVALID_DOMAIN_MSG);
        break;
     case 0x82:
	throw new nmprk::nmprkException(NMPRK_NM_INVALID_POLICY_TRIG_CODE,NMPRK_NM_INVALID_POLICY_TRIG_MSG);
        break;
     case 0x83:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_POLICY_ACTION_CODE,NMPRK_NM_INVALID_POLICY_ACTION_MSG);
        break;
     case 0x84:
        throw new nmprk::nmprkException(NMPRK_NM_PWR_LIMIT_OUT_RANGE_CODE,NMPRK_NM_PWR_LIMIT_OUT_RANGE_MSG);
        break;
     case 0x85:
        throw new nmprk::nmprkException(NMPRK_NM_COR_TIME_OUT_RANGE_CODE,NMPRK_NM_COR_TIME_OUT_RANGE_MSG);
        break;
     case 0x86:
        throw new nmprk::nmprkException(NMPRK_NM_POL_TRIG_OUT_RANGE_CODE,NMPRK_NM_POL_TRIG_OUT_RANGE_MSG);
        break;
     case 0x89:
        throw new nmprk::nmprkException(NMPRK_NM_STAT_REPORT_OUT_RANGE_CODE,NMPRK_NM_STAT_REPORT_OUT_RANGE_MSG);
        break;
     case 0xD5:
        throw new nmprk::nmprkException(NMPRK_NM_POLICY_ID_ALREADY_EXIST_CODE,NMPRK_NM_POLICY_ID_ALREADY_EXIST_MSG);
        break;
     default:
        // Software Forge Inc. --- Start ------------------------------------------
		// Improved the error message
		std::stringstream ss;
		ss << NMPRK_CMD_FAILED_MSG << ": 0x" << std::setfill ('0') << std::setw(2) << std::hex << (int)rsp.rspCode;
		throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE, ss.str());
		// Software Forge Inc. --- End --------------------------------------------
     }
   }else ret = true;
  }else throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG);
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 return ret;
} 

bool     nmprk::translation::nm_dnm::nm_dnm_delPolicy(nmprk::ipmi::device* d,nmprk::translation::policy_t* policy) {
 bool ret = false;
 
 if(d != NULL && policy != NULL) {
  nmprk::translation::policy_t* p = nmprk::translation::nm_dnm::nm_dnm_getPolicy(d,policy);
  if(p->policyEnabled == true) {
   if(!nmprk::translation::nm_dnm::nm_dnm_setPolicyStatus(d,policy,nmprk::translation::policyDisabled)) {
    throw new nmprk::nmprkException(NMPRK_NM_UNABLE_DISABLE_POL_CODE,NMPRK_NM_UNABLE_DISABLE_POL_MSG);
    return false;
   }
  }
  nmprk::ipmi::commandReq_t req;
  req.data.push_back("0x2e");
  req.data.push_back("0xc1");
  req.data.push_back("0x57");
  req.data.push_back("0x01");
  req.data.push_back("0x00");
  int domain;
  switch(policy->component) {
  case 0x00:  // global
   domain = 0x00;
   break;
  case 0x01: // per cpu domain
   domain = 0x01;
   break;
  case 0x02: // per mem domain
   domain = 0x02;
   break;
  default:
   throw new nmprk::nmprkException(NMPRK_INVALID_DOMAIN_CODE,NMPRK_INVALID_DOMAIN_MSG);
  };
  // byte 4 bits [0:3] are the domain id, so first we take our domain id
  // and then convert it to a binary string because byte4[4] == 1 means to enable policy
  // and == 0 means disable, so we then set the bit in our string and then add it to the reqest data
  std::string bits = nmprk::helper::int2BinStr(domain);
  bits[4] = '0';
  req.data.push_back(std::string("0x") + nmprk::helper::int2HexStr(nmprk::helper::binStr2Int(bits)));
  req.data.push_back(std::string("0x") + nmprk::helper::int2HexStr(policy->policyId));
  // once again byte 6 (policy type | trigger type) differnt bits mean stuff, so we do what we did last time
  int t;
  if(policy->policyType == 0x00) { // power policy
   t = 0x00;
  }else { // thermal policy
   t = 0x01;
  }
  bits = nmprk::helper::int2BinStr(t);
  bits[4] = '0';  // policy configuration action, this is what deletes the policy
  bits[5] = '0';  // 5:6 are aggresive cpu power correction, we leave this to default
  bits[6] = '0';
  req.data.push_back(std::string("0x") + nmprk::helper::int2HexStr(nmprk::helper::binStr2Int(bits)));
  
   // byte 7 policy exception actions
  bits = nmprk::helper::int2BinStr(0x00);
  if(policy->sendAlert == true)
   bits[0] = '1';
  else
   bits[0] = '0';
  if(policy->shutdown == true)
   bits[1] = '1';
  else
   bits[1] = '0';
  req.data.push_back(std::string("0x") + nmprk::helper::int2HexStr(nmprk::helper::binStr2Int(bits)));

  std::string e = nmprk::helper::int2HexStr(policy->policyLimit);
  int el = e.length();
  std::string n;
  for(int j=(el-2);j<4;j++)
   n += "0";
  for(int j=2;j<el;j++)
   n += e[j];
  std::string b("0x");;
  b = n[2];
  b += n[3];
  req.data.push_back(b);  // byte 8 first have of policy limit
  b = "0x";
  b += n[0];
  b += n[1];
  req.data.push_back(b); // byte 9, second half

  e = nmprk::helper::int2HexStr(policy->correctionTime);
  el = e.length();
  n = "";
  for(int j=(el-2);j<8;j++)
   n += "0";
  for(int j=2;j<el;j++)
   n += e[j];
  b = "0x";
  b += n[6];
  b += n[7];
  req.data.push_back(b); // byte 10
  b = "0x";
  b += n[4];
  b += n[5];
  req.data.push_back(b); // byte 11
  b = "0x";
  b += n[2];
  b += n[3];
  req.data.push_back(b); // byte 12
  b = "0x";
  b += n[0];
  b += n[1];
  req.data.push_back(b); // byte 13

   // bytes 14:15 are the trigger policy limit, just like we set in bytes 8:9
  e = nmprk::helper::int2HexStr(policy->policyLimit);
  el = e.length();
  n = "";
  for(int j=(el-2);j<4;j++)
   n += "0";
  for(int j=2;j<el;j++)
   n += e[j];
  b  = "0x"; 
  b = n[2];
  b += n[3];
  req.data.push_back(b);  // byte 14 first have of policy limit
  b = "0x";
  b += n[0];
  b += n[1];
  req.data.push_back(b); // byte 15, second half

  // last two btes (16/17) are the stat reporting period
  e = nmprk::helper::int2HexStr(policy->statReportingPeriod);
  el = e.length();
  n = "";
  for(int j=(el-2);j<4;j++)
   n += "0";
  for(int j=2;j<el;j++)
   n += e[j];
  b = "0x";
  b = n[2];
  b += n[3];
  req.data.push_back(b);  // byte 16 first half of stat reporting period
  b = "0x";
  b += n[0];
  b += n[1];
  req.data.push_back(b); // byte 17, second half
  nmprk::ipmi::commandRsp_t rsp;
  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
   if(rsp.rspCode > 0 ) {
     switch(rsp.rspCode) {
     case 0x80:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_POLICY_ID_CODE,NMPRK_NM_INVALID_POLICY_ID_MSG);
        break;
     case 0x81:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_DOMAIN_CODE,NMPRK_NM_INVALID_DOMAIN_MSG);
        break;
     case 0x82:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_POLICY_TRIG_CODE,NMPRK_NM_INVALID_POLICY_TRIG_MSG);
        break;
     case 0x83:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_POLICY_ACTION_CODE,NMPRK_NM_INVALID_POLICY_ACTION_MSG);
        break;
     case 0x84:
        throw new nmprk::nmprkException(NMPRK_NM_PWR_LIMIT_OUT_RANGE_CODE,NMPRK_NM_PWR_LIMIT_OUT_RANGE_MSG);
        break;
     case 0x85:
        throw new nmprk::nmprkException(NMPRK_NM_COR_TIME_OUT_RANGE_CODE,NMPRK_NM_COR_TIME_OUT_RANGE_MSG);
        break;
     case 0x86:
        throw new nmprk::nmprkException(NMPRK_NM_POL_TRIG_OUT_RANGE_CODE,NMPRK_NM_POL_TRIG_OUT_RANGE_MSG);
        break;
     case 0x89:
        throw new nmprk::nmprkException(NMPRK_NM_STAT_REPORT_OUT_RANGE_CODE,NMPRK_NM_STAT_REPORT_OUT_RANGE_MSG);
        break;
     case 0xD5:
        throw new nmprk::nmprkException(NMPRK_NM_POLICY_ID_ALREADY_EXIST_CODE,NMPRK_NM_POLICY_ID_ALREADY_EXIST_MSG);
        break;
     default:
        // Software Forge Inc. --- Start ------------------------------------------
		// Improved the error message
		std::stringstream ss;
		ss << NMPRK_CMD_FAILED_MSG << ": 0x" << std::setfill ('0') << std::setw(2) << std::hex << (int)rsp.rspCode;
		throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE, ss.str());
		// Software Forge Inc. --- End --------------------------------------------
     }
   }else ret = true;
  }else ret = false;

 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 return ret;
}

// 0x2e 0xc0 0x57 0x01 0x00 flags domainId policyId
bool     nmprk::translation::nm_dnm::nm_dnm_setPolicyStatus(nmprk::ipmi::device* d,nmprk::translation::policy_t* policy,nmprk::translation::policyStatusType_t status) {
 bool ret = false;
 
 if(d != NULL && policy != NULL) {
  nmprk::ipmi::commandReq_t req;
  req.data.push_back("0x2e");
  req.data.push_back("0xc0");
  req.data.push_back("0x57");
  req.data.push_back("0x01");
  req.data.push_back("0x00");
  if(status == 0x00)  // enable policy
   req.data.push_back("0x05");
  else
   req.data.push_back("0x04");
  
  switch(policy->component) {  
  case 0x00:  // global
   req.data.push_back("0x00");
   break;
  case 0x01: // per cpu domain
   req.data.push_back("0x01");
   break;
  case 0x02: // per mem domain
   req.data.push_back("0x02");
   break;
  default:
   throw new nmprk::nmprkException(NMPRK_INVALID_DOMAIN_CODE,NMPRK_INVALID_DOMAIN_MSG);
  };

  req.data.push_back(std::string("0x") + nmprk::helper::int2HexStr(policy->policyId));
  nmprk::ipmi::commandRsp_t rsp;
  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
    if(rsp.rspCode > 0 ) {
     switch(rsp.rspCode) {
     case 0x80:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_POLICY_ID_CODE,NMPRK_NM_INVALID_POLICY_ID_MSG);
        break;
     case 0x81:
        throw new nmprk::nmprkException(NMPRK_NM_INVALID_DOMAIN_CODE,NMPRK_NM_INVALID_DOMAIN_MSG);
        break;
     default:
        // Software Forge Inc. --- Start ------------------------------------------
		// Improved the error message
		std::stringstream ss;
		ss << NMPRK_CMD_FAILED_MSG << ": 0x" << std::setfill ('0') << std::setw(2) << std::hex << (int)rsp.rspCode;
		throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE, ss.str());
		// Software Forge Inc. --- End --------------------------------------------
     }
   }else ret = true;

  }else{
    ret = false;
  }
 }else{
  throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return ret;
}

