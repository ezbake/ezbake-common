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
 * HostAndPort.h
 *
 *  Created on: Mar 18, 2014
 *      Author: oarowojolu
 */

#ifndef EZBAKE_COMMON_HOSTANDPORT_H_
#define EZBAKE_COMMON_HOSTANDPORT_H_

#include <stdexcept>
#include <string>
#include <ostream>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>


namespace ezbake { namespace common {

class HostAndPort
{
public:
    static const char DELIM = ':';
    static const unsigned int MIN_PORT = 0;
    static const unsigned int MAX_PORT = 65535;

    static HostAndPort fromHost(const std::string& host) {
        return HostAndPort(host);
    }

    static HostAndPort fromParts(const std::string& host, unsigned int port) {
        return HostAndPort(host, port);
    }

    static HostAndPort fromString(const std::string& str) {
        size_t delimPosition = str.find(DELIM);

        if (std::string::npos == delimPosition) {
            return fromHost(str);
        }

        unsigned int port = boost::lexical_cast<unsigned int>(str.substr(delimPosition+1));
        return fromParts(str.substr(0, delimPosition), port);
    }

    inline std::string getHostText() const {
        return _host;
    }

    inline unsigned int getPort() const {
        if (!_port) {
            BOOST_THROW_EXCEPTION(std::invalid_argument("no port defined"));
        }
        return *_port;
    }

    inline unsigned int getPortOrDefault(unsigned int defaultPort) const {
        try {
            return getPort();
        } catch (const std::invalid_argument& e) {
            return defaultPort;
        }
    }

    inline bool hasPort() const {
        return (_port ? true : false);
    }

    inline std::string toString() const {
        return (hasPort() ? _host + DELIM + std::to_string(static_cast<long long unsigned int>(*_port))
                          : _host + DELIM);
    }

    bool operator==(const HostAndPort& rhs) const {
        if (this->_host == rhs._host) {
            if (this->hasPort() && rhs.hasPort()) {
                return (this->getPort() == rhs.getPort());
            } else if (!this->hasPort() && !rhs.hasPort()) {
                return true;
            }
        }
        return false;
    }

    bool operator!=(const HostAndPort& rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const HostAndPort& rhs) const {
        if (this->_host == rhs._host) {
            if (this->hasPort() && rhs.hasPort()) {
                return (this->getPort() < rhs.getPort());
            } else if (this->hasPort()) {
                return false;
            }
            return true;
        }

        return (this->_host < rhs._host);
    }

    friend std::ostream& operator<< (std::ostream& stream, const HostAndPort& hostAndPort) {
        stream << hostAndPort.toString();
        return stream;
    }

private:
    HostAndPort(const std::string& host, unsigned int port) {
        if (port > MAX_PORT) {
            BOOST_THROW_EXCEPTION(std::out_of_range("port out of range"));
        }

        _host = host;
        _port = port;
    }

    HostAndPort(const ::std::string& host) {
        _host = host;
    }

    std::string _host;
    boost::optional<unsigned int> _port;
};


}} //namespace ::ezbake::common

#endif /* EZBAKE_COMMON_HOSTANDPORT_H_ */
