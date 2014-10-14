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
 * PbeMd5AndDes.h
 *
 *  Created on: Aug 25, 2014
 *      Author: oarowojolu
 */

#ifndef EZBAKE_COMMON_SECURITY_PBEMD5ANDDES_H_
#define EZBAKE_COMMON_SECURITY_PBEMD5ANDDES_H_

#include <ezbake/common/security/PbeStringEncryptor.h>
#include <openssl/des.h>

namespace ezbake { namespace common { namespace security {

class PbeMd5AndDesEncryptor : public PbeStringEncryptor<PbeMd5AndDesEncryptor> {
public:
    typedef struct {
        DES_key_schedule schedule;
        DES_cblock       ivec;
    }PbeMd5AndDesKey;


public:
    static const unsigned int ALGO_BLOCK_SIZE;


public:
    /**
     * Returns an encrypted string
     *
     * @param plaintext     the message to be encrypted
     * @param password      the password used in encrypting the message
     * @param salt          the salt used in the encryption algorithm.
     * @param iterations    number of passes in randomizing the password used in encryption.
     *
     * @throws StringEncryptorException if an error occurs while encrypting the message
     */
    static std::string encrypt(const std::string& plaintext, const std::string& password,
            const std::string salt="do provide default", long iterations=1000);

    /**
     * Returns a descrypted string
     *
     * @param encryptedtext the message to be decrypted
     * @param password      the password used in decrypting the message
     * @param salt          the salt used in the encryption algorithm.
     * @param iterations    number of passes in randomizing the password used in encryption.
     *
     * @throws StringEncryptorException if an error occurs while decrypting the message
     */
    static std::string decrypt(const std::string& encryptedtext, const std::string& password,
            const std::string salt="do provide default", long iterations=1000);

    /**
     * Generates a Pbe Md5 & Des Key
     *
     * @param password      required password for the key
     * @param salt          password salt.
     * @param iterations    number of passes in randomizing the password.
     *
     * @throws StringEncryptorException if an error occurs while decrypting the message
     */
    static PbeMd5AndDesKey generateKey(const std::string& password, const std::string salt, long iterations);

private:
    static std::string generateDataHash(const std::string& data, const std::string salt, long iterations);
};

}}} // ezbake::common::security

#endif /* EZBAKE_COMMON_SECURITY_PBEMD5ANDDES_H_ */
