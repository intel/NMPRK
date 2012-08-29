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


#include <stdlib.h>
#include "ByteVector.h"
#include <stdio.h>
#include <stdarg.h>

ByteVector::ByteVector()
{
}

ByteVector::ByteVector(const byte_t arr[], int len)
{
    for (int i=0; i < len; i++)
    {
        push_back(arr[i]);
    }
}

ByteVector::ByteVector(byte_t b)
{
    push_back(b);
}

const byte_t&
ByteVector::operator[](size_type i) const
{
    try
    {
        return at( i );
    }
    catch (std::exception&)
    {
        throw (int) i;
    }
}

byte_t&
ByteVector::operator[](size_type i)
{
    try
    {
        return at( i );
    }
    catch (std::exception&)
    {
        throw (int) i;
    }
}

void
ByteVector::append(byte_t b)
{
    push_back(b);
}

int
ByteVector::append(const ByteVector& bv, int n)
{
    int i, sz;
    sz = (int)bv.size();
    reserve(size() + sz);

    for (i=0; i < n && i < sz; i++)
    {
        push_back(bv[i]);
    }
    return i;
}

int
ByteVector::append(const ByteVector &bv)
{
    size_t i, bvSz;
    bvSz = bv.size();
    reserve(size() + bv.size() );

    for (i=0; i < bvSz; i++)
    {
        push_back(bv[i]);
    }
    return (int)i;
}

int
ByteVector::append(int b, int n)
{
    for (; n > 0; n--)
    {
        push_back(static_cast<byte_t>(b));
    }
    return n;
}

int
ByteVector::append(const byte_t arr[], int n)
{
    for (int i = 0; i < n; i++)
    {
        push_back(arr[i]);
    }
    return n;
}

void
ByteVector::copy(int b, int n)
{
    resize(0);
    append(b, n);
}

void
ByteVector::copy(const byte_t arr[], int n)
{
    resize( 0);
    reserve(n);

    append(arr, n);
}

void
ByteVector::copy(const ByteVector& bv, int start, int length)
{
    resize( 0);
    reserve(length);

    int end = start+length;
    for (; start < end; start++)
    {
        push_back(bv[start]);
    }
}

void
ByteVector::copy(const ByteVector& bv, int len)
{
    resize(0);
    reserve(len);

    for (int i=0; i < len; i++)
    {
        push_back(bv[i]);
    }
}

void
ByteVector::copyto(byte_t arr[], int len) const
{
    for (int i=0; i < len; i++)
    {
        arr[i] = at(i);
    }
}

void
ByteVector::append(int n, byte_t b1, byte_t b2, ...)
{
    byte_t bn;

    push_back(b1);
    push_back(b2);

    va_list argptr;
    va_start(argptr, b2);

    for (; n > 2; n--)
    {
        bn = static_cast<byte_t>(va_arg(argptr, int));
        push_back(bn);
    }
    va_end(argptr);
}

const byte_t*
ByteVector::c_ptr(int offset) const
{
    return size() > 0 ? &at(offset) : NULL;
}

void
ByteVector::appendRand(int len)
{
    for (; len > 0; len--)
    {
        push_back( (byte_t)::rand()%0xff );
    }
}

void
ByteVector::rand(int len)
{
    resize(0);
    appendRand(len);
}

void
ByteVector::operator+=(const ByteVector& bv)
{
    append(bv);
}

void
ByteVector::operator+=(byte_t b)
{
    push_back(b);
}

void
ByteVector::operator+=(const std::string& s)
{
    size_t szStr = s.size();
    for( size_t sStr=0; sStr<szStr; sStr++ )
    {
        push_back(s[sStr]);
    }
}

int
ByteVector::length() const
{
    return static_cast<int>(size());
}
uint32_t
ByteVector::getUint32(int offset) const
{
    uint32_t retVal = 0;
    for (int i=3; i>=0; i--)
    {
        retVal = (retVal<<8) + (*this)[i+offset];
    }
    return retVal;
}

void
ByteVector::setUint32(uint32_t val, int offset)
{
    for (int i=0; i<3; i++)
    {
        (*this)[i+offset] = static_cast<byte_t>((val>>(8*i))&0xff);
    }
}
ByteVector&
ByteVector::operator=(const std::string& str)
{
    size_t strSz = str.size();
    resize(0);

    for( size_t i=0; i < strSz; i++ )
    push_back( (byte_t)str[i] );
    return *this;
}

ByteVector&
ByteVector::operator=(int b)
{
    resize( 0 );
    push_back(static_cast<byte_t>(b));
    return *this;
}

ByteVector&
ByteVector::operator+(const ByteVector& bv)
{
    append(bv);
    return *this;
}
bool
ByteVector::operator!=(const std::string& s)
{
    return s.compare(0, size(), reinterpret_cast<const char*>(c_ptr())) != 0;
}

ByteVector&
ByteVector::range(int start, int end)
{
    static int i = 0;
    static ByteVector bv[15];
    i = (i+1) % 15;

    bv[i].copy(*this, start, end+1 - start);
    return bv[i];
}
