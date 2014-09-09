/*******************************************************************************
********************************************************************************
***                                                                           **
***							INTEL CONFIDENTIAL								  **
***                    COPYRIGHT 2008 INTEL CORPORATION                       **
***							All Rights Reserved								  **
***                                                                           **
***                INTEL CORPORATION PROPRIETARY INFORMATION                  **
***                                                                           **
***		The source code contained or described herein and all documents		  **
***		related to the source code ("Material") are owned by Intel			  **
***		Corporation or its suppliers or licensors. Title to the Material	  **
***		remains with Intel Corporation or its suppliers and licensors.		  **
***		The Material contains trade secrets and proprietary and confidential  **
***		information of Intel or its suppliers and licensors.				  **
***		The Material is protected by worldwide copyright and trade secret	  **
***		laws and treaty provisions. No part of the Material may be used,	  **
***		copied, reproduced, modified, published, uploaded, posted,			  **
***		transmitted, distributed, or disclosed in any way without Intel’s	  **
***		prior express written permission.									  **
***																			  **
***		No license under any patent, copyright, trade secret or other		  **
***		intellectual property right is granted to or conferred upon you by	  **
***		disclosure or delivery of the Materials, either expressly, by		  **
***		implication, inducement, estoppel or otherwise. Any license under	  **
***		such intellectual property rights must be express and approved by	  **
***		Intel in writing.													  **
********************************************************************************/

#ifndef _BYTE_VECTOR_H
#define _BYTE_VECTOR_H

#include <vector>
#include <string>
#include "IpmiProxy.h"

//typedef unsigned char byte_t;
typedef unsigned int uint32_t;

class ByteVector : public std::vector<byte_t>
{
public:
    ByteVector();

    ByteVector(const byte_t arr[], int len);

    explicit ByteVector(byte_t b);

    const byte_t& operator[](size_type i) const;
    byte_t& operator[](size_type i);

    /**
     * append
     * 
     * @param b	- a byte_t.
     */
    void append(byte_t b);

    /**
     * append
     * 
     * @param bv	- a ByteVector reference.
     * @param n	- an int.
     * @return	- an int.
     */
    int append(const ByteVector& bv, int n);

    /**
     * append
     * 
     * @param bv	- a ByteVector reference.
     * @return	- an int.
     */
    int append(const ByteVector& bv);

    /**
     * append
     * 
     * @param b	- an int.
     * @param n	- an int.
     * @return	- an int.
     */
    int append(int b, int n);

    /**
     * append
     * 
     * @param arr	- a byte_t array.
     * @param n	- an int.
     * @return	- an int.
     */
    int append(const byte_t arr[], int n);

    /**
     * copy
     * 
     * @param b	- an int.
     * @param n	- an int.
     */
    void copy(int b, int n);

    /**
     * copy
     * 
     * @param arr	- a byte_t array.
     * @param n	- an int.
     */
    void copy(const byte_t arr[], int n);

    /**
     * copy
     * 
     * @param bv	- a ByteVector reference.
     * @param start	- an int.
     * @param length	- an int.
     */
    void copy(const ByteVector& bv, int start, int length);

    /**
     * copy
     * 
     * @param bv	- a ByteVector reference.
     * @param len	- an int.
     */
    void copy(const ByteVector& bv, int len);

    /**
     * copyto
     * 
     * @param arr	- a byte_t array.
     * @param len	- an int.
     */
    void copyto(byte_t arr[], int len) const;

    /**
     * append
     * 
     * @param n	- an int.
     * @param b1	- a byte_t.
     * @param b2	- a byte_t.
     */
    void append(int n, byte_t b1, byte_t b2, ...);

    /**
     * c_ptr
     * 
     * @param offset	- an int. Default value =0.
     * @return	- a byte_t pointer.
     */
    const byte_t* c_ptr(int offset = 0) const;

    /**
     * appendRand
     * 
     * @param len	- an int.
     */
    void appendRand(int len);

    /**
     * rand
     * 
     * @param len	- an int.
     */
    void rand(int len);

    /**
     * operator+=
     * 
     * @param bv	- a ByteVector reference.
     */
    void operator+=(const ByteVector& bv);

    /**
     * operator+=
     * 
     * @param b	- a byte_t.
     */
    void operator+=(byte_t b);

    void operator+=(const std::string& s);

    /**
     * length
     * 
     * @return	- an int.
     */
    int length() const;

    /**
     * ulong
     * 
     * @param offset	- an int. Default value =0.
     * @return	- an uint32_t reference.
     */
    uint32_t getUint32(int offset = 0) const;

    void setUint32(uint32_t val, int offset = 0);

    /**
     * operator=
     * 
     * @param str	- a string reference.
     * @return	- a ByteVector reference.
     */
    ByteVector& operator=(const std::string& str);

    /**
     * operator=
     * 
     * @param b	- an int.
     * @return	- a ByteVector reference.
     */
    ByteVector& operator=(int b);

    ByteVector& operator+(const ByteVector& bv);

    bool operator!=(const std::string& s);

    /**
     * range
     * 
     * @param start	- an int.
     * @param end	- an int.
     * @return	- a ByteVector reference.
     */
    ByteVector& range(int start, int end);
};
#endif
