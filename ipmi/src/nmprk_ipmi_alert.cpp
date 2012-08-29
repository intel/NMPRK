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

#include <nmprk_ipmi.h>
#include <nmprk_defines.h>
#include <nmprk_exception.h>
#include <nmprk_errCodes.h>
#include <stdio.h>
#include <string>

bool nmprk::ipmi::alert::registerEventHandler(nmprk::ipmi::device*, nmprk::ipmi::sensorFilter_t, nmprk::ipmi::eventFilter_t, nmprk::ipmi::eventHandler_t)  {
 bool ret = false;
 throw new nmprk::nmprkException(NMPRK_NOT_IMPLEMNETED_CODE,NMPRK_NOT_IMPLEMNETED_MSG);
 return ret;
}

bool nmprk::ipmi::alert::unregisterEventHandler(nmprk::ipmi::device*, nmprk::ipmi::sensorFilter_t, nmprk::ipmi::eventFilter_t) {
 bool ret = false;
 throw new nmprk::nmprkException(NMPRK_NOT_IMPLEMNETED_CODE,NMPRK_NOT_IMPLEMNETED_MSG);
 return ret;
}