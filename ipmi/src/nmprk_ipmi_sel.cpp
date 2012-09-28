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
nmprk::ipmi::repoInfo_t* nmprk::ipmi::sel::getSelInfo(nmprk::ipmi::device* d) 
{
  nmprk::ipmi::repoInfo_t* ret = NULL;
  if(d != NULL) 
  {
    ret = new nmprk::ipmi::repoInfo_t();
    if(ret != NULL) 
	  {
      nmprk::ipmi::commandReq_t req;
      req.data.push_back(IPMI_CMD_STORAGE);
      req.data.push_back("0x40");          // Get SEL Repo Info
      nmprk::ipmi::commandRsp_t rsp;
      if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp) == true) 
	    {
        if(rsp.rspCode > 0)
        {
          ipmi_rspCodes(rsp.rspCode);
        }
        else
        {
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
          if(ti == NULL)
	  	    { 
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
          ret->getAllocInfoSup = (bits[0] == '1') ? true : false;
          ret->reserveSup = (bits[1] == '1') ? true : false;
          ret->parAddSup = (bits[2] == '1') ? true : false;
          ret->delSup = (bits[3] == '1') ? true : false;
        }
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
nmprk::ipmi::record_t*   nmprk::ipmi::sel::getSelRecord(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId) 
{
  nmprk::ipmi::record_t* ret = NULL;
  if(d != NULL && recordId != NULL) 
  {
    ret = new nmprk::ipmi::record_t();
    if(ret != NULL) 
    {
      nmprk::ipmi::commandReq_t req;
      req.data.push_back(IPMI_CMD_STORAGE);
      req.data.push_back("0x43");  // Get SEL command
      req.data.push_back("0x00");  // We don't support partial reads so
      req.data.push_back("0x00");  // reservation ID is set to 0x0000
      req.data.push_back(nmprk::helper::int2HexStr(recordId->lsb));
      req.data.push_back(nmprk::helper::int2HexStr(recordId->msb));
      req.data.push_back("0x00");  // we don't support partial reads so offset into record is always zero
      req.data.push_back("0xff");  // read the whole record
      nmprk::ipmi::commandRsp_t rsp;
      if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp) == true) 
      {
        if(rsp.rspCode > 0)
        {
          ipmi_rspCodes(rsp.rspCode);
        }
        else
        {
          ret->nextRecord.lsb = nmprk::helper::hexStr2Int(rsp.data[0]);
          ret->nextRecord.msb = nmprk::helper::hexStr2Int(rsp.data[1]);
          int cnt = rsp.data.size();
     
          for(int i = 2;i<cnt;i++) {
            ret->data[(i-2)] = nmprk::helper::hexStr2Int(rsp.data[i]);
          }
	        ret->len = rsp.data.size() - 2;
        }
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

/*
// Add Sel is a useless function in that you can not really add SEL records to the SEL this way.  The SEL will modify
// the record that is being added, modifing the time of the event. Therefore this command is being commented out for now
// until someone can define a use for it.  There is another command for adding new SEL events, which appends those 
// those events to the current SEL.
// 0x0a 0x44 n+1
bool nmprk::ipmi::sel::addSelRecord(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId,nmprk::ipmi::record_t* record) 
{
  bool ret = false;
  if(d != NULL && record != NULL && record->data != NULL && recordId != NULL) 
  {
    nmprk::ipmi::commandReq_t req;
    req.data.push_back(IPMI_CMD_STORAGE);
    req.data.push_back("0x44");                       // Add SEL
    for(unsigned int i=0; i < record->len; i++)       // bytes n+1 = data
      req.data.push_back(nmprk::helper::int2HexStr(record->data[i]));
    // req data needs to be 16 bytes long, so we need to pad the rest
    for(int i=record->len;i<16;i++)
      req.data.push_back("0x00");
    nmprk::ipmi::commandRsp_t rsp;
    if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) 
    {
      recordId->lsb = nmprk::helper::hexStr2Int(rsp.data[0]);
      recordId->msb = nmprk::helper::hexStr2Int(rsp.data[1]);
      ret = true;
    }else{
      // For some reason it returns a false, but the record is being entered.  So for
      // testing purposes change the false to true because it appears to be successful.
      ret = true; //false;
    }
  }else{
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
  }
  return ret;
}
*/

// 0x0a 0x46 0x00 0x00 lsb msb
bool nmprk::ipmi::sel::delSelRecord(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId) 
{
  bool ret = false;
  if(d != NULL && recordId != NULL) 
  {
    nmprk::ipmi::commandReq_t req;
    req.data.push_back(IPMI_CMD_STORAGE);
    req.data.push_back("0x46");   // Delete SEL record
    req.data.push_back("0x00");   // we don't support partial reads or deletes so
    req.data.push_back("0x00");   // reservation id is always set to 0x0000
    req.data.push_back("0xff");   // You have a choice between first 0x0000
    req.data.push_back("0xff");   // and last record 0xffff
    nmprk::ipmi::commandRsp_t rsp;
    if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp) == true) 
    {
		  if(rsp.rspCode == 0xC5) // then need to reserve
      {
    	  // Reserve SEL command
	  	  req.data.clear();
		    req.data.push_back(IPMI_CMD_STORAGE);
        req.data.push_back("0x42");  // Reserve SEL Repository command
	      rsp.data.clear();
    	  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) 
        {
			    if(rsp.rspCode != 0x00) // there was an error
          {
            ipmi_rspCodes(rsp.rspCode);
			    }
          else
          { // command was successful
            req.data.clear();
            req.data.push_back(IPMI_CMD_STORAGE);
            req.data.push_back("0x46"); // Delete SEL record
            req.data.push_back(rsp.data[0]); 
            req.data.push_back(rsp.data[1]);
            req.data.push_back(nmprk::helper::int2HexStr(recordId->lsb));
            req.data.push_back(nmprk::helper::int2HexStr(recordId->msb));
            rsp.data.clear();
            if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp) == true) 
            {
              if(rsp.rspCode == 0x00) // it was successful
                ret = true;
              else // It failed for some reason, throw an exception.
                ipmi_rspCodes(rsp.rspCode);
            }
            else // It failed for some reason, throw an exception.
            {
        		  std::string msg = NMPRK_CMD_RETURNED_NON_ZERO_MSG;
              msg += "0x";
        		  msg += nmprk::helper::int2HexStr(rsp.rspCode);
              throw new nmprk::nmprkException(NMPRK_CMD_RETURNED_NON_ZERO_CODE,msg);
            }
          }
        }
        else // It failed for some reason, throw an exception.
        {
          std::string msg = NMPRK_CMD_RETURNED_NON_ZERO_MSG;
          msg += "0x";
          msg += nmprk::helper::int2HexStr(rsp.rspCode);
          throw new nmprk::nmprkException(NMPRK_CMD_RETURNED_NON_ZERO_CODE,msg);
        }
      }
      else
      {
        if(rsp.rspCode == 0x00) // it was successful
          ret = true;
        else // It failed for some reason, throw an exception.
          ipmi_rspCodes(rsp.rspCode);
      }
    }
    else // returned false
    {
			std::string msg = NMPRK_CMD_RETURNED_NON_ZERO_MSG;
      msg += "0x";
			msg += nmprk::helper::int2HexStr(rsp.rspCode);
      throw new nmprk::nmprkException(NMPRK_CMD_RETURNED_NON_ZERO_CODE,msg);
    }
  }else{
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
  }
  return ret;
}

