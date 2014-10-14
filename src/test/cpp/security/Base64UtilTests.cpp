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
 * Base64UtilTest.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: oarowojolu
 */

#include "../AllTests.h"

#include <ezbake/common/security/Base64Util.h>


class Base64UtilTest : public ::testing::Test {
public:
    Base64UtilTest() {}
    virtual ~Base64UtilTest() {}
};


using namespace ::ezbake::common::security;

TEST_F(Base64UtilTest, EncodeDecode) {
    EXPECT_EQ(Base64Util::encode(""), "");
    EXPECT_EQ(Base64Util::encode("f"), "Zg==");
    EXPECT_EQ(Base64Util::encode("fo"), "Zm8=");
    EXPECT_EQ(Base64Util::encode("foo"), "Zm9v");
    EXPECT_EQ(Base64Util::encode("foob"), "Zm9vYg==");
    EXPECT_EQ(Base64Util::encode("fooba"), "Zm9vYmE=");
    EXPECT_EQ(Base64Util::encode("foobar"), "Zm9vYmFy");
}
