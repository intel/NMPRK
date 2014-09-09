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
***		transmitted, distributed, or disclosed in any way without Intel�s	  **
***		prior express written permission.									  **
***																			  **
***		No license under any patent, copyright, trade secret or other		  **
***		intellectual property right is granted to or conferred upon you by	  **
***		disclosure or delivery of the Materials, either expressly, by		  **
***		implication, inducement, estoppel or otherwise. Any license under	  **
***		such intellectual property rights must be express and approved by	  **
***		Intel in writing.													  **
********************************************************************************/

#include "CryptoProxy.h"
#include "ByteVector.h"
//#include "RemoteLogger.h"

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <stdexcept>
#include <assert.h>

using namespace std;

CryptoProxy* CryptoProxy::instance = NULL;

// Do not need anything private when using OPENSSL
// struct CryptoIpml {}

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
           :impl(NULL)
{
    //LOG_HIDEBUG << "CryptoProxy init using: " << OPENSSL_VERSION_TEXT;
}

CryptoProxy::~CryptoProxy()
{
}

void
CryptoProxy::hmac(HmacAlgId algId, const ByteVector& data, const ByteVector& key, ByteVector& digest)
{
    const EVP_MD* evpMd = NULL;
    switch (algId)
    {
    case HMAC_MD5:
        evpMd = EVP_md5();
        digest.resize(MD5_HASHSIZE);
        break;

    case HMAC_SHA1:
        evpMd = EVP_sha1();
        digest.resize(SHA1_HASHSIZE);
        break;

    default:
        throw logic_error("Unknown HMAC algorithm ID");
    }
    assert(evpMd != NULL);
//Anandhi - add check for null key
    if ( key.length() == 0)
    {
        //LOG_LODEBUG<<" Null key used for hmac calculation";
        HMAC(evpMd, 
"",key.length(),data.c_ptr(),data.length(),&digest.front(),NULL);
    }
    else
    {
         HMAC(evpMd, key.c_ptr(), key.length(), data.c_ptr(), 
data.length(), &digest.front(), NULL);
    }
//end check - Anandhi
}

void
CryptoProxy::md5(const ByteVector& data, ByteVector& digest)
{
    EVP_MD_CTX ctx;
    int md5Length;

    digest.resize(MD5_HASHSIZE);

    EVP_MD_CTX_init(&ctx);
    EVP_DigestInit_ex(&ctx, EVP_md5(), NULL);
    EVP_DigestUpdate(&ctx, data.c_ptr(), data.length());
    EVP_DigestFinal_ex(&ctx, &digest.front(), NULL);
    EVP_MD_CTX_cleanup(&ctx);
}

void
CryptoProxy::genRand(unsigned int reqSize, ByteVector& randData)
{
    randData.resize(reqSize);
    RAND_bytes(&randData.front(), reqSize);
}

class CryptoKey
{
public:
    CryptoKey(const ByteVector& keyMaterial, const ByteVector& iv);
    ~CryptoKey();

private:
    const ByteVector& k;
    const ByteVector& iv;
    EVP_CIPHER_CTX ctx;
    friend class CryptoProxy;
};

CryptoKey::CryptoKey(const ByteVector& keyMaterial, const ByteVector& iv)
         :k(keyMaterial), iv(iv)
{
    EVP_CIPHER_CTX_init(&ctx);
}

CryptoKey::~CryptoKey()
{
    EVP_CIPHER_CTX_cleanup(&ctx);
}

CryptoKey*
CryptoProxy::createKey(const ByteVector& keyMaterial, const ByteVector& iv)
{
    return new CryptoKey(keyMaterial, iv);
}

void
CryptoProxy::destroyKey(CryptoKey* key)
{
    delete key;
}

void
CryptoProxy::encrypt(CryptoKey* key, const ByteVector& data, ByteVector& encryptedData)
{
    EVP_CIPHER_CTX ctx;

    encryptedData.resize(data.length() + 16);
    int outLength = 0;
    int finalLength = 0;
    
    EVP_EncryptInit_ex(&key->ctx, EVP_aes_128_cbc(), NULL, key->k.c_ptr(), key->iv.c_ptr());
    EVP_CIPHER_CTX_set_padding(&key->ctx, 0);
    if (!EVP_EncryptUpdate(&key->ctx, &encryptedData.front(), &outLength, data.c_ptr(), data.length()))
        throw runtime_error("Failed to EVP_EncryptUpdate()");
    if (!EVP_EncryptFinal_ex(&key->ctx, (&encryptedData.front() + outLength), &finalLength))
        throw runtime_error("Failed to EVP_EncryptFinal_ex()");
    outLength += finalLength;
    encryptedData.resize(outLength);
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
CryptoProxy::decrypt(CryptoKey* key, const ByteVector& data, ByteVector& decryptedData)
{
    decryptedData.resize(data.length());
    int outLength = 0;

    EVP_DecryptInit_ex(&key->ctx, EVP_aes_128_cbc(), NULL, key->k.c_ptr(), key->iv.c_ptr());
    EVP_CIPHER_CTX_set_padding(&key->ctx, 0);
    if (!EVP_DecryptUpdate(&key->ctx, &decryptedData.front(), &outLength, data.c_ptr(), data.length()))
        throw runtime_error("Failed to EVP_DecryptUpdate()");
    decryptedData.resize(outLength);
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

