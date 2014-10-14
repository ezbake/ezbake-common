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
 * Base64Util.h
 *
 *  Created on: Aug 25, 2014
 *      Author: oarowojolu
 */

#ifndef EZBAKE_COMMON_SECURITY_BASE64UTIL_H_
#define EZBAKE_COMMON_SECURITY_BASE64UTIL_H_

#include <string>

namespace ezbake { namespace common { namespace security {

class Base64Util {
public:
    static std::string encode(const std::string& serializedData) {
        return encode(serializedData.data(), static_cast<int>(serializedData.length()));
    }

    static std::string decode(const std::string& encodedData) {
        return decode(encodedData.data(), static_cast<int>(encodedData.length()));
    }

    static std::string encode(const unsigned char* data, int length) {
        return encode(reinterpret_cast<const char *>(data), length);
    }
    static std::string decode(const unsigned char* data, int length) {
        return decode(reinterpret_cast<const char *>(data), length);
    }

    static std::string encode(const char* data, int length);
    static std::string decode(const char* data, int length);

public:
    Base64Util();
};

}}} //ezbake::common::security

#endif /* EZBAKE_COMMON_SECURITY_BASE64UTIL_H_ */
