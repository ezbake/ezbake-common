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
 * SharedSecretTextCryptoProvider.h
 *
 *  Created on: Aug 26, 2014
 *      Author: oarowojolu
 */

#ifndef EZBAKE_COMMON_SECURITY_SHAREDSECRETTEXTCRYPTOPROVIDER_H_
#define EZBAKE_COMMON_SECURITY_SHAREDSECRETTEXTCRYPTOPROVIDER_H_

#include <ezbake/common/security/TextCryptoProvider.h>
#include <boost/shared_ptr.hpp>

namespace ezbake { namespace common { namespace security {

class SharedSecretTextCryptoProvider : public TextCryptoProvider {
public:
    SharedSecretTextCryptoProvider(const std::string& secret)
        : _password(secret) {}

    virtual ~SharedSecretTextCryptoProvider() {}

    /**
     * Encrypts a message and return a "encrypted" message.
     *
     * @param message the message to be encrypted
     *
     * @throws SecurityException if an error occurs while encrypting the message
     */
    virtual std::string encrypt(const std::string& message) const;

    /**
     * Decrypt a message and return a plain text message.
     *
     * @param encryptedMessage the message to be decrypted
     *
     * @throws SecurityException if an error occurs while trying to decrypt the message.
     */
    virtual std::string decrypt(const std::string& encryptedMessage) const;

private:
    static const std::string SALT;

    std::string _password;
};

}}} // namespace ezbake::common::security

#endif /* EZBAKE_COMMON_SECURITY_SHAREDSECRETTEXTCRYPTOPROVIDER_H_ */
