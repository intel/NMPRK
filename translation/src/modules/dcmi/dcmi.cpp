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
 #include "dcmi.h"

#include "../../../include/nmprk_translation.h"
#include "../../../include/nmprk_translation_defines.h"
#include "../../../../ipmi/include/nmprk_ipmi.h"
#include "../../../../ipmi/include/nmprk_exception.h"
#include "../../../../ipmi/include/nmprk_errCodes.h"
#include "../../../../ipmi/include/nmprk_helper.h"

#include <string>
#include <time.h>

bool     nmprk::translation::dcmi::dcmi_swSubSystemSetup(nmprk::translation::initType_t type,nmprk::ipmi::device* d){  
 // Software Forge Inc. --- Start ------------------------------------------
 // This entire function seems wrong. Replaced with same implementation for NM version of this function
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
 // Software Forge Inc. --- End --------------------------------------------
}

                                                                                                                      
nmprk::translation::capabilities_t* nmprk::translation::dcmi::dcmi_getCapabilities(nmprk::ipmi::device* d) { 
 nmprk::translation::capabilities_t* ret = NULL;

 if(d != NULL) {
  ret = new nmprk::translation::capabilities_t();
  if(ret != NULL) {
   nmprk::ipmi::commandReq_t req;
   req.data.push_back("0x2c");  // DCGRP group extension for running DCMI commands
   req.data.push_back("0x01");  // Get DCMI Capabiltiies info
   req.data.push_back("0xdc");  // other half of the DCGRP extension
   req.data.push_back("0x01");  // paramater 1 (supported dcmi capabiltieis)

   nmprk::ipmi::commandRsp_t rsp;
   if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
    std::string bits = nmprk::helper::int2BinStr(nmprk::helper::hexStr2Int(rsp.data[5]));
    if(bits[0] == '1')
     ret->supportsPowerManagement = true;
    else
     ret->supportsPowerManagement = false;
    bits = nmprk::helper::int2BinStr(nmprk::helper::hexStr2Int(rsp.data[6]));
    if(bits[0] == '1')
     ret->inBandInterfaceEnabled = true;
    else
     ret->inBandInterfaceEnabled = false;
    if(bits[1] == '1')
     ret->serialMode = true;
    else
     ret->serialMode = false;
    if(bits[2] == '1')
     ret->oobOverLan = true;
    else
     ret->oobOverLan = false;
    req.data.clear();
    rsp.data.clear();
    req.data.push_back("0x2c");  // DCGRP group extension for running DCMI commands
    req.data.push_back("0x01");  // Get DCMI Capabiltiies info
    req.data.push_back("0xdc");  // other half of the DCGRP extension
    req.data.push_back("0x02");  // paramater 2 (mandatory platform attributes)

    if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
     ret->temperatureSamplingFreq = nmprk::helper::hexStr2Int(rsp.data[8]);
    }else{
     throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG); 
    }
   }else{
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

bool     nmprk::translation::dcmi::dcmi_resetStatistics(nmprk::ipmi::device* d,nmprk::translation::subSystemComponentType_t type,nmprk::translation::policy_t* policy) {
 // DCMI does not support a concept of statistics
 // So this is a auto throw of functionality not supported on device
 throw new nmprk::nmprkException(NMPRK_CMD_NOT_SUPPORT_CODE,NMPRK_CMD_NOT_SUPPORT_MSG);
 return false;
}

// domain and policy have no effect on these type of platforms
// as they don't support those types of concepts
nmprk::translation::sample_t*    nmprk::translation::dcmi::dcmi_getSample(nmprk::ipmi::device* d,nmprk::translation::sampleType_t type, nmprk::translation::subSystemComponentType_t domain,nmprk::translation::policy_t* policy) {
 nmprk::translation::sample_t* ret = NULL;
 
 if(d != NULL) {
  ret = new nmprk::translation::sample_t();
  if(ret != NULL) { 
   nmprk::ipmi::commandReq_t req;
   nmprk::ipmi::commandRsp_t rsp;
  
   ret->sampleType = type;

   if(type == samplePower) {
     req.data.push_back("0x2c");
     req.data.push_back("0x02");  // Get Power Reading
     req.data.push_back("0xdc");  // DCGRP group extension
     req.data.push_back("0x01");  // Mode == System Power Stats
     req.data.push_back("0x00");  // mode based attributes
     req.data.push_back("0x00");  // reserved

     if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {

      // byte 19 has the power reading state, if its set to no power measurement available we should throw a exception
      std::string bits = nmprk::helper::int2BinStr(nmprk::helper::hexStr2Int(rsp.data[17]));
      if(bits[6] == '0') {  // no power measurement is available
       throw new nmprk::nmprkException(NMPRK_DCMI_NO_MEASUREMENT_CODE,NMPRK_DCMI_NO_MEASUREMENT_MSG);
      }

      std::string e("0x");
      e += rsp.data[2];
      e += rsp.data[1];      
      ret->cur = nmprk::helper::hexStr2Int(e);
      
      e = "0x";
      e += rsp.data[4];
      e += rsp.data[3];
      ret->min = nmprk::helper::hexStr2Int(e);
      
      e = "0x";
      e += rsp.data[6];
      e += rsp.data[5];
      ret->max = nmprk::helper::hexStr2Int(e);
 
      e = "0x";
      e += rsp.data[8];
      e += rsp.data[7];
      ret->avg = nmprk::helper::hexStr2Int(e);

      e = "0x";
      e += rsp.data[12];
      e += rsp.data[11];
      e += rsp.data[10];
      e += rsp.data[9];
      unsigned int ui = nmprk::helper::hexStr2Int(e);
	  // Software Forge Inc. --- Start ------------------------------------------
	  // Fixed this, previous cast caused pointer to point outside its scope
	  // struct tm* t = localtime((time_t*)&ui);
	  time_t temp = ui;
      struct tm* t = localtime(&temp);
	  // Software Forge Inc. --- End --------------------------------------------
      ret->timestamp = *t;

      e = "0x";
      e += rsp.data[16];
      e += rsp.data[15];
      e += rsp.data[14];
      e += rsp.data[13];
      ret->statReportingPeriod = nmprk::helper::hexStr2Int(e);

     }else{
       throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG);
     }
   }else{  // thermal reading
     req.data.push_back("0x2c");
     req.data.push_back("0x10");  // Get Thermal Reading
     req.data.push_back("0xdc");  // DCGRP group extension
     req.data.push_back("0x01"); // Sensor Type (page 40 of dcmi spec) temp(01h);
     req.data.push_back("0x40"); // Entity Id
     req.data.push_back("0x00"); // Entity instance, we retirve information from all instances assoiced with this id
     req.data.push_back("0x01"); // return the first set of temp data
     if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
      std::string bits = nmprk::helper::int2BinStr(nmprk::helper::hexStr2Int(rsp.data[3]));
      bits[7] = '0'; // this is supposed to signify if value is positive or negative
                     // but will mess up our parsing and so we just asume all values 
                     // will be positive
      ret->cur = ret->min = ret->max = ret->avg = nmprk::helper::binStr2Int(bits);
      time_t tme = time(NULL);
      struct tm* TM = localtime(&tme);
      ret->timestamp = *TM;
      ret->statReportingPeriod = -1;
     }else{
       throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG);
     }
   }
  }else{
    throw new nmprk::nmprkException(NMPRK_FAILED_ALLOC_CODE,NMPRK_FAILED_ALLOC_MSG);
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return ret;
} 
// DCMI really doesn't have the concepts of policies
// we still use a policy class because most of the fields
// still relate, we just don't support having multple polcies
nmprk::translation::policy_t*  nmprk::translation::dcmi::dcmi_getPolicy(nmprk::ipmi::device* d,nmprk::translation::policy_t* policy) {             // Returns the specified policy
 
 if(d != NULL && policy != NULL) {
  
  if(policy->policyType == policyPower) {  // Get info for power limit

    nmprk::ipmi::commandReq_t req;
    req.data.push_back("0x2c");
    req.data.push_back("0x03");  // Get Power Limit
    req.data.push_back("0xdc");  // DCGRP extension
    req.data.push_back("0x00");  // Reservered for future use
    req.data.push_back("0x00");  // Reservered for future use
    nmprk::ipmi::commandRsp_t rsp;
  
    if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
      policy->sendAlert = false;
      policy->shutdown  = false;
    
      switch(nmprk::helper::hexStr2Int(rsp.data[3])) {
      case 0x01:  // shutdown 
       policy->shutdown = true;
       break;
      case 0x11:  // send alert
       policy->sendAlert = true;
       break;
      };
  
      std::string e("0x");
      e += rsp.data[5];
      e += rsp.data[4];
      policy->policyLimit = nmprk::helper::hexStr2Int(e);

      e = "0x";
      e += rsp.data[9];
      e += rsp.data[8];
      e += rsp.data[7]; 
      e += rsp.data[6];
      policy->correctionTime = nmprk::helper::hexStr2Int(e);

      e = "0x";
      e += rsp.data[13];
      e += rsp.data[12];
      policy->statReportingPeriod = nmprk::helper::hexStr2Int(e);
  
    }else{
      throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG);
    }
  
  }else{
   
    nmprk::ipmi::commandReq_t req;
    req.data.push_back("0x2c");
    req.data.push_back("0x0C");  // Get Thermal Limit
    req.data.push_back("0xdc");  // DCGRP extension
    req.data.push_back("0x40");  // Entity Id (either 0x37 or 0x40)
    req.data.push_back("0x01");  // Entity instance
    nmprk::ipmi::commandRsp_t rsp;
   
    if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
      policy->sendAlert = false;
      policy->shutdown  = false;
      
      std::string bits = nmprk::helper::int2BinStr(nmprk::helper::hexStr2Int(rsp.data[1]));
      if(bits[5] == '1') 
       policy->sendAlert = true;
      else
       policy->sendAlert = false;
     
      if(bits[6] == '1')
       policy->shutdown = true;
      else
       policy->shutdown = false;
 
      policy->policyLimit = nmprk::helper::hexStr2Int(rsp.data[2]);

      std::string e("0x");
      e += rsp.data[4];
      e += rsp.data[3];
      policy->correctionTime = nmprk::helper::hexStr2Int(e);      

    }else{
      throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG);
    }
   
  }

 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return policy;
}

