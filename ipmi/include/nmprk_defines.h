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

#ifndef NMPRK_DEFINES_H
#define NMPRK_DEFINES_H

#include <ctime>
#include <string>
#include <vector>
#ifndef WIN32
#include <stdint.h>
#endif

#define IPMI_CMD_CHASSIS     "0x00"
#define IPMI_CMD_BRIDGE      "0x02"
#define IPMI_CMD_SENSOREVENT "0x04"
#define IPMI_CMD_APPLICATION "0x06"
#define IPMI_CMD_FIRMWARE    "0x08"
#define IPMI_CMD_STORAGE     "0x0a"
#define IPMI_CMD_TRANSPORT   "0x0c"

#include <stdio.h>
#define DBG_MSG(x) { fprintf(stderr,x);fflush(stderr); }

namespace nmprk { 
  
  namespace ipmi {
    // forward declaration
    class device;

    typedef unsigned int byte_t;
  
    typedef struct {
      std::vector<std::string> data;
    }commandReq_t;

    typedef struct {
      unsigned int rspCode;
      std::vector<std::string> data;
    }commandRsp_t;

    typedef struct {
      byte_t       repoVersion;        // Version of the SEL
      unsigned int repoEntries;        // get total entries in SEL
      unsigned int repoFreeSpace;      // the amount of space left in the SEL
      tm           mostRecentAddTS;    // Timestamp from the last add command
      tm           mostRecentDelTS;    // timestamp from the last delete command
      bool         getAllocInfoSup;    // does this SEL support getAllocInfo command
      bool         reserveSup;         // reserve sel supported
      bool         parAddSup;          // partial add sel support
      bool         delSup;             // sel supports delete 
      bool         nonmodalSupported;  // non-modal SDR Repository update supported
      bool         modalSupported;     // modal SDR Repository update supported
    }repoInfo_t;

    typedef struct {
      byte_t lsb;                      // least significant byte in addy
      byte_t msb;                      // most significant byte
    }address_t;
  
    typedef struct {
      address_t nextRecord;            // address of next record
      byte_t data[1024];               // record data
      unsigned int len;                // length of data field
    }record_t;

    typedef struct {
      byte_t fruSize[2];                //total size of fru
      bool   accessByWord;              // access by word or byte
    }fruInfo_t;

    typedef struct {
      byte_t       deviceId;            // the id of this device
      unsigned int deviceRev;           // this devices revision
      bool         deviceProvidesSdr;   // does this device provide SDR access
      unsigned int firmwareRev;         // firmware revision
      bool         devNormOp;           // device is in normal operation mode
      unsigned int firmwareRev2;        // firmware revision 2
      bool         isSensorDev;         // this device is a sensor device
      bool         isSdrRepoDev;        // is a sdr repo device
      bool         isFruInvDev;         // is a fru 
      bool         isIpmbRevDev;         
      bool         isIpmbGenDev;        // is a ipmi device
      bool         isBridgeDev;         // device acts as bridge
      bool         isChassisDev;   
      std::string  manufId;             // manfuctor id
      std::string  productId;           // product id
    }getDeviceIdRsp;

    typedef enum {
      resetCold = 0,
      resetWarm
    }resetReq_t;

    typedef enum {
      stateSysS0G0 = 0x00,
      stateSysS1 = 0x01,
      stateSysS2 = 0x02,
      stateSysS3 = 0x03,
      stateSysS4 = 0x04,
      stateSysS5G2 = 0x05,
      stateSysS4S5 = 0x06,
      stateSysG3 = 0x07,
      stateSysSleeping = 0x08,
      stateSysG1Sleep = 0x09,
      stateSysOverRide = 0x0A,
      stateSysLegacyOn = 0x20 ,
      stateSysLegacyOff = 0x21, 
      stateSysUnknown = 0x2A,
      stateSysNoChange = 0x7f 
    }acpiSystemPwrState_t;

    typedef enum {
      stateDevD0 = 0x00,
      stateDevD1 = 0x01,
      stateDevD2 = 0x02,
      stateDevD3 = 0x03,
      stateDevUnknown = 0x2a,
      stateDevNoChange = 0x7f
    }acpiDevicePwrState_t;

    typedef struct {
      acpiSystemPwrState_t systemState;
      acpiDevicePwrState_t deviceState;
    }acpiPwrState_t;
    
    // Stuff for alerting

    typedef enum {
     sensorSystemEvent = 0x00,
     sensorTemperature,
     sensorPlatformAlert,
     sensorPower,
     sensorProc,
     sensorFan,
     sensorDrive
    }sensorFilter_t;

    typedef struct {
     sensorFilter_t type;
     byte_t*                     alertData;
    }alert_t;

    typedef std::string eventFilter_t;
    typedef void(*eventHandler_t)(nmprk::ipmi::device*,sensorFilter_t,alert_t);

  };

};

#endif
