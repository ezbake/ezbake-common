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
 * StringEncryptor.h
 *
 *  Created on: Aug 26, 2014
 *      Author: oarowojolu
 */

#ifndef EZBAKE_COMMON_SECURITY_STRINGENCRYPTOR_H_
#define EZBAKE_COMMON_SECURITY_STRINGENCRYPTOR_H_

#include <stdexcept>

namespace ezbake { namespace common { namespace security {

/**
 * Common String Encryptor Exception
 */
class StringEncryptorException : public std::runtime_error {
public:
    explicit StringEncryptorException(const std::string& what)
        : std::runtime_error(what) {}
    explicit StringEncryptorException(const char* what)
        : std::runtime_error(what) {}
};


/**
 * Abstract Password Based String Encryptor
 */
template <class Implementer>
class PbeStringEncryptor {
public:
    /**
     * Returns an encrypted string
     *
     * @param plaintext     the message to be encrypted
     * @param password      the password used in encrypting the message
     * @param salt          (optional) the salt used in the encryption algorithm.
     *                      Sub classes may choose not to use a salt if their algorithm doesn't require it
     * @param iterations    (optional) number of passes in randomizing the password used in encryption.
     *                      Sub classes may choose not to use a salt if their algorithm doesn't require it
     *
     * @throws StringEncryptorException if an error occurs while encrypting the message
     */
    inline static std::string encrypt(const std::string& plaintext, const std::string& password,
            const std::string salt="", long iterations=1) {
        return Implementer::encrypt(plaintext, password, salt, iterations);
    }

    /**
     * Returns a descrypted string
     *
     * @param encryptedtext the message to be decrypted
     * @param password      the password used in decrypting the message
     * @param salt          (optional) the salt used in the encryption algorithm.
     *                      Sub classes may choose not to use a salt if their algorithm doesn't require it
     * @param iterations    (optional) number of passes in randomizing the password used in encryption.
     *                      Sub classes may choose not to use a salt if their algorithm doesn't require it
     *
     * @throws StringEncryptorException if an error occurs while decrypting the message
     */
    inline static std::string decrypt(const std::string& encryptedtext, const std::string& password,
            const std::string salt="", long iterations=1) {
        return Implementer::decrypt(encryptedtext, password, salt, iterations);
    }
};

}}} // namespace ezbake::common::security

#endif /* EZBAKE_COMMON_SECURITY_STRINGENCRYPTOR_H_ */