bool     nmprk::translation::dcmi::dcmi_setPolicy(nmprk::ipmi::device* d, nmprk::translation::policy_t* policy) {
 bool ret=false;

 if(d != NULL && policy != NULL) {
  nmprk::ipmi::commandReq_t req;
  nmprk::ipmi::commandRsp_t rsp;
  if(policy->policyType == policyPower) {
   req.data.push_back("0x2c");
   req.data.push_back("0x04");  // Set Power Limit
   req.data.push_back("0xdc");  // DCGRP group extension
   req.data.push_back("0x00");  // reserved
   req.data.push_back("0x00");  // reserved
   req.data.push_back("0x00");  // reserved
   // since dcmi only allows us to specify 1 action to take
   // we start off by setting alert then shutdown 
   // and if thats false we check just send alert
   if(policy->shutdown == true) 
    req.data.push_back("0x01");
   else if(policy->sendAlert == true)
    req.data.push_back("0x11");
   else // perform no action
    req.data.push_back("0x00");

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
   req.data.push_back(b);  // byte 6 first have of policy limit
   b = "0x";
   b += n[0];
   b += n[1];
   req.data.push_back(b); // byte 7, second half

   // Software Forge Inc. --- Start ------------------------------------------
   // Replaced what was here with a better approach
   req.data.push_back(nmprk::helper::getByteStr(policy->correctionTime, 0)); // byte 10
   req.data.push_back(nmprk::helper::getByteStr(policy->correctionTime, 1)); // byte 11
   req.data.push_back(nmprk::helper::getByteStr(policy->correctionTime, 2)); // byte 12
   req.data.push_back(nmprk::helper::getByteStr(policy->correctionTime, 3)); // byte 13
   // Software Forge Inc. --- End --------------------------------------------

   req.data.push_back("0x00"); // reserved for future use
   req.data.push_back("0x00"); // reserved for future use

   // Software Forge Inc. --- Start ------------------------------------------
   // Replaced what was here with a better approach
   req.data.push_back(nmprk::helper::getByteStr(policy->statReportingPeriod, 0));  // byte 16 first half of stat reporting period
   req.data.push_back(nmprk::helper::getByteStr(policy->statReportingPeriod, 1));  // byte 17, second half
   // Software Forge Inc. --- End --------------------------------------------

   if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
	// Software Forge Inc. --- Start ------------------------------------------
	// TODO: Need to check completion code
    // Software Forge Inc. --- End --------------------------------------------
    ret = true;
   }else{
     throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG);
   }
  }else{  // set thermal policy
    
   req.data.push_back("0x2c");
   req.data.push_back("0x0b");  // Set thermal Limit
   req.data.push_back("0xdc");  // DCGRP group extension
   req.data.push_back("0x40");  // entity id, either 0x37 or 0x40
   req.data.push_back("0x01");  // reserved
   
   // unlike in the set power limit we can have multple actions for thermal limits
   // so we need to make a string of bits and then convert that to the actual byte
   std::string bits = nmprk::helper::int2BinStr(0x00);
   if(policy->shutdown)
    bits[6] = '1';
   if(policy->sendAlert)
    bits[5] = '1';
   req.data.push_back(nmprk::helper::int2HexStr(nmprk::helper::binStr2Int(bits)));

   std::string e = nmprk::helper::int2HexStr(policy->correctionTime);
   int el = e.length();
   std::string n = "";
   for(int j=(el-2);j<4;j++)
    n += "0";
   for(int j=2;j<el;j++)
    n += e[j];
   std::string b("0x");
   b += n[2];
   b += n[3];
   req.data.push_back(b); // byte 6, first half of correction time
   b = "0x";
   b += n[0];
   b += n[1];
   req.data.push_back(b); // byte 7, second half

   if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
	// Software Forge Inc. --- Start ------------------------------------------
	// TODO: Need to check completion code
    // Software Forge Inc. --- End --------------------------------------------
    ret = true;
   }else{
     throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG);
   }

  }

 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return ret;
} 

