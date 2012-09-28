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

#ifndef NMPRK_IPMI_SDR_H
#define NMPRK_IPMI_SDR_H

#include "nmprk_ipmi.h"
#include "nmprk_defines.h"

namespace nmprk {

  namespace ipmi {
  
    namespace sdr {

      nmprk::ipmi::repoInfo_t* getSdrInfo(nmprk::ipmi::device* d);

      nmprk::ipmi::record_t*   getSdrRecord(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId);

      void setupSdrCall(nmprk::ipmi::commandReq_t *,  nmprk::ipmi::commandRsp_t *, int, int, int, int, int, int);

	  bool addSdrRecord(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId,nmprk::ipmi::record_t* record);

      bool delSdrRecord(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId); 

      bool clearSdr(nmprk::ipmi::device* d);

    };

  };

};

#endif 
