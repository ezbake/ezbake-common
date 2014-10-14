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
 * TextCryptoProvider.h
 *
 *  Created on: Aug 26, 2014
 *      Author: oarowojolu
 */

#ifndef EZBAKE_COMMON_SECURITY_TEXTCRYPTOPROVIDER_H_
#define EZBAKE_COMMON_SECURITY_TEXTCRYPTOPROVIDER_H_

#include <stdexcept>

namespace ezbake { namespace common { namespace security {


class SecurityException : public std::runtime_error {
public:
    explicit SecurityException(const std::string& what)
        : std::runtime_error("SecurityException: " + what) {}
    explicit SecurityException(const char* what)
        : std::runtime_error(std::string("SecurityException: ") + what) {}
};



class TextCryptoProvider {
public:
    virtual ~TextCryptoProvider() {}

    /**
     * Encrypts a message and return a "encrypted" message.
     *
     * @param message the message to be encrypted
     *
     * @throws SecurityException if an error occurs while encrypting the message
     */
    virtual std::string encrypt(const std::string& message) const = 0;

    /**
     * Decrypt a message and return a plain text message.
     *
     * @param encryptedMessage the message to be decrypted
     *
     * @throws SecurityException if an error occurs while trying to decrypt the message.
     */
    virtual std::string decrypt(const std::string& encryptedMessage) const = 0;
};

}}} // namespace ezbake::common::security

#endif /* EZBAKE_COMMON_SECURITY_TEXTCRYPTOPROVIDER_H_ */
