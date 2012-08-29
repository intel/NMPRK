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

#include "KcsIpmiResponse.h"

#include <iomanip>

using namespace std;

KcsIpmiResponse::KcsIpmiResponse(unsigned char completionCode, unsigned int dataLen,
                                             unsigned char* data, bool stripCompCode)
                     :completionCode(completionCode), 
                      data(data, data + dataLen)
{
    //strip the completion code.
    if (stripCompCode == true && this->data.size() > 0)
    {
        this->data.erase(this->data.begin(), this->data.begin()+1);
    }
}

KcsIpmiResponse::KcsIpmiResponse()
                     :completionCode(0xFF)
{
}

std::ostream&
operator<<(std::ostream& os, const KcsIpmiResponse& resp)
{
    os << "comp code: 0x" << hex << setw(2) << setfill('0') << (int)resp.getCompletionCode()
            << " data size: " << dec << (resp.getData().size() -1) << " [";
    for (size_t i = 1; i < resp.getData().size(); ++i)
    {
        os << hex << setw(2) << setfill('0') << (int)resp.getData()[i] << " ";
    }
    os << "]";
    return os;
}
