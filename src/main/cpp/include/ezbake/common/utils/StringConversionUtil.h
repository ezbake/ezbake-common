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
 * StringConversionUtil.h
 *
 *  Created on: Mar 18, 2014
 *      Author: oarowojolu
 */

#ifndef EZBAKE_COMMON_UTILS_STRINGCONVERSIONUTIL_H_
#define EZBAKE_COMMON_UTILS_STRINGCONVERSIONUTIL_H_

#include <cstdlib>
#include <string>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>


namespace ezbake { namespace common { namespace utils {

class StringConversionUtil
{
public:
    static bool convertToBoolean(const ::boost::optional< ::std::string>& toConvert, bool defaultValue) {
        if (!toConvert) {
            return defaultValue;
        }

        ::std::string transform = toConvert.get();
        ::std::transform(transform.begin(), transform.end(), transform.begin(), ::tolower);

        if ((transform == "true") || (transform == "1") || (transform == "t") ||
            (transform == "y") || (transform == "yes")) { return true; }
        return false;
    }

    static double convertToDouble(const ::boost::optional< ::std::string>& toConvert, double defaultValue) {
        if (!toConvert) {
            return defaultValue;
        }

        return ::boost::lexical_cast<double>(toConvert.get());
    }

    static float convertToFloat(const ::boost::optional< ::std::string>& toConvert, float defaultValue) {
        if (!toConvert) {
            return defaultValue;
        }

        return ::boost::lexical_cast<float>(toConvert.get());
    }

    static int convertToInt(const ::boost::optional< ::std::string>& toConvert, int defaultValue) {
        if (!toConvert) {
            return defaultValue;
        }

        return ::boost::lexical_cast<int>(toConvert.get());
    }

    static long convertToLong(const ::boost::optional< ::std::string>& toConvert, long defaultValue) {
        if (!toConvert) {
            return defaultValue;
        }

        return ::boost::lexical_cast<long>(toConvert.get());
    }

};

}}} //namespace ::ezbake::common::utils

#endif /* EZBAKE_COMMON_UTILS_STRINGCONVERSIONUTIL_H_ */
