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
 * PbeMd5AndDesTests.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: oarowojolu
 */


#include "../AllTests.h"

#include <ezbake/common/security/PbeMd5AndDesEncryptor.h>

using namespace ::ezbake::common::security;


class PbeMd5AndDesEncryptorTest : public ::testing::Test {
public:
    PbeMd5AndDesEncryptorTest() {}
    virtual ~PbeMd5AndDesEncryptorTest() {}

    static const std::string SALT;
    static const int ITERTAIONS;

    static void runEncryptionTest(const std::string plaintext, const std::string& password,
            const std::string encryptedString) {
        std::string encryptedText = PbeMd5AndDesEncryptor::encrypt(plaintext, password, SALT, ITERTAIONS);
        std::string decryptedText = PbeMd5AndDesEncryptor::decrypt(encryptedText, password, SALT, ITERTAIONS);
        EXPECT_EQ(encryptedString, encryptedText);
        EXPECT_EQ(plaintext, decryptedText);
    }
};


const std::string PbeMd5AndDesEncryptorTest::SALT = "EncryptionFTW";
const int PbeMd5AndDesEncryptorTest::ITERTAIONS = 1000;


TEST_F(PbeMd5AndDesEncryptorTest, testEncryptionAndDecryption) {
    runEncryptionTest("TheKingOfTheNorth", "AWonderfulPassword", "u+rBvJ4eClCtfuLKSopC1yq7QAfjShdG");
    runEncryptionTest("agreatstring", "alsoagreatpassword", "oU+otnnF+D/z0IJbF1/fGw==");
    runEncryptionTest("!@#$%^&()211938fjd3948d!", "@h@rd3rp@55w0rd", "3xci9oIzGPKXJ4xe1Ct4ePxl9Hho5TvFOFjAefTEjys=");
}


TEST_F(PbeMd5AndDesEncryptorTest, testEncryptAndDecryptDollarSigns) {
    std::string plainText = "$mongo99$mongo99";
    std::string password = "ognsVcuS3N4InhpnuiHEliPcb4JSe1FE";

    std::string encryptedText = PbeMd5AndDesEncryptor::encrypt(plainText, password, SALT, ITERTAIONS);
    std::string decryptedText = PbeMd5AndDesEncryptor::decrypt(encryptedText, password, SALT, ITERTAIONS);

    EXPECT_EQ(plainText, decryptedText);
}


TEST_F(PbeMd5AndDesEncryptorTest, testExceptions) {
    //test for exceptions. SALT should be greater than 8 characters
    EXPECT_THROW(PbeMd5AndDesEncryptor::encrypt("", "", "", 100), StringEncryptorException);
    EXPECT_THROW(PbeMd5AndDesEncryptor::decrypt("agreatstring", "alsoagreatpassword", "small", 2), std::runtime_error);
}

