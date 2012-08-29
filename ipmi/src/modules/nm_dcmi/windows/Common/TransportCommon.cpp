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



#include "TransportCommon.h"

#include <iomanip>

using namespace std;

namespace TransportPlugin
{

std::ostream&
operator<<(std::ostream& os, const struct dcmi_req_t& req)
{
    os << "rsAddr " << hex << setw(2) << setfill('0') << (int)req.rsAddr 
            << "rsLun " << hex << setw(2) << setfill('0') << (int)req.rsLun 
            << "netFn " << hex << setw(2) << setfill('0') << (int)req.netFun 
            << " cmd " << setw(2) << setfill('0') << (int) req.cmd 
            << " len " << req.len << " data [";
    for (int i = 0; i < req.len; ++i)
    {
        os << " " << setw(2) << setfill('0') << (int)req.data[i];
    }
    os << " ]" << dec;
    return os;
}

std::ostream&
operator<<(std::ostream& os, const struct dcmi_rsp_t& rsp)
{
    os << "compCode " << hex << setw(2) << setfill('0') << (int) rsp.compCode 
            << " len " << rsp.len << " data [";
    for (int i = 0; i < rsp.len; ++i)
    {
        os << " " << setw(2) << setfill('0') << (int)rsp.data[i];
    }
    os << " ]" << dec;
    return os;
}

}
