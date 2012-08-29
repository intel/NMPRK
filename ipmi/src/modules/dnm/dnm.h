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
#ifndef NMPRK_IPMI_MODULE_DNM_H
#define NMPRK_IPMI_MODULE_DNM_H

#include "../../../include/nmprk_ipmi.h"
#include "../../../include/nmprk_defines.h"

namespace nmprk {
 
  namespace ipmi {

    namespace modules {
      
      namespace dnm {

	bool dnm_local_connectToDevice(nmprk::ipmi::device* d);
	bool dnm_remote_connectToDevice(nmprk::ipmi::device* d);
	bool dnm_local_disconnectDevice(nmprk::ipmi::device* d);
	bool dnm_remote_disconnectDevice(nmprk::ipmi::device* d);
	bool dnm_local_runIpmiCommand(nmprk::ipmi::device* d, nmprk::ipmi::commandReq_t*, nmprk::ipmi::commandRsp_t*);
	bool dnm_remote_runIpmiCommand(nmprk::ipmi::device* d, nmprk::ipmi::commandReq_t*, nmprk::ipmi::commandRsp_t*);
	bool dnm_initSystemForLocal();

      };
  
    };

  };

};

#endif 
