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
#ifndef _CRYPTO_PROXY_H
#define _CRYPTO_PROXY_H

// forward declarations
class ByteVector;

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

#endif

