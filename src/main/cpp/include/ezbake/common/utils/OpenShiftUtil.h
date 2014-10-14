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
 * OpenShiftUtil.h
 *
 *  Created on: Mar 18, 2014
 *      Author: oarowojolu
 */

#ifndef EZBAKE_COMMON_UTILS_OPENSHIFTUTIL_H_
#define EZBAKE_COMMON_UTILS_OPENSHIFTUTIL_H_

#include <ezbake/common/HostAndPort.h>

namespace ezbake { namespace common { namespace utils {

class OpenShiftUtil {
public:
    static const std::string FILE_SEPERATOR;
    static const std::string OPENSHIFT_REPO_DIR;
    static const std::string OPENSHIFT_JAVA_THRIFTRUNNER_IP;
    static const std::string OPENSHIFT_JAVA_THRIFTRUNNER_TCP_PORT;

public:
    static std::string getConfigurationDir();

    static bool inOpenShiftContainer();

    static std::string getRepoDir();

    static std::string getRepoPath(const ::std::string& file);

    static HostAndPort getThriftPrivateInfo();

private:
    OpenShiftUtil();
};

}}} // namespace ::ezbake::common::utils

#endif /* EZBAKE_COMMON_UTILS_OPENSHIFTUTIL_H_ */
