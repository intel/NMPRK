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
#include "../include/nmprk_ipmi_fru.h"
#include "../include/nmprk_helper.h"
#include "../include/nmprk_exception.h"
#include "../include/nmprk_ipmi.h"
#include "../include/nmprk_errCodes.h"
#include "../include/nmprk_defines.h"

#include <string>
#include <vector>

// raw 0x0a 0x10 0x00
nmprk::ipmi::fruInfo_t*  nmprk::ipmi::fru::getFruInfo(nmprk::ipmi::device* d) {
 nmprk::ipmi::fruInfo_t* ret = NULL;
 if(d != NULL) {
   ret = new nmprk::ipmi::fruInfo_t();
  if(ret != NULL) {
   nmprk::ipmi::commandReq_t req;
   req.data.push_back(IPMI_CMD_STORAGE); // Storage netFn
   req.data.push_back("0x10");           // Get Fru Inventory Area Info
   req.data.push_back("0x00");           // Fru Device ID

   nmprk::ipmi::commandRsp_t rsp;
   if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
    ret->fruSize[0] = nmprk::helper::hexStr2Int(rsp.data[0]);
    ret->fruSize[1] = nmprk::helper::hexStr2Int(rsp.data[1]);
    int i = nmprk::helper::hexStr2Int(rsp.data[2]);  
    if(i == 0) // if bit[0] == 0 then access by byte
     ret->accessByWord = false;
    else
     ret->accessByWord = true; 
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

// raw 0x0a 0x11 0x00 LS_offSet MS_offSet count
nmprk::ipmi::record_t*   nmprk::ipmi::fru::getFruData(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId) {
 nmprk::ipmi::record_t* ret = NULL;
 if(d != NULL && recordId != NULL) {
  ret = new nmprk::ipmi::record_t();
  if(ret != NULL) {
   nmprk::ipmi::commandReq_t req;
   req.data.push_back(IPMI_CMD_STORAGE);
   req.data.push_back("0x11");   // Read Fru Data
   req.data.push_back("0x00");   // device id
   req.data.push_back(nmprk::helper::int2HexStr(recordId->lsb));
   req.data.push_back(nmprk::helper::int2HexStr(recordId->msb));
   req.data.push_back("0x01");  // return back 1 entry
   nmprk::ipmi::commandRsp_t rsp;
   if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
      for(int i = 0;i < rsp.data.size();i++)
       ret->data[i] = nmprk::helper::hexStr2Int(rsp.data[i]);
      ret->nextRecord.lsb = recordId->lsb;
      ret->nextRecord.msb = recordId->msb;
	  ret->len = rsp.data.size();; 
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

// raw 0x0a 0x12 0x00 lsb msb data+
bool                     nmprk::ipmi::fru::setFruData(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId,nmprk::ipmi::record_t* record) {
 bool ret = false;
 if(d != NULL && recordId != NULL && record != NULL) {
  nmprk::ipmi::commandReq_t req;
  req.data.push_back(IPMI_CMD_STORAGE);
  req.data.push_back("0x12");   // Write Fru Data
  req.data.push_back("0x00");   // Device Id
  req.data.push_back(nmprk::helper::int2HexStr(recordId->lsb));
  req.data.push_back(nmprk::helper::int2HexStr(recordId->lsb));
  for(int i = 0;i < record->len; i++)
   req.data.push_back(nmprk::helper::int2HexStr(record->data[i]));

  
  nmprk::ipmi::commandRsp_t rsp;
  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
    ret = true;
  }else{
   ret = false;
  }
 }else{
  throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 return ret;
}

