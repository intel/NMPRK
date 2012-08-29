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
#include "../include/nmprk_ipmi_global.h"
#include "../include/nmprk_ipmi.h"
#include "../include/nmprk_helper.h"
#include "../include/nmprk_errCodes.h"
#include "../include/nmprk_exception.h"

// 0x06 0x01
nmprk::ipmi::getDeviceIdRsp* nmprk::ipmi::global::getDeviceId(nmprk::ipmi::device* d) {
 nmprk::ipmi::getDeviceIdRsp* ret = NULL;
 if(d != NULL) {
  ret = new nmprk::ipmi::getDeviceIdRsp();
  if(ret != NULL) {
   nmprk::ipmi::commandReq_t req;
   req.data.push_back(IPMI_CMD_APPLICATION);
   req.data.push_back("0x01");
   nmprk::ipmi::commandRsp_t rsp;
   if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
    if(rsp.rspCode != 0x00) {
     std::string msg = NMPRK_CMD_RETURNED_NON_ZERO_MSG;
     msg += "0x";
     msg += nmprk::helper::int2HexStr(rsp.rspCode);
     throw new nmprk::nmprkException(NMPRK_CMD_RETURNED_NON_ZERO_CODE,msg);
    }
    ret->deviceId = nmprk::helper::hexStr2Int(rsp.data[0]);
    std::string bits = nmprk::helper::int2BinStr(nmprk::helper::hexStr2Int(rsp.data[1]));
    if(bits[7] == '1')
     ret->deviceProvidesSdr = true;
    else
     ret->deviceProvidesSdr = false;
    bits[4] = '\0';
    ret->deviceRev = nmprk::helper::binStr2Int(bits);    
    bits = nmprk::helper::int2BinStr(nmprk::helper::hexStr2Int(rsp.data[2]));
    if(bits[7] == '0')
     ret->devNormOp = true;
    else
     ret->devNormOp = false;
    bits[7] = '\0';
    ret->firmwareRev =  nmprk::helper::hexStr2Int(bits);
    ret->firmwareRev2 = nmprk::helper::hexStr2Int(rsp.data[3]);
    bits = nmprk::helper::int2BinStr(nmprk::helper::hexStr2Int(rsp.data[5]));
    if(bits[0] == '1')
     ret->isSensorDev = true;
    else
     ret->isSensorDev = false;
    if(bits[1] == '1')
     ret->isSdrRepoDev = true;
    else
     ret->isSdrRepoDev = false;
    if(bits[3] == '1')
     ret->isFruInvDev = true;
    else
     ret->isFruInvDev = false;
    if(bits[4] == '1')
     ret->isIpmbRevDev = true;
    else
     ret->isIpmbRevDev = false;
    if(bits[5] == '1')
     ret->isIpmbGenDev = true;
    else
     ret->isIpmbGenDev = false;
    if(bits[6] == '1')
     ret->isBridgeDev = true;
    else
     ret->isBridgeDev = false;
    if(bits[7] == '1')
     ret->isChassisDev = true;
    else
     ret->isChassisDev = false;
    std::string e("0x");
    e += rsp.data[8];
    e += rsp.data[7];
    e += rsp.data[6];
    ret->manufId = e;
    e = "0x";
    e += rsp.data[10];
    e += rsp.data[9];
    ret->productId = e; 

   }else{
     delete ret;
     ret = NULL;
   }
  }else{
    throw new nmprk::nmprkException(NMPRK_FAILED_ALLOC_CODE,NMPRK_FAILED_ALLOC_MSG);
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return ret;
}

// 0x0a 0x07
nmprk::ipmi::acpiPwrState_t  nmprk::ipmi::global::getAcpiPwrState(nmprk::ipmi::device* d) {
 nmprk::ipmi::acpiPwrState_t ret;
 if(d != NULL) {
  nmprk::ipmi::commandReq_t req;
  req.data.push_back(IPMI_CMD_APPLICATION);
  req.data.push_back("0x07");
  nmprk::ipmi::commandRsp_t rsp;
  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
    if(rsp.rspCode != 0x00) {
     std::string msg = NMPRK_CMD_RETURNED_NON_ZERO_MSG;
     msg += "0x";
     msg += nmprk::helper::int2HexStr(rsp.rspCode);
     throw nmprk::nmprkException(NMPRK_CMD_RETURNED_NON_ZERO_CODE,msg);
    }
   // System Power State
   switch(nmprk::helper::hexStr2Int(rsp.data[0])) {
   case 0x00:  // S0/G0
    ret.systemState = stateSysS0G0;
    break;
   case 0x01:  // S1
    ret.systemState = stateSysS1;
    break;
   case 0x02:  // S2
    ret.systemState = stateSysS2;
    break;
   case 0x03:  // S3
    ret.systemState = stateSysS3;
    break;
   case 0x04:  // S4
    ret.systemState = stateSysS4;
    break;
   case 0x05:  // S5/G2
    ret.systemState = stateSysS5G2;
    break;
   case 0x06:  // S4/S5
    ret.systemState = stateSysS4S5;
    break;
   case 0x07:  // G3
    ret.systemState = stateSysG3;
    break;
   case 0x08:  // Sleeping
    ret.systemState = stateSysSleeping;
    break;
   case 0x09:  // G1 Sleeping
    ret.systemState = stateSysG1Sleep;
    break;
   case 0x0A:  // over ride
    ret.systemState = stateSysOverRide;
    break;
   case 0x20:  // Legacy On
    ret.systemState = stateSysSleeping;
    break;
   case 0x21:  // Legacy Off
    ret.systemState = stateSysSleeping;
    break;
   case 0x2A:  // Unknown
    ret.systemState = stateSysSleeping;
    break;
   };  
 
   // Device Power State
   switch(nmprk::helper::hexStr2Int(rsp.data[1])) {
   case 0x00: // D0
    ret.deviceState = stateDevD0;
    break;
   case 0x01: // D1
    ret.deviceState = stateDevD1;
    break;
   case 0x02: // D2
    ret.deviceState = stateDevD2;
    break;
   case 0x03: // D3
    ret.deviceState = stateDevD3;
    break;
   case 0x2A: // Unknown
    ret.deviceState = stateDevUnknown;
    break;
   };
  }else{
   throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG);
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return ret;
}

// 0x0a 0x06
bool                         nmprk::ipmi::global::setAcpiPwrState(nmprk::ipmi::device* d,nmprk::ipmi::acpiPwrState_t state) {
 bool ret = false;
 if(d != NULL) {
  nmprk::ipmi::commandReq_t req;
  req.data.push_back(IPMI_CMD_STORAGE);
  req.data.push_back("0x06");
  // on byte 1 we need to make sure to set bit[7] == 1 to set the power state 
  std::string bits = nmprk::helper::int2BinStr(state.systemState);
  bits[7] = '1';
  req.data.push_back(nmprk::helper::int2HexStr(nmprk::helper::binStr2Int(bits))); // byte 1, system power state to set
  req.data.push_back(nmprk::helper::int2HexStr(state.deviceState)); // byte 2, device power state to set
  nmprk::ipmi::commandRsp_t rsp;
  
  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
   ret = true;
  }else{
   throw new nmprk::nmprkException(NMPRK_CMD_FAILED_CODE,NMPRK_CMD_FAILED_MSG);
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 } 
 return ret;
}

