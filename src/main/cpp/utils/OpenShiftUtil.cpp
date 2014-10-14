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
 * OpenShiftUtil.cpp
 *
 *  Created on: Mar 18, 2014
 *      Author: oarowojolu
 */

#include <cstdlib>
#include <boost/filesystem.hpp>
#include <ezbake/common/utils/OpenShiftUtil.h>
#include <ezbake/common/utils/StringConversionUtil.h>


namespace ezbake { namespace common { namespace utils {

const ::std::string OpenShiftUtil::FILE_SEPERATOR = "/";
const ::std::string OpenShiftUtil::OPENSHIFT_REPO_DIR = "OPENSHIFT_REPO_DIR";
const ::std::string OpenShiftUtil::OPENSHIFT_JAVA_THRIFTRUNNER_IP = "OPENSHIFT_JAVA_THRIFTRUNNER_IP";
const ::std::string OpenShiftUtil::OPENSHIFT_JAVA_THRIFTRUNNER_TCP_PORT = "OPENSHIFT_JAVA_THRIFTRUNNER_TCP_PORT";


::std::string OpenShiftUtil::getConfigurationDir() {
    ::std::string dirFromEnv = getRepoDir();

    const ::std::string openShiftConfigDir = "config";
    if (dirFromEnv.find_last_of(FILE_SEPERATOR) == (dirFromEnv.length() - 1)) {
        return dirFromEnv + openShiftConfigDir;
    } else {
        return dirFromEnv + FILE_SEPERATOR + openShiftConfigDir;
    }
}


bool OpenShiftUtil::inOpenShiftContainer() {
    char * env = ::std::getenv(OPENSHIFT_REPO_DIR.c_str());
    return (env ? (::std::string(env).find_first_not_of(' ') != ::std::string::npos) : false);
}


::std::string OpenShiftUtil::getRepoDir() {
    ::std::string dirFromEnv = ::std::getenv(OPENSHIFT_REPO_DIR.c_str());

    if (dirFromEnv.empty()) {
        BOOST_THROW_EXCEPTION(::std::runtime_error("environment var" + OPENSHIFT_REPO_DIR + " not set"));
    }

    return dirFromEnv;
}


::std::string OpenShiftUtil::getRepoPath(const ::std::string& file) {
    if (file.find(FILE_SEPERATOR) == 0) {
        //absolute path
        return file;
    }

    return getRepoDir() + FILE_SEPERATOR + file;
}


HostAndPort OpenShiftUtil::getThriftPrivateInfo() {
    ::std::string ip = ::std::getenv("OPENSHIFT_JAVA_THRIFTRUNNER_IP");
    ::std::string port = ::std::getenv("OPENSHIFT_JAVA_THRIFTRUNNER_TCP_PORT");

    if (::std::string::npos == ip.find_first_not_of(' ')) {
        BOOST_THROW_EXCEPTION(::std::runtime_error("environment var " + OPENSHIFT_JAVA_THRIFTRUNNER_IP + " not set"));
    }
    if (::std::string::npos == port.find_first_not_of(' ')) {
        BOOST_THROW_EXCEPTION(::std::runtime_error("environment var " + OPENSHIFT_JAVA_THRIFTRUNNER_TCP_PORT + " not set"));
    }

    return HostAndPort::fromParts(ip, StringConversionUtil::convertToInt(port, -1));
}


}}} // namespace ::ezbake::common::utils
