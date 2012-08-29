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
#include "../include/nmprk_ipmi_sel.h"
#include "../include/nmprk_ipmi.h"
#include "../include/nmprk_defines.h"
#include "../include/nmprk_helper.h"
#include "../include/nmprk_errCodes.h"
#include "../include/nmprk_exception.h"

#include <stdio.h>


// 0x0a 0x40 
nmprk::ipmi::repoInfo_t* nmprk::ipmi::sel::getSelInfo(nmprk::ipmi::device* d) {
 nmprk::ipmi::repoInfo_t* ret = NULL;
 if(d != NULL) {
  ret = new nmprk::ipmi::repoInfo_t();
  if(ret != NULL) {
   nmprk::ipmi::commandReq_t req;
   req.data.push_back(IPMI_CMD_STORAGE);
   req.data.push_back("0x40");          // Get SDr Repo Info
   nmprk::ipmi::commandRsp_t rsp;
   if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
    ret->repoVersion = nmprk::helper::hexStr2Int(rsp.data[0]);
    // following code prints out the response
    //for(int i = 0;i<rsp.data.size();i++)
    // fprintf(stderr,"%s ",rsp.data[i].c_str()); 
    std::string e(rsp.data[2]);
    e += rsp.data[1];
    ret->repoEntries = nmprk::helper::hexStr2Int(e);
    e = rsp.data[4];
    e += rsp.data[3];
    ret->repoFreeSpace = nmprk::helper::hexStr2Int(e);
    e = rsp.data[8];
    e += rsp.data[7];
    e += rsp.data[6];
    e += rsp.data[5];
    unsigned int un = nmprk::helper::hexStr2Int(e);
    time_t tt = (time_t)un;
    time_t* T = new time_t();
    *T = tt;
    struct tm* ti = NULL;
    ti = localtime(T);
    if(ti == NULL){ 
     DBG_MSG("ti == NULL\n");
     throw new nmprk::nmprkException(NMPRK_CNVRT_TS_FAILED_CODE,NMPRK_CNVRT_TS_FAILED_MSG);
    }
    ret->mostRecentAddTS  = *ti;
    e = rsp.data[12];
    e += rsp.data[11];
    e += rsp.data[10];
    e += rsp.data[9];
    un = nmprk::helper::hexStr2Int(e);
    tt = (time_t)un;
    *T = tt;
    ti = localtime(T);
    if(ti == NULL){
     DBG_MSG("ti == NULL\n");
     throw new nmprk::nmprkException(NMPRK_CNVRT_TS_FAILED_CODE,NMPRK_CNVRT_TS_FAILED_MSG);
    }
    ret->mostRecentDelTS = *ti;
    std::string bits = nmprk::helper::int2BinStr(nmprk::helper::hexStr2Int(rsp.data[13]));
    if(bits[0] == '1')
     ret->getAllocInfoSup = true;
    else
     ret->getAllocInfoSup = true;
    if(bits[1] == '1')
     ret->reserveSup = true;
    else
     ret->reserveSup = false;
    if(bits[2] == '1')
     ret->parAddSup = true;
    else
     ret->parAddSup = false;
    if(bits[3] == '1')
     ret->delSup = true;
    else
     ret->delSup = false;
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

// 0x0a 0x43 0x00 0x00 LSBID MSBID offSet BytesToRead
nmprk::ipmi::record_t*   nmprk::ipmi::sel::getSelRecord(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId) {
  nmprk::ipmi::record_t* ret = NULL;
 if(d != NULL && recordId != NULL) {
  ret = new nmprk::ipmi::record_t();
  if(ret != NULL) {
   nmprk::ipmi::commandReq_t req;
   req.data.push_back(IPMI_CMD_STORAGE);
   req.data.push_back("0x43");  // Get SDR command
   req.data.push_back("0x00");  // We don't support partial reads so
   req.data.push_back("0x00");  // reservation ID is set to 0x0000
   req.data.push_back(nmprk::helper::int2HexStr(recordId->lsb));
   req.data.push_back(nmprk::helper::int2HexStr(recordId->msb));
   req.data.push_back("0x00");  // we don't support partial reads so offset into record is always zero
   req.data.push_back("0xff");  // read the whole record
   nmprk::ipmi::commandRsp_t rsp;
   if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
    ret->nextRecord.lsb = nmprk::helper::hexStr2Int(rsp.data[0]);
    ret->nextRecord.msb = nmprk::helper::hexStr2Int(rsp.data[1]);
    int cnt = rsp.data.size();
     
    for(int i = 2;i<cnt;i++) {
     ret->data[(i-2)] = nmprk::helper::hexStr2Int(rsp.data[i]);
    }
	ret->len = rsp.data.size() - 2;
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

// 0x0a 0x44 n+1
bool                     nmprk::ipmi::sel::addSelRecord(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId,nmprk::ipmi::record_t* record) {
  bool ret = false;
 if(d != NULL && record != NULL && record->data != NULL && recordId != NULL) {
  nmprk::ipmi::commandReq_t req;
  req.data.push_back(IPMI_CMD_STORAGE);
  req.data.push_back("0x44");          // Add Sdr
  for(int i=0;i<record->len;i++)       // bytes n+1 = data
   req.data.push_back(nmprk::helper::int2HexStr(record->data[i]));
  // req data needs to be 16 bytes long, so we need to pad the rest
  for(int i=record->len;i<16;i++)
   req.data.push_back("0x00");
  nmprk::ipmi::commandRsp_t rsp;
  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
   recordId->lsb = nmprk::helper::hexStr2Int(rsp.data[0]);
   recordId->msb = nmprk::helper::hexStr2Int(rsp.data[1]);
   ret = true;
  }else{
    ret = false;
  }
 }else{
  throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 return ret;

}

// 0x0a 0x46 0x00 0x00 lsb msb
bool                     nmprk::ipmi::sel::delSelRecord(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId) {
  bool ret = false;
 if(d != NULL && recordId != NULL) {
  nmprk::ipmi::commandReq_t req;
  req.data.push_back(IPMI_CMD_STORAGE);
  req.data.push_back("0x46");   // delete SDR
  req.data.push_back("0x00");   // we don't support partial reads or deletes so
  req.data.push_back("0x00");   // reservation id is always set to 0x0000
  req.data.push_back(nmprk::helper::int2HexStr(recordId->lsb));
  req.data.push_back(nmprk::helper::int2HexStr(recordId->msb));
  nmprk::ipmi::commandRsp_t rsp;
  DBG_MSG("here?\n"); 
  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
   DBG_MSG("did we get here\n");
   ret = true;
  }
 }else{
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return ret;

}

// 0x0a 0x47 0x00 0x00 0x43 0x4c 0x52
bool                     nmprk::ipmi::sel::clearSel(nmprk::ipmi::device* d) {
 bool ret = false;
 if(d != NULL) {
  nmprk::ipmi::commandReq_t req;
  req.data.push_back(IPMI_CMD_STORAGE);
  req.data.push_back("0x47"); // Clear SDR
  req.data.push_back("0x00"); // no partialas,
  req.data.push_back("0x00"); // so reservation id is set to 0x000
  req.data.push_back("0x43"); // 'C'
  req.data.push_back("0x4c"); // 'L'
  req.data.push_back("0x52"); // 'R'
  req.data.push_back("0xaa"); // initiate erase
  nmprk::ipmi::commandRsp_t rsp;
  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) {
   ret = true;
  }
 }else{
  throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }

 return ret;

}

