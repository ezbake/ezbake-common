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
 * UtilsTest.cpp
 *
 *  Created on: Mar 25, 2014
 *      Author: oarowojolu
 */

#include "../AllTests.h"

#include <ezbake/common/HostAndPort.h>


class HostAndPortTest : public ::testing::Test {
public:
    HostAndPortTest() {}
    virtual ~HostAndPortTest() {}
};


using namespace ::ezbake::common;

TEST_F(HostAndPortTest, CreationAndValidation) {
    EXPECT_TRUE(HostAndPort::fromString("localhost:1234").hasPort());
    EXPECT_FALSE(HostAndPort::fromString("localhost").hasPort());
    EXPECT_FALSE(HostAndPort::fromHost("localhost").hasPort());

    EXPECT_EQ(std::string("localhost"), HostAndPort::fromString("localhost:1234").getHostText());
    EXPECT_EQ(std::string("localhost:1234"), HostAndPort::fromParts("localhost", 1234).toString());

    EXPECT_LT(HostAndPort::fromString("localhost:1234"), HostAndPort::fromString("localhost:5678"));
    EXPECT_LT(HostAndPort::fromString("localhost"), HostAndPort::fromString("localhost:5678"));
    EXPECT_FALSE(HostAndPort::fromString("localhost:1234") < HostAndPort::fromString("localhost"));
    EXPECT_LT(HostAndPort::fromString("localhost:1234"), HostAndPort::fromString("www.example.com:1234"));

    EXPECT_EQ(HostAndPort::fromString("localhost:1234"), HostAndPort::fromString("localhost:1234"));
    EXPECT_EQ(HostAndPort::fromString("localhost"), HostAndPort::fromString("localhost"));

    EXPECT_NE(HostAndPort::fromString("localhost:1234"), HostAndPort::fromString("localhost"));
}
