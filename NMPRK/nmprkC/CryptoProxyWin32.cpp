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

#include "CryptoProxy.h"

#include "WinExcept.h"
#include <windows.h>
#include <wincrypt.h>

#include <assert.h>

#include "ByteVector.h"
#include "DebugLog.h"

#define SI_THIS_MODULE SI_DEBUG_MODULE_RMCPP

using namespace std;

#define TOE(f, name) if (!(f)) throw WinApiError(name)

CryptoProxy* CryptoProxy::instance = NULL;

struct CryptoImpl
{
    CryptoImpl();
    ~CryptoImpl();

    HCRYPTPROV provider;
};

CryptoImpl::CryptoImpl()
          :provider(NULL)
{
    try
    {
        // get handle to the provider
        TOE(CryptAcquireContext(&provider, 
                                NULL, //use default key container
                                NULL, //use default provider
                                PROV_RSA_AES,
                                CRYPT_SILENT), "CryptAcquireContext(PROV_RSA_AES)");
    }
    catch (WinApiError& e)
    {
        // if the keyset does not exist, try again with another flag
        if (e.getErr() == NTE_BAD_KEYSET)
        {
            TOE(CryptAcquireContext(&provider, 
                                    NULL, //use default key container
                                    NULL, //use default provider
                                    PROV_RSA_AES,
                                    CRYPT_SILENT | CRYPT_NEWKEYSET), 
                                    "CryptAcquireContext(PROV_RSA_AES, CRYPT_NEWKEYSET)");
        }
        else
            throw; //another problem - rethrow
    }
}

CryptoImpl::~CryptoImpl()
{
    if (NULL != provider)
    {
        if (!CryptReleaseContext(provider, 0))
        {
            SI_DEBUG_ERROR(SI_THIS_MODULE, "Error: %s", WinApiError("CryptReleaseContext").what());
        }
    }
}

class CryptoHash
{
public:
    CryptoHash(HCRYPTPROV cryptProv, ALG_ID algId);
    CryptoHash(HCRYPTPROV cryptProv, ALG_ID algId, const ByteVector& data);
    ~CryptoHash();
   
    CryptoHash& update(const ByteVector& data);
    void getValue(ByteVector& value);

private:
    CryptoHash(CryptoHash const&);
    CryptoHash& operator=(CryptoHash const&);
    
    HCRYPTHASH handle;
};

CryptoHash::CryptoHash(HCRYPTPROV cryptProv, ALG_ID algId)
         :handle(NULL)
{
    TOE(CryptCreateHash(cryptProv, algId, 0, 0, &handle), "CryptCreateHash");
}

CryptoHash::CryptoHash(HCRYPTPROV cryptProv, ALG_ID algId, const ByteVector& data)
         :handle(NULL)
{
    TOE(CryptCreateHash(cryptProv, algId, 0, 0, &handle), "CryptCreateHash");
    try
    {
        update(data);
    }
    catch (exception&)
    {
        CryptDestroyHash(handle);
        throw;
    }
}


CryptoHash&
CryptoHash::update(const ByteVector& data)
{
    TOE(CryptHashData(handle, data.c_ptr(), static_cast<DWORD>(data.size()), 0), "CryptHashData");
    return *this;
}

void
CryptoHash::getValue(ByteVector& value)
{
    DWORD reqHashLen = 0;
    TOE(CryptGetHashParam(handle, HP_HASHVAL, NULL, &reqHashLen, 0), "CryptGetHashParam");
    value.resize(reqHashLen);
    TOE(CryptGetHashParam(handle, HP_HASHVAL, &value.front(), &reqHashLen, 0), "CryptGetHashParam");
}

CryptoHash::~CryptoHash()
{
    if (NULL != handle)
    {
        CryptDestroyHash(handle);
        handle = NULL;
    }
}

class CryptoKey
{
public:
    CryptoKey(HCRYPTPROV cryptProv, const ByteVector& keyMaterial, const ByteVector& iv);
    ~CryptoKey();
    operator HCRYPTKEY() const;
private:
    HCRYPTKEY handle;
};

