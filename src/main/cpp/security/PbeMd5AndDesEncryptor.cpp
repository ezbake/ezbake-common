/*   Copyright (C) 2013-2014 Computer Sciences Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

/*
 * PbeMd5AndDesEncryptor.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: oarowojolu
 */

#include <ezbake/common/security/PbeMd5AndDesEncryptor.h>
#include <ezbake/common/security/Base64Util.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <boost/throw_exception.hpp>
#include <boost/foreach.hpp>


namespace ezbake { namespace common { namespace security {

const unsigned int PbeMd5AndDesEncryptor::ALGO_BLOCK_SIZE = MD5_DIGEST_LENGTH / 2;


std::string PbeMd5AndDesEncryptor::encrypt(const std::string& plaintext, const std::string& password,
        const std::string salt, long iterations) {
    int padding = ALGO_BLOCK_SIZE - (static_cast<int>(plaintext.length()) % ALGO_BLOCK_SIZE);
    std::string pad(padding, static_cast<char>(padding));
    std::string toEncrypt = plaintext + pad;

    PbeMd5AndDesKey key = generateKey(password, salt, iterations);

    int encryptedReturnedLength = static_cast<int>(toEncrypt.length());
    unsigned char encryptedReturned[encryptedReturnedLength];
    DES_ncbc_encrypt(reinterpret_cast<const unsigned char*>(toEncrypt.data()), encryptedReturned,
            encryptedReturnedLength, &key.schedule, &key.ivec, DES_ENCRYPT);

    return Base64Util::encode(encryptedReturned, encryptedReturnedLength);
}


std::string PbeMd5AndDesEncryptor::decrypt(const std::string& encryptedtext, const std::string& password,
        const std::string salt, long iterations) {
    std::string data = Base64Util::decode(encryptedtext);
    PbeMd5AndDesKey key = generateKey(password, salt, iterations);

    int decryptedReturnedLength = static_cast<int>(data.length());
    unsigned char decryptedReturned[decryptedReturnedLength];
    DES_ncbc_encrypt(reinterpret_cast<const unsigned char*>(data.data()), decryptedReturned,
            decryptedReturnedLength, &key.schedule, &key.ivec, DES_DECRYPT);

    std::string decrypted(reinterpret_cast<char *>(decryptedReturned), decryptedReturnedLength);
    int padValue = static_cast<int>(decrypted[decryptedReturnedLength - 1]);
    return decrypted.substr(0, decryptedReturnedLength - padValue);
}


PbeMd5AndDesEncryptor::PbeMd5AndDesKey PbeMd5AndDesEncryptor::generateKey(const std::string& password,
        const std::string salt, long iterations) {
    std::string keyHash = generateDataHash(password, salt, iterations);

    PbeMd5AndDesKey retVal;
    DES_cblock key;

    keyHash.copy(reinterpret_cast<char *>(&key), ALGO_BLOCK_SIZE, 0);
    keyHash.copy(reinterpret_cast<char *>(&retVal.ivec), ALGO_BLOCK_SIZE, ALGO_BLOCK_SIZE);

    DES_set_odd_parity(&key);
    DES_set_key_checked(&key, &retVal.schedule);

    return retVal;
}


std::string PbeMd5AndDesEncryptor::generateDataHash(const std::string& data, const std::string salt, long iterations) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_create();
    const EVP_MD *algorithm = EVP_md5();
    unsigned char result[MD5_DIGEST_LENGTH];

    if (salt.length() < ALGO_BLOCK_SIZE) {
        BOOST_THROW_EXCEPTION(StringEncryptorException("Provided salt is of insufficient length"));
    }

    /*
     * Add digest to digest stack if not already there.
     * Digest stack gets cleanup upon program termination.
     */
    EVP_add_digest(algorithm);

    if (!EVP_DigestInit_ex(ctx, algorithm, NULL) ||
        !EVP_DigestUpdate(ctx, data.data(), data.length()) ||
        !EVP_DigestUpdate(ctx, salt.data(), ALGO_BLOCK_SIZE) ||
        !EVP_DigestFinal_ex(ctx, result, NULL)) {
        BOOST_THROW_EXCEPTION(StringEncryptorException("Error in generating digest"));
    }

    while (--iterations != 0) {
        if (!EVP_DigestInit_ex(ctx, algorithm, NULL) ||
            !EVP_DigestUpdate(ctx, result, MD5_DIGEST_LENGTH) ||
            !EVP_DigestFinal_ex(ctx, result, NULL)) {
            BOOST_THROW_EXCEPTION(StringEncryptorException("Error in generating digest"));
        }
    }

    EVP_MD_CTX_destroy(ctx);
    return std::string(reinterpret_cast<char *>(result), MD5_DIGEST_LENGTH);
}


}}} // ezbake::common::security
