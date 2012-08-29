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


#include "CryptoProxy.h"
#include "ByteVector.h"
#include "RemoteLogger.h"

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <stdexcept>
#include <assert.h>

using namespace std;

namespace RmcppTransportPlugin
{

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
    LOG_HIDEBUG << "CryptoProxy init using: " << OPENSSL_VERSION_TEXT;
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
        LOG_LODEBUG<<" Null key used for hmac calculation";
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

}

