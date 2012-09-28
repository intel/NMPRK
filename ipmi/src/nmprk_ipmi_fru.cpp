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
nmprk::ipmi::fruInfo_t*  nmprk::ipmi::fru::getFruInfo(nmprk::ipmi::device* d) 
{
  nmprk::ipmi::fruInfo_t* ret = NULL;
  if(d != NULL) 
  {
    ret = new nmprk::ipmi::fruInfo_t();
    if(ret != NULL) 
    {
      nmprk::ipmi::commandReq_t req;
      req.data.push_back(IPMI_CMD_STORAGE); // Storage netFn
      req.data.push_back("0x10");           // Get Fru Inventory Area Info
      req.data.push_back("0x00");           // Fru Device ID

      nmprk::ipmi::commandRsp_t rsp;
      if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp) == true) 
      {
        if(rsp.rspCode != 0)
          ipmi_rspCodes(rsp.rspCode);
        else
        {
          ret->fruSize[0] = nmprk::helper::hexStr2Int(rsp.data[0]);
          ret->fruSize[1] = nmprk::helper::hexStr2Int(rsp.data[1]);
          int i = nmprk::helper::hexStr2Int(rsp.data[2]);  
          if(i == 0) // if bit[0] == 0 then access by byte
            ret->accessByWord = false;
          else
            ret->accessByWord = true; 
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


/**************************************************************************************************
*
* Name: getFruRpry
*
* Description:  Retreives all FRU data from the Repository. The bytes are read from the FRU
*     Repository an Area at a time and stored in a data array and returned to the colling 
*     routine.
*
* raw 0x0a 0x11 0x00 LS_offSet MS_offSet count
***************************************************************************************************/

nmprk::ipmi::record_t*   nmprk::ipmi::fru::getFruRpry(nmprk::ipmi::device* d) 
{
  nmprk::ipmi::record_t* retRpry = NULL; 
  if(d != NULL) // && recordId != NULL) 
  {
    retRpry = new nmprk::ipmi::record_t();
    if(retRpry != NULL) 
    {
      nmprk::ipmi::record_t* ret = NULL;
      nmprk::ipmi::commandReq_t req;
      nmprk::ipmi::commandRsp_t rsp;
      int offset = 0;               // Initial offset   
      int fru_offset[5];
      int length = 0x8;
      ret = nmprk::ipmi::fru::getFruData(d, offset, length);
      if(ret == NULL){
        delete retRpry;
        retRpry = NULL;
        return retRpry;
      }else{
        for(int j = 0;j < length; j++ )
          retRpry->data[j] = ret->data[j];
        offset += length;
        // Add first 8 bytes and verify checksum of 00
        int checksum = 0;
        for(int j = 0; j < 8; j++)
          checksum += ret->data[j];
        if((checksum & 0xFF) == 0x00)
        {
          fru_offset[0] = ret->data[1]*8;       // Internal area size
          fru_offset[1] = ret->data[2]*8;       // Chassis area size
          fru_offset[2] = ret->data[3]*8;       // Board area size
          fru_offset[3] = ret->data[4]*8;       // Product area size
          fru_offset[4] = ret->data[5]*8;       // Multi Record area size
        }
        else
        {
          throw new nmprk::nmprkException(NMPRK_INVALID_CHECKSUM_CODE,NMPRK_INVALID_CHECKSUM_MSG); 
          delete ret;
          delete retRpry;
          retRpry = NULL;
          return retRpry;
        }
        delete ret;
      }
      // Now read in the five areas
      for(int k = 0; k < 5; k++)
      {
        if(fru_offset[k] > 0)
        {
          // read first 8 bytes
          length = 0x8;
          int temp_length = 0;
          ret = nmprk::ipmi::fru::getFruData(d, fru_offset[k], length);
          if(ret == NULL){
            delete retRpry;
            retRpry = NULL;
            return retRpry;
          }else{
            temp_length = ret->data[1];
            if(temp_length > 0)
              temp_length = --temp_length * 8;
            for(int j = 0;j < length; j++ )
              retRpry->data[offset+j] = ret->data[j];
            offset += length;
            fru_offset[k] += length;
            delete ret;
          }
          while(temp_length > 0) // then there is still more to read
          {
            if(temp_length > 0x20)
              length = 0x20;
            else 
              length = temp_length;
            ret = nmprk::ipmi::fru::getFruData(d, fru_offset[k], length);
            if(ret == NULL){
              delete retRpry;
              retRpry = NULL;
              return retRpry;
            }else{
              length = ret->len;
              for(int j = 0;j < length; j++ )
                retRpry->data[offset+j] = ret->data[j];
              offset += length;
              fru_offset[k] += length;
              temp_length -= length;
              delete ret;
            }
          }
        }
      }
      retRpry->len = offset;
    }else{
      throw new nmprk::nmprkException(NMPRK_FAILED_ALLOC_CODE,NMPRK_FAILED_ALLOC_MSG);
    }
  }else{
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
  }
  return retRpry;
}

/**************************************************************************************************
*
* Name: getFruData
*
* Description:  Retreives FRU data. The offset and length are passed in as to where the data is
*     to be read and the length.  No more than 32 bytes may be read at any time. If a length of 
*     more than 32 bytes is passed it, then it is truncated to 32 bytes.  The length of bytes 
*     read is returned with the data.
*
* raw 0x0a 0x11 0x00 LS_offSet MS_offSet count
***************************************************************************************************/

nmprk::ipmi::record_t* nmprk::ipmi::fru::getFruData(nmprk::ipmi::device* d, int offset, int length) 
{
  nmprk::ipmi::record_t* ret = NULL;
  if(d != NULL) 
  {
    ret = new nmprk::ipmi::record_t();
    if(ret != NULL) 
    {
      nmprk::ipmi::commandReq_t req;
      nmprk::ipmi::commandRsp_t rsp;
      req.data.clear();
      req.data.push_back(IPMI_CMD_STORAGE);
      req.data.push_back("0x11");   // Read Fru Data
      req.data.push_back("0x00");   // Device id
      req.data.push_back(nmprk::helper::int2HexStr(offset & 0xFF)); // Offset lsb 
      req.data.push_back(nmprk::helper::int2HexStr((offset & 0xFF00)>>8)); // Offset msb
      if(length > 0x20)
        length = 0x20;
      req.data.push_back(nmprk::helper::int2HexStr(length)); 
      rsp.data.clear();
      if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) 
      {
        if(rsp.rspCode != 0)
        {
          ipmi_rspCodes(rsp.rspCode);
        }
        else 
        {
          for(int j = 0; j < length; j++ )
            ret->data[j] = nmprk::helper::hexStr2Int(rsp.data[j+1]);
          ret->len = length;
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

// raw 0x0a 0x12 0x00 lsb msb data+
bool nmprk::ipmi::fru::setFruData(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId,nmprk::ipmi::record_t* record) 
{
  bool ret = false;
  if(d != NULL && recordId != NULL && record != NULL) 
  {
    int offset = 0;
    int length = 0x20;
    while(record->len > 0)
    {
      if(record->len < 0x20)
      {
        length = record->len;
      }
      else
      {
        length = 0x20;
      }
      nmprk::ipmi::commandReq_t req;
      req.data.push_back(IPMI_CMD_STORAGE);
      req.data.push_back("0x12");   // Write Fru Data
      req.data.push_back("0x00");   // Device Id
      req.data.push_back(nmprk::helper::int2HexStr(offset & 0xFF)); // offset lsb
      req.data.push_back(nmprk::helper::int2HexStr((offset & 0xFF00)>>8)); // offset msb
      for(int i = 0; i < length; i++)
        req.data.push_back(nmprk::helper::int2HexStr(record->data[offset+i])); 
      offset += length;
      record->len -= length;
      nmprk::ipmi::commandRsp_t rsp;
      if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp) == true) 
      {
        if(rsp.rspCode != 0)
          ipmi_rspCodes(rsp.rspCode);
        else   
          ret = true;
      }else{
        ret = false;
        record->len = 0;
      }
    }
  }else{
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
  }
  return ret;
}

