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



#ifndef _KCS_IPMI_RESPONSE
#define _KCS_IPMI_RESPONSE

#include <vector>
#include "OsSpecificSockets.h"

class KcsIpmiResponse
{
public:
    /**
     * Default ctor for internal use.
     */
    KcsIpmiResponse();

    /**
     * Ctor.
     * 
     * @param completionCode Completion Code.
     * @param dataLen Specifies length of the <code>data</code>.
     * @param data Data.
     */
    KcsIpmiResponse(unsigned char completionCode, unsigned int dataLen, unsigned char* data, bool stripCompCode = false);

    /**
     * Retrieves the completion code.
     * 
     * @return Completion Code.
     */
    inline unsigned char getCompletionCode() const;

    /**
     * Retrieves the data.
     *
     * @return Data.
     */
    inline const std::vector<unsigned char>& getData() const;

private:
    // Completion Code.
    unsigned char completionCode;

    // Data.
    std::vector<unsigned char> data;
};

inline
unsigned char
KcsIpmiResponse::getCompletionCode() const
{
    return completionCode;
}

inline
const std::vector<unsigned char>&
KcsIpmiResponse::getData() const
{
    return data;
}

/**
 * Dumps content of the request to the specified output stream.
 *
 * @param os Destination output stream.
 * @param req Request.
 *
 * @return Reference to the <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const KcsIpmiResponse& resp);

#endif //#ifndef _TCP_INBAND_IPMI_RESPONSE
