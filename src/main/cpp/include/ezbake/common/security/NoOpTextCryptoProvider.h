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
 * NoOpTextCryptoProvider.h
 *
 *  Created on: Aug 26, 2014
 *      Author: oarowojolu
 */

#ifndef EZBAKE_COMMON_SECURITY_NOOPTEXTCRYPTOPROVIDER_H_
#define EZBAKE_COMMON_SECURITY_NOOPTEXTCRYPTOPROVIDER_H_

#include <ezbake/common/security/TextCryptoProvider.h>

namespace ezbake { namespace common { namespace security {


/**
 * This is just a pass through and will NOT encrypt or decrypt anything.
 */
class NoOpTextCryptoProvider : public TextCryptoProvider {
public:
    NoOpTextCryptoProvider() {}
    virtual ~NoOpTextCryptoProvider() {}

    /**
     * This is a no op so just return the passed in message
     *
     * @param message the message to be returned
     *
     * @return the message passed in
     */
    inline virtual std::string encrypt(const std::string& message) const {
        return message;
    }

    /**
     * This is a no op so just returned the passed in "encrypted" message.
     *
     * @param encryptedMessage this should be already decrypted as this is just a pass through
     *
     * @return return the "encrypted" message passed in
     */
    inline virtual std::string decrypt(const std::string& encryptedMessage) const {
        return encryptedMessage;
    }
};

}}} // namespace ezbake::common::security

#endif /* EZBAKE_COMMON_SECURITY_NOOPTEXTCRYPTOPROVIDER_H_ */
