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
#include "stdafx.h"

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