// 0x0a 0x47 0x00 0x00 0x43 0x4c 0x52
bool nmprk::ipmi::sel::clearSel(nmprk::ipmi::device* d) 
{
  bool ret = false;
  if(d != NULL) 
  {
    nmprk::ipmi::commandReq_t req;
    req.data.push_back(IPMI_CMD_STORAGE);
    req.data.push_back("0x47"); // Clear SEL
    req.data.push_back("0x00"); // no partial erase,
    req.data.push_back("0x00"); // so reservation id is set to 0x000
    req.data.push_back("0x43"); // 'C'
    req.data.push_back("0x4c"); // 'L'
    req.data.push_back("0x52"); // 'R'
    req.data.push_back("0xaa"); // initiate erase
    nmprk::ipmi::commandRsp_t rsp;
    if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp) == true) 
    {
		  if(rsp.rspCode == 0xC5) // then need to reserve
      {
    	  // Reserve SEL command
	  	  req.data.clear();
		    req.data.push_back(IPMI_CMD_STORAGE);
        req.data.push_back("0x42");  // Reserve SEL Repository command
	      rsp.data.clear();
    	  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) 
        {
			    if(rsp.rspCode != 0x00) // there was an error
          {
			      std::string msg = NMPRK_CMD_RETURNED_NON_ZERO_MSG;
      	    msg += "0x";
			      msg += nmprk::helper::int2HexStr(rsp.rspCode);
      		  throw new nmprk::nmprkException(NMPRK_CMD_RETURNED_NON_ZERO_CODE,msg);
			    }else{ // command was successful, so clear sel
            // Read SEL record header to get the length
            req.data.clear();
            req.data.push_back(IPMI_CMD_STORAGE);
            req.data.push_back("0x47"); // Clear SEL
            req.data.push_back(rsp.data[0]); 
            req.data.push_back(rsp.data[1]);
            req.data.push_back("0x43"); // 'C'
            req.data.push_back("0x4c"); // 'L'
            req.data.push_back("0x52"); // 'R'
            req.data.push_back("0xaa"); // initiate erase
            nmprk::ipmi::commandRsp_t rsp;
            if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp) == true) 
            {
			        if(rsp.rspCode != 0x00){ // there was an error
                ipmi_rspCodes(rsp.rspCode);
			        }else{ // command was successful, so get status
                ret = true;
              }
            }
            else // It failed for some reason, throw an exception.
            {
        		  std::string msg = NMPRK_CMD_RETURNED_NON_ZERO_MSG;
              msg += "0x";
        		  msg += nmprk::helper::int2HexStr(rsp.rspCode);
              throw new nmprk::nmprkException(NMPRK_CMD_RETURNED_NON_ZERO_CODE,msg);
            }
          }
        }
        else // It failed for some reason, throw an exception.
        {
          std::string msg = NMPRK_CMD_RETURNED_NON_ZERO_MSG;
          msg += "0x";
          msg += nmprk::helper::int2HexStr(rsp.rspCode);
          throw new nmprk::nmprkException(NMPRK_CMD_RETURNED_NON_ZERO_CODE,msg);
        }
      }
      else if(rsp.rspCode == 0x00){ // it was successful
        ret = true;
      }else{ // rspCode > 0 and not 0xC5.
        ipmi_rspCodes(rsp.rspCode);
      }
    }
    else // returned false
    {
			std::string msg = NMPRK_CMD_RETURNED_NON_ZERO_MSG;
      msg += "0x";
			msg += nmprk::helper::int2HexStr(rsp.rspCode);
      throw new nmprk::nmprkException(NMPRK_CMD_RETURNED_NON_ZERO_CODE,msg);
    }
  }else{
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
  }
  return ret;
}

