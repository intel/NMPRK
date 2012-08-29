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
#ifndef NMPRK_IPMI_ALERT_H
#define NMPRK_IPMI_ALERT_H

#include "nmprk_ipmi.h"
#include "nmprk_defines.h"

namespace nmprk {

  namespace ipmi {

    namespace alert {
      
      bool registerEventHandler(nmprk::ipmi::device*, nmprk::ipmi::sensorFilter_t, nmprk::ipmi::eventFilter_t, nmprk::ipmi::eventHandler_t);

      bool unregisterEventHandler(nmprk::ipmi::device*, nmprk::ipmi::sensorFilter_t, nmprk::ipmi::eventFilter_t);


    };

  };

};


#endif