CryptoKey::CryptoKey(HCRYPTPROV cryptProv, const ByteVector& keyMaterial, const ByteVector& iv)
         :handle(NULL)
{
    if (keyMaterial.size() != 0x10)
        throw logic_error("Invalid key length. Expected 128b");
    if (iv.size() != 0x10)
        throw logic_error("Invalid IV length. Expected 128b");

    byte_t blobHeader[] = { 
        // Header
        0x08,                               //plaintextblob
        CUR_BLOB_VERSION, 
        0x00, 0x00,
        ALG_SID_AES_128, 0x66, 0x00, 0x00,  //ALG_ID
        0x10, 0x00, 0x00, 0x00};            //key length in bytes
    
    ByteVector keyToImport;
    keyToImport.append(blobHeader, sizeof(blobHeader));
    keyToImport.append(keyMaterial);

    // import key
    TOE(CryptImportKey(cryptProv, keyToImport.c_ptr(), static_cast<DWORD>(keyToImport.size()),
                       0, 0, &handle), "CryptImportKey");
    
    // set IV
    TOE(CryptSetKeyParam(handle, KP_IV, const_cast<byte_t*>(iv.c_ptr()), 0), "CryptSetKeyParam(IV)");
}

CryptoKey::~CryptoKey()
{
    if (NULL != handle)
    {
        CryptDestroyKey(handle);
        handle = NULL;
    }
}

CryptoKey::operator HCRYPTKEY() const
{
    return handle;
}

class Hmac
{
public:
    Hmac(HCRYPTPROV cryptProv, ALG_ID algId, const ByteVector& keyMaterial);
    void hashData(const ByteVector& data, ByteVector& hash);

private:
    HCRYPTPROV cryptProv;
    ALG_ID algId;
    ByteVector iPad;
    ByteVector oPad;
};

Hmac::Hmac(HCRYPTPROV cryptProv, ALG_ID algId, const ByteVector& keyMaterial)
    :cryptProv(cryptProv), algId(algId)
{
    ByteVector key(keyMaterial);
    if (key.size() > CryptoProxy::HASH_ALG_BLOCKSIZE)
    {
        CryptoHash keyHash(cryptProv, algId, key);
        keyHash.getValue(key);
        assert((algId == CALG_SHA1 && key.size() == 20) || (algId == CALG_MD5 && key.size() == 16));
    }
    iPad = key;
    iPad.append(0, CryptoProxy::HASH_ALG_BLOCKSIZE - static_cast<int>(key.size()));
    oPad = key;
    oPad.append(0, CryptoProxy::HASH_ALG_BLOCKSIZE - static_cast<int>(key.size()));
    for (size_t i =0; i < CryptoProxy::HASH_ALG_BLOCKSIZE; ++i)
    {
        iPad[i] ^= 0x36;
        oPad[i] ^= 0x5c;
    }
}

void
Hmac::hashData(const ByteVector& data, ByteVector& hash)
{
    CryptoHash(cryptProv, algId, iPad).update(data).getValue(hash);
    CryptoHash(cryptProv, algId, oPad).update(hash).getValue(hash);
}

CryptoProxy*
CryptoProxy::getInstance()
{
    if (NULL == instance)
    {
        instance = new CryptoProxy;
    }
    return instance;
}

CryptoProxy::CryptoProxy()
           :impl(new CryptoImpl)
{
    // check whether all required capabilities
    // are supported by the provider
    verifyCryptoAlgs();
}

CryptoProxy::~CryptoProxy()
{
    delete impl;
}

