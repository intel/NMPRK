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


#include <iostream>
#include <stdexcept>
#include "KcsIpmiRequest.h"

#include <iomanip>

using namespace std;

KcsIpmiRequest::KcsIpmiRequest(unsigned char dest, unsigned char netFn,
                                           unsigned char lun, unsigned int cmd,
                                           unsigned int dataLen, const unsigned char* data)
                    :dest(dest), netFn(netFn), lun(lun), cmd(cmd)
{
    if (dataLen > 0)
    {
        this->data.assign(data, data + dataLen);
    }
}

std::ostream&
operator<<(std::ostream& os, const KcsIpmiRequest& req)
{
    os << "net_fn: 0x" << hex << setw(2) << setfill('0') << (int)req.getNetFn() 
            << " LUN: 0x" << hex << setw(2) << setfill('0') << (int)req.getLun() 
            << " cmd: 0x" << hex << setw(2) << setfill('0') << (int)req.getCmd() 
            << " data size: " << dec << req.getData().size() << " [ ";
    for (size_t i = 0; i < req.getData().size(); ++i)
    {
        os << hex << setw(2) << setfill('0') << (int)req.getData()[i] << " ";
    }
    os << "]";
    return os;
}
