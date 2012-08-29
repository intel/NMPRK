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


#include "dnm.h"
#ifdef  WIN32
#include "../../../include/nmprk_exception.h"
#include "../../../include/nmprk_errCodes.h"
#include "../../../include/nmprk_ipmi.h"
#include "../../../include/nmprk_defines.h"
#else  // Do it the linux build way
#include <nmprk_exception.h>
#include <nmprk_errCodes.h>
#include <nmprk_ipmi.h>
#include <nmprk_defines.h>
#endif

bool  nmprk::ipmi::modules::dnm::dnm_local_connectToDevice(nmprk::ipmi::device* d) {
 bool ret = false;
 throw new nmprk::nmprkException(NMPRK_NOT_IMPLEMNETED_CODE,NMPRK_NOT_IMPLEMNETED_MSG);
 return ret;
}

bool  nmprk::ipmi::modules::dnm::dnm_remote_connectToDevice(nmprk::ipmi::device* d) {
  bool ret = false;
  throw new nmprk::nmprkException(NMPRK_NOT_IMPLEMNETED_CODE,NMPRK_NOT_IMPLEMNETED_MSG);
  return ret;
}

bool  nmprk::ipmi::modules::dnm::dnm_local_disconnectDevice(nmprk::ipmi::device* d) {
  bool ret = false;
  throw new nmprk::nmprkException(NMPRK_NOT_IMPLEMNETED_CODE,NMPRK_NOT_IMPLEMNETED_MSG);
  return ret;
}

bool  nmprk::ipmi::modules::dnm::dnm_remote_disconnectDevice(nmprk::ipmi::device* d) {
  bool ret = false;
  throw new nmprk::nmprkException(NMPRK_NOT_IMPLEMNETED_CODE,NMPRK_NOT_IMPLEMNETED_MSG);
  return ret;
}

bool  nmprk::ipmi::modules::dnm::dnm_local_runIpmiCommand(nmprk::ipmi::device* d, nmprk::ipmi::commandReq_t* req, nmprk::ipmi::commandRsp_t* rsp) {
  bool ret = false;
  throw new nmprk::nmprkException(NMPRK_NOT_IMPLEMNETED_CODE,NMPRK_NOT_IMPLEMNETED_MSG);
  return ret;
}

bool  nmprk::ipmi::modules::dnm::dnm_remote_runIpmiCommand(nmprk::ipmi::device* d, nmprk::ipmi::commandReq_t* req, nmprk::ipmi::commandRsp_t* rsp) {
  bool ret = false;
  throw new nmprk::nmprkException(NMPRK_NOT_IMPLEMNETED_CODE,NMPRK_NOT_IMPLEMNETED_MSG);
  return ret;
}

bool  nmprk::ipmi::modules::dnm::dnm_initSystemForLocal() {
  bool ret = false;
  throw new nmprk::nmprkException(NMPRK_NOT_IMPLEMNETED_CODE,NMPRK_NOT_IMPLEMNETED_MSG);
  return ret;
}