void
CryptoProxy::verifyCryptoAlgs()
{
    bool isMd5Supported = false;
    bool isSha1Supported = false;
    bool isAesSupported = false;

    DWORD flags = CRYPT_FIRST;
    for (;;)
    {
        PROV_ENUMALGS algs;
        DWORD dataLen = sizeof(algs);
        if (!CryptGetProvParam(impl->provider, PP_ENUMALGS, 
                               reinterpret_cast<BYTE*>(&algs), &dataLen, flags))
        {
            DWORD err = GetLastError();
            if (err == ERROR_NO_MORE_ITEMS)
                break; // finished
            // another error - throw
            throw WinApiError("CryptGetProvParams", err);
        }
        switch (algs.aiAlgid)
        {
        case CALG_MD5:
            isMd5Supported = true;
            break;

        case CALG_SHA:
            isSha1Supported = true;
            break;

        case CALG_AES_128:
            isAesSupported = true;
            break;
        }
        flags &= ~CRYPT_FIRST;
    }
    if (!isMd5Supported)
        throw runtime_error("Crypto: MD5 is not supported");
    if (!isSha1Supported)
        throw runtime_error("Crypto: SHA-1 is not supported");
    if (!isAesSupported)
        throw runtime_error("Crypto: AES-128 is not supported");

}

void
CryptoProxy::hmac(HmacAlgId algId, const ByteVector& data, const ByteVector& key, ByteVector& digest)
{
    ALG_ID nativeAlgId = 0;
    switch (algId)
    {
    case HMAC_MD5:
        nativeAlgId = CALG_MD5;
        break;

    case HMAC_SHA1:
        nativeAlgId = CALG_SHA1;
        break;

    default:
        throw logic_error("Unknown HMAC algorithm ID");
    }
    Hmac(impl->provider, nativeAlgId, key).hashData(data, digest);
}

void
CryptoProxy::md5(const ByteVector& data, ByteVector& digest)
{
    CryptoHash(impl->provider, CALG_MD5, data).getValue(digest);
}

void
CryptoProxy::genRand(unsigned int reqSize, ByteVector& randData)
{
    randData.resize(reqSize);
    TOE(CryptGenRandom(impl->provider, reqSize, &randData.front()), "CryptGenRandom");
}

CryptoKey*
CryptoProxy::createKey(const ByteVector& keyMaterial, const ByteVector& iv)
{
    return new CryptoKey(impl->provider, keyMaterial, iv);
}

void
CryptoProxy::destroyKey(CryptoKey* key)
{
    delete key;
}

void
CryptoProxy::encrypt(CryptoKey* key, const ByteVector& data, ByteVector& encryptedData)
{
    if ((data.length() % 16) > 0)
        throw runtime_error("encrypt: incorrect data length");
    encryptedData = data; //Crypto encrypts in-place
    // NOTE: do not reserve block for padding, specify final = FALSE instead
    //encryptedData.append(0x00, 16); // add one extra block as MS wants it for padding
    const DWORD bytesToEncrypt = static_cast<DWORD>(data.size());
    DWORD dataSize = bytesToEncrypt;
    try
    {
        TOE(CryptEncrypt(*key, 0, FALSE/*final*/, 0/*flags*/, &encryptedData.front(),
                         &dataSize, static_cast<DWORD>(encryptedData.size())), "CryptEncrypt 1st");
    }
    catch (WinApiError& e)
    {
        // larger buffer is required??
        if (e.getErr() == ERROR_MORE_DATA)
        {
            encryptedData.append(0x00, dataSize - encryptedData.size());
            // try again, no second catch this time
            dataSize = bytesToEncrypt; //set again to input data length
            TOE(CryptEncrypt(*key, 0, FALSE/*final*/, 0/*flags*/, &encryptedData.front(),
                             &dataSize, static_cast<DWORD>(encryptedData.size())), "CryptEncrypt 2nd");
        }
        else
            throw; // another error -> re-throw
    }
    // strip the padding
    encryptedData.resize(bytesToEncrypt);
}

void
CryptoProxy::encrypt(const ByteVector& keyMaterial, const ByteVector& iv, 
                     const ByteVector& data, ByteVector& encryptedData)
{
    CryptoKey* key = createKey(keyMaterial, iv);
    try
    {
        encrypt(key, data, encryptedData);
    }
    catch (exception&)
    {
        destroyKey(key);
        throw;
    }
    destroyKey(key);
}

void
CryptoProxy::decrypt(const ByteVector& keyMaterial, const ByteVector& iv,
                     const ByteVector& data, ByteVector& decryptedData)
{
    CryptoKey* key = createKey(keyMaterial, iv);
    try
    {
        decrypt(key, data, decryptedData);
    }
    catch (exception&)
    {
        destroyKey(key);
        throw;
    }
    destroyKey(key);
}

