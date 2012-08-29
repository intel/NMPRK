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

#ifndef _KCS_IPMI_REQUEST_H
#define _KCS_IPMI_REQUEST_H

#include <vector>

class KcsIpmiRequest
{
public:

    KcsIpmiRequest() { }
    
    /**
     * Ctor.
     *
     * @param dest Destination - BMC address (currently not used due to
     *    compatibility with existing Linux server implementation).
     * @param netFn Network function.
     * @param lun Lun.
     * @param cmd Command.
     * @param dataLen Length of the <code>data</code> array.
     * @param data Data.
     */
    KcsIpmiRequest(unsigned char dest, unsigned char netFn, unsigned char lun,
                         unsigned int cmd, unsigned int dataLen, const unsigned char* data);

    /**
     * Retrieves the Destination.
     *
     * @return Destination.
     */
    inline unsigned char getDest() const;

    /**
     * Retrieves the Network Function.
     *
     * @return Network Function.
     */
    inline unsigned char getNetFn() const;

    /**
     * Retrieves the Lun.
     * 
     * @return Lun.
     */
    inline unsigned char getLun() const;

    /**
     * Retrieves teh Command.
     *
     * @return Command.
     */
    inline unsigned char getCmd() const;

    /**
     * Retrieves the Data.
     *
     * @return Data.
     */
    inline const std::vector<unsigned char>& getData() const;

private:
    /// Destination.
    unsigned char dest;

    /// Network Function.
    unsigned char netFn;

    /// Lun.
    unsigned char lun;

    /// Command.
    unsigned char cmd;

    /// Data.
    std::vector<unsigned char> data;
};

inline
unsigned char
KcsIpmiRequest::getDest() const
{
    return dest;
}

inline
unsigned char
KcsIpmiRequest::getNetFn() const
{
    return netFn;
}

inline
unsigned char
KcsIpmiRequest::getLun() const
{
    return lun;
}

inline
unsigned char
KcsIpmiRequest::getCmd() const
{
    return cmd;
}

inline
const std::vector<unsigned char>&
KcsIpmiRequest::getData() const
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
std::ostream& operator<<(std::ostream& os, const KcsIpmiRequest& req);

#endif //#ifndef _TCP_INBAND_IPMI_REQUEST_H
