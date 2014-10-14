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
 * Base64Util.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: oarowojolu
 */

#include <ezbake/common/security/Base64Util.h>
#include <openssl/pem.h>

namespace ezbake { namespace common { namespace security {

::std::string Base64Util::encode(const char * data, int length) {
    BIO *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());              //create BIO chain
    b64 = BIO_push(b64, BIO_new(BIO_s_mem()));  //push new block to chain located in memory (memory based chain)
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
    BIO_write(b64, data, length);               //write data to element
    (void)BIO_flush(b64);                       //flush final block to the chain
    BIO_get_mem_ptr(b64, &bptr);                //get pointer to memory based chain we created

    ::std::string encode(bptr->data, bptr->length);
    BIO_free_all(b64);                          //free all memory allocations in chain

    return encode;
}

::std::string Base64Util::decode(const char * data, int length) {
    BIO *b64;
    b64 = BIO_new(BIO_f_base64());
    b64 = BIO_push(b64, BIO_new_mem_buf(const_cast<char*>(data), length));
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    char *rawdata = new char [length]; //use property that encoded strings are always longer than raw strings
    int buffread = BIO_read(b64, rawdata, length);
    BIO_free_all(b64);

    ::std::string decode(rawdata, buffread);
    delete[] rawdata;
    return decode;
}

}}} //ezbake::common::security
