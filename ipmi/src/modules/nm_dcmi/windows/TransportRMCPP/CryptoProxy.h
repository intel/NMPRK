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

#ifndef _CRYPTO_PROXY_H
#define _CRYPTO_PROXY_H

// forward declarations
class ByteVector;

namespace RmcppTransportPlugin
{

// forward declarations
class CryptoKey;

/** 
 * Singleton class that provides an interface
 * to the OS specific crypto implementation.
 */
class CryptoProxy
{
public:
    ~CryptoProxy();

    static CryptoProxy* getInstance();

    /// HMAC algorithm IDs.
    enum HmacAlgId
    {
        /// HMAC MD5
        HMAC_MD5,
        /// HMAC SHA-1
        HMAC_SHA1
    };
    
    /// Common hash algorithm block size.
    static const int HASH_ALG_BLOCKSIZE = 64;

    /// MD5 hash algorithm block size.
    static const int MD5_BLOCKSIZE = HASH_ALG_BLOCKSIZE;
    /// MD5 hash algorithm hash size.
    static const int MD5_HASHSIZE = 16;

    /// SHA-1 hash algorithm block size.
    static const int SHA1_BLOCKSIZE = HASH_ALG_BLOCKSIZE;
    /// SHA-1 hash algorithm hash size.
    static const int SHA1_HASHSIZE = 20;

    /**
     * Calculates HMAC.
     * @param algId HMAC Algorithm Id.
     * @param data Input data.
     * @param key Key.
     * @param digest Receives the digest.
     */
    void hmac(HmacAlgId algId, const ByteVector& data, const ByteVector& key, ByteVector& digest);

    /**
     * Calculates MD5 sum.
     * @param data Input data.
     * @param digest Receives the digest.
     */
    void md5(const ByteVector& data, ByteVector& digest);

    /**
     * Generates random data.
     * @param reqSize Required size of <code>randData</code> upon return.
     * @param randData Receives random data.
     */
    void genRand(unsigned int reqSize, ByteVector& randData);

    /**
     * Creates a key that can be reused many times
     * for subsequent <code>encrypt()</code> and <code>decrypt()</code>
     * operations.
     *
     * @param keyMaterial Key material.
     * @param iv Initialization vector for algorithm that will use the key.
     *
     * @return Pointer to the allocated key. Key should be destroyed by
     *      <code>destroyKey()</code> when no longer in use.
     */
    CryptoKey* createKey(const ByteVector& keyMaterial, const ByteVector& iv);
    
    /**
     * Destroys the key created by the <code>createKey()</code>.
     * @param key Pointer to the key.
     */
    void destroyKey(CryptoKey* key);

    /**
     * Encrypts the data.
     * @param key Key to be used.
     * @param data Data to be encrypted.
     * @param encryptedData Receives encrypted data.
     */
    void encrypt(CryptoKey* key, const ByteVector& data, ByteVector& encryptedData);

    /**
     * Decrypts the data.
     * @param key Key to be used.
     * @param data Data to be decrypted.
     * @param encryptedData Receives decrypted data.
     */
    void decrypt(CryptoKey* key, const ByteVector& data, ByteVector& decryptedData);

    /**
     * Encrypts the data.
     * @param keyMaterial Key material to be used to create the key.
     * @param iv IV to be used to create the key.
     * @param data Data to be encrypted.
     * @param encryptedData Receives encrypted data.
     */
    void encrypt(const ByteVector& keyMaterial, const ByteVector& iv, 
                 const ByteVector& data, ByteVector& encryptedData);

    /**
     * Decrypts the data.
     * @param keyMaterial Key material to be used to create the key.
     * @param iv IV to be used to create the key.
     * @param data Data to be decrypted.
     * @param encryptedData Receives decrypted data.
     */
    void decrypt(const ByteVector& keyMaterial, const ByteVector& iv,
                 const ByteVector& data, ByteVector& decryptedData);

    /// Runs a self-test checking provided algorithms with known test vectors.
    void selfTest();

private:
    /// Hidden ctor for singleton.
    CryptoProxy();

    /// Hidden copy ctor for singleton.
    CryptoProxy(CryptoProxy const&);

    /// Hidden assignment operator for singleton.
    CryptoProxy& operator=(CryptoProxy const&);

    /// Pointer to the only instance of the class.
    static CryptoProxy* instance;

    /// Internal function that verifies that all required crypto algoruthms are available.
    void verifyCryptoAlgs();

    /// Pointer to the implementation specific data.
    struct CryptoImpl* impl;
};

}

#endif

