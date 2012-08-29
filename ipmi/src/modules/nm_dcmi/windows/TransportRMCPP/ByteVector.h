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

#ifndef _BYTE_VECTOR_H
#define _BYTE_VECTOR_H

#include <vector>
#include <string>

typedef unsigned char byte_t;
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
