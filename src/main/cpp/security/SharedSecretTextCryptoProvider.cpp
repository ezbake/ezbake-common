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
 * SharedSecretTextCryptoProvider.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: oarowojolu
 */

#include <ezbake/common/security/SharedSecretTextCryptoProvider.h>
#include <ezbake/common/security/PbeMd5AndDesEncryptor.h>
#include <boost/throw_exception.hpp>
#include <boost/make_shared.hpp>

namespace ezbake { namespace common { namespace security {

const std::string SharedSecretTextCryptoProvider::SALT = "bouncycastle";


std::string SharedSecretTextCryptoProvider::encrypt(const std::string& message) const {
    try {
        return PbeMd5AndDesEncryptor::encrypt(message, _password, SALT);
    } catch (const std::exception &ex) {
        BOOST_THROW_EXCEPTION(SecurityException(std::string("Error in encrypting string: ") + ex.what()));
    }
}


std::string SharedSecretTextCryptoProvider::decrypt(const std::string& encryptedMessage) const {
    try {
        return PbeMd5AndDesEncryptor::decrypt(encryptedMessage, _password, SALT);
    } catch (const std::exception& ex) {
        BOOST_THROW_EXCEPTION(SecurityException(std::string("Error in decrypting string: ") + ex.what()));
    }
}


}}} // namespace ezbake::common::security
