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
 * NoOpTextCryptoProviderTests.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: oarowojolu
 */


#include "../AllTests.h"

#include <ezbake/common/security/NoOpTextCryptoProvider.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using namespace ::ezbake::common::security;


class NoOpTextCryptoProviderTest : public ::testing::Test {
public:
    NoOpTextCryptoProviderTest() {}
    virtual ~NoOpTextCryptoProviderTest() {}
};


TEST_F(NoOpTextCryptoProviderTest, testEncrypt) {
    std::string message = "My test message";
    boost::shared_ptr<TextCryptoProvider> provider = boost::make_shared<NoOpTextCryptoProvider>();
    EXPECT_EQ(message, provider->encrypt(message)) << "The message has changed, but should not have!";
}


TEST_F(NoOpTextCryptoProviderTest, testDecrypt) {
    std::string message = "My test message";
    boost::shared_ptr<TextCryptoProvider> provider = boost::make_shared<NoOpTextCryptoProvider>();
    EXPECT_EQ(provider->encrypt(message), provider->encrypt(message)) << "The message has changed, but should not have!";
}