// DCMI doesn't support "deleting" a policy as well as support for multiple policies
// so what we do here is call dcmi_setPolicyStatus with a policyStatusType_t of policyDisabled
// if its a thermal policy we can't even do that so we set it will a max limit (128)
bool     nmprk::translation::dcmi::dcmi_delPolicy(nmprk::ipmi::device* d,nmprk::translation::policy_t* policy) {             // Deletes a policy
 bool ret = false;

 if(d != NULL && policy != NULL) {
  if(policy->policyType == policyPower)
   ret = nmprk::translation::dcmi::dcmi_setPolicyStatus(d,policy,policyDisabled);
  else { // its a thermal policy so we need to do some trickery
   policy->policyLimit = 128;  // max supported value per DCMI spec
   policy->shutdown = false;
   policy->sendAlert = false;
   ret = nmprk::translation::dcmi::dcmi_setPolicy(d,policy);
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return ret;
}

// DCMI doesn't support disabling thermal polcies so instead we 
// call set thermal limit with the max value of 128
// and thermal policies are auto enabled by the set thermal policy command so we 
// call set thermal to enable a policy
bool    nmprk::translation::dcmi::dcmi_setPolicyStatus(nmprk::ipmi::device* d,nmprk::translation::policy_t* policy,nmprk::translation::policyStatusType_t status) {          // Enables / disables a policy
 bool ret = false;

 if(d != NULL && policy != NULL) {
  if(policy->policyType != policyPower) { // set status of thermal policy

   if(status == policyEnabled) {
    ret = nmprk::translation::dcmi::dcmi_setPolicy(d,policy);
   }else{
      // hack since there is no techinal way to disable thermal limits
      policy->policyLimit = 128;  // max supported value per DCMI spec
      policy->shutdown = false;
      policy->sendAlert = false;
      ret = nmprk::translation::dcmi::dcmi_setPolicy(d,policy);
   }

  }else{
    nmprk::ipmi::commandReq_t req;
    req.data.push_back("0x2c");
    req.data.push_back("0x05"); // activate/deactivate power limit
    req.data.push_back("0xdc"); // DCGRP extension
    if(status == policyEnabled)
     req.data.push_back("0x01");
    else
     req.data.push_back("0x00");

    req.data.push_back("0x00"); //reserved
    req.data.push_back("0x00"); // reserved
    nmprk::ipmi::commandRsp_t rsp;
    
    if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
     ret = true;
    }else{
      throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG);
    }
    
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
  
 return ret;
}