void
CryptoProxy::decrypt(CryptoKey* key, const ByteVector& data, ByteVector& decryptedData)
{
    decryptedData = data; //Crypto decrypts in-place
    DWORD dataSize = static_cast<DWORD>(data.size());
    // NOTE: check FINAL again - currently no FINAL to avoid checking padding
    //TOE(CryptDecrypt(*key, 0, TRUE, 0/*flags*/, &decryptedData.front(), &dataSize), "CryptDecrypt");
    TOE(CryptDecrypt(*key, 0, FALSE, 0/*flags*/, &decryptedData.front(), &dataSize), "CryptDecrypt");
    if (dataSize < decryptedData.size())
    {
        decryptedData.resize(decryptedData.size() - dataSize);
    }
}

void
CryptoProxy::selfTest()
{
    // test HMAC MD 5
    ByteVector key;
    ByteVector data;
    ByteVector digest;
    //   case 1
    key.assign(16, 0x0b);
    data.copy((byte_t*)"Hi There", 8);
    const byte_t dig1[] = { 0x92, 0x94, 0x72, 0x7a, 0x36, 0x38, 0xbb, 0x1c, 0x13, 0xf4, 0x8e, 0xf8, 0x15, 0x8b, 0xfc, 0x9d };
    hmac(CryptoProxy::HMAC_MD5, data, key, digest);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "MD5 1: %s", (memcmp(digest.c_ptr(), dig1, 16) == 0 ? "OK" : "FAILED"));
    //   case 2
    key.copy((byte_t*)"Jefe", 4);
    data.copy((byte_t*)"what do ya want for nothing?", 28);
    const byte_t dig2[] = { 0x75, 0x0c, 0x78, 0x3e, 0x6a, 0xb0, 0xb5, 0x03, 0xea, 0xa8, 0x6e, 0x31, 0x0a, 0x5d, 0xb7, 0x38 };
    hmac(CryptoProxy::HMAC_MD5, data, key, digest);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "MD5 2: %s", (memcmp(digest.c_ptr(), dig2, 16) == 0 ? "OK" : "FAILED"));
    //   case 3
    key.assign(16, 0xaa);
    data.assign(50, 0xdd);
    const byte_t dig3[] = {0x56, 0xbe, 0x34, 0x52, 0x1d, 0x14, 0x4c, 0x88, 0xdb, 0xb8, 0xc7, 0x33, 0xf0, 0xe8, 0xb3, 0xf6};
    hmac(CryptoProxy::HMAC_MD5, data, key, digest);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "MD5 3: %s", (memcmp(digest.c_ptr(), dig3, 16) == 0 ? "OK" : "FAILED"));

    // test AES CBC 128
    ByteVector plainText;
    ByteVector encryptedText;
    ByteVector decryptedText;
    //   case 1 - one block
    const byte_t key1[] = {0x06, 0xa9, 0x21, 0x40, 0x36, 0xb8, 0xa1, 0x5b, 0x51, 0x2e, 0x03, 0xd5, 0x34, 0x12, 0x00, 0x06};
    const byte_t iv1[] = {0x3d, 0xaf, 0xba, 0x42, 0x9d, 0x9e, 0xb4, 0x30, 0xb4, 0x22, 0xda, 0x80, 0x2c, 0x9f, 0xac, 0x41};
    const byte_t cipher1[] = {0xe3, 0x53, 0x77, 0x9c, 0x10, 0x79, 0xae, 0xb8, 0x27, 0x08, 0x94, 0x2d, 0xbe, 0x77, 0x18, 0x1a};
    plainText.copy((byte_t*)"Single block msg", 16);
    encrypt(ByteVector(key1, 16), ByteVector(iv1, 16), plainText, encryptedText);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "AES 1: length %s", (encryptedText.length() == 16 ? "OK" : "FAILED"));
    SI_DEBUG_TRACE(SI_THIS_MODULE, "AES 1: content %s", (memcmp(encryptedText.c_ptr(), cipher1, 16) == 0 ? "OK" : "FAILED"));
    decrypt(ByteVector(key1, 16), ByteVector(iv1, 16), encryptedText, decryptedText);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "AES 1: decrypt %s", (memcmp(decryptedText.c_ptr(), plainText.c_ptr(), 16) == 0 ? "OK" : "FAILED"));
    //   case 2 - two blocks
    const byte_t key2[] = {0xc2, 0x86, 0x69, 0x6d, 0x88, 0x7c, 0x9a, 0xa0, 0x61, 0x1b, 0xbb, 0x3e, 0x20, 0x25, 0xa4, 0x5a};
    const byte_t iv2[] = {0x56, 0x2e, 0x17, 0x99, 0x6d, 0x09, 0x3d, 0x28, 0xdd, 0xb3, 0xba, 0x69, 0x5a, 0x2e, 0x6f, 0x58};
    const byte_t cipher2[] = {0xd2, 0x96, 0xcd, 0x94, 0xc2, 0xcc, 0xcf, 0x8a, 0x3a, 0x86, 0x30, 0x28, 0xb5, 0xe1, 0xdc, 0x0a,
                              0x75, 0x86, 0x60, 0x2d, 0x25, 0x3c, 0xff, 0xf9, 0x1b, 0x82, 0x66, 0xbe, 0xa6, 0xd6, 0x1a, 0xb1};
    plainText.clear();
    for (byte_t i = 0; i < 32; plainText += i++);
    encrypt(ByteVector(key2, 16), ByteVector(iv2, 16), plainText, encryptedText);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "AES 2: length %s", (encryptedText.length() == 32 ? "OK" : "FAILED"));
    SI_DEBUG_TRACE(SI_THIS_MODULE, "AES 2: content %s", (memcmp(encryptedText.c_ptr(), cipher2, 32) == 0 ? "OK" : "FAILED"));
    decrypt(ByteVector(key2, 16), ByteVector(iv2, 16), encryptedText, decryptedText);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "AES 2: decrypt %s", (memcmp(decryptedText.c_ptr(), plainText.c_ptr(), 32) == 0 ? "OK" : "FAILED"));
    //   case 3 - three blocks
    const byte_t key3[] = {0x6c, 0x3e, 0xa0, 0x47, 0x76, 0x30, 0xce, 0x21, 0xa2, 0xce, 0x33, 0x4a, 0xa7, 0x46, 0xc2, 0xcd};
    const byte_t iv3[] = {0xc7, 0x82, 0xdc, 0x4c, 0x09, 0x8c, 0x66, 0xcb, 0xd9, 0xcd, 0x27, 0xd8, 0x25, 0x68, 0x2c, 0x81};
    const byte_t cipher3[] = {0xd0, 0xa0, 0x2b, 0x38, 0x36, 0x45, 0x17, 0x53, 0xd4, 0x93, 0x66, 0x5d, 0x33, 0xf0, 0xe8, 0x86,
                              0x2d, 0xea, 0x54, 0xcd, 0xb2, 0x93, 0xab, 0xc7, 0x50, 0x69, 0x39, 0x27, 0x67, 0x72, 0xf8, 0xd5,
                              0x02, 0x1c, 0x19, 0x21, 0x6b, 0xad, 0x52, 0x5c, 0x85, 0x79, 0x69, 0x5d, 0x83, 0xba, 0x26, 0x84};
    plainText.copy((byte_t*)"This is a 48-byte message (exactly 3 AES blocks)", 48);
    encrypt(ByteVector(key3, 16), ByteVector(iv3, 16), plainText, encryptedText);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "AES 3: length %s", (encryptedText.length() == 48 ? "OK" : "FAILED"));
    SI_DEBUG_TRACE(SI_THIS_MODULE, "AES 3: content %s", (memcmp(encryptedText.c_ptr(), cipher3, 48) == 0 ? "OK" : "FAILED"));
    decrypt(ByteVector(key3, 16), ByteVector(iv3, 16), encryptedText, decryptedText);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "AES 3: decrypt %s", (memcmp(decryptedText.c_ptr(), plainText.c_ptr(), 48) == 0 ? "OK" : "FAILED"));
}
