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
 * LRUCacheTests.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: oarowojolu
 */

#include "../AllTests.h"
#include <ezbake/common/lrucache/LRUCache.h>
#include <string>
#include <utility>

TEST(LRUCacheTest, HandlesBasicPutAndGet) {
    ezbake::common::lrucache::LRUCache<std::string, std::string> cache;

    EXPECT_FALSE(cache.isFull());
    EXPECT_EQ(static_cast<unsigned int>(0), cache.size());

    EXPECT_FALSE(cache.get("Key1"));
    cache.put("Key1", "Value1");
    EXPECT_EQ("Value1", cache.get("Key1").get());
}

TEST(LRUCacheTest, Clear) {
    ezbake::common::lrucache::LRUCache<std::string, std::string> cache;

    EXPECT_TRUE(cache.isEmpty());
    cache.put("Key1", "Value1");
    cache.put("Key2", "Value2");
    cache.put("Key3", "Value3");
    EXPECT_FALSE(cache.isEmpty());

    cache.clear();
    EXPECT_TRUE(cache.isEmpty());
}

TEST(LRUCacheTest, EntrySet) {
    ezbake::common::lrucache::LRUCache<std::string, std::string> cache;
    std::set<std::pair<std::string, std::string> >::iterator itr;
    std::set<std::pair<std::string, std::string> > expectedSet;
    std::set<std::pair<std::string, std::string> > expectedFilteredSet;
    std::string filterKey = "Key2";

    expectedSet.insert(::std::make_pair("Key1", "Value1"));
    expectedSet.insert(::std::make_pair("Key2", "Value21"));
    expectedSet.insert(::std::make_pair("Key2", "Value22"));
    expectedSet.insert(::std::make_pair("Key3", "Value3"));
    expectedSet.insert(::std::make_pair("Key2", "Value23"));
    expectedSet.insert(::std::make_pair("Key4", "Value4"));

    for (itr = expectedSet.begin(); itr != expectedSet.end(); itr++) {
        cache.put(itr->first, itr->second);
        if (itr->first == filterKey) {
            expectedFilteredSet.insert(::std::make_pair(itr->first, itr->second));
        }
    }

    ezbake::common::lrucache::LRUCache<std::string, std::string>::Entry entry;

    itr = expectedSet.begin();
    BOOST_FOREACH(entry, cache.entrySet()) {
        EXPECT_EQ(entry.first, itr->first);
        EXPECT_EQ(entry.second, itr->second);
        itr++;
    }

    itr = expectedFilteredSet.begin();
    BOOST_FOREACH(const std::string& entry, cache.valueSet(filterKey)) {
        EXPECT_EQ(entry, itr->second);
        itr++;
    }
}

TEST(LRUCacheTest, RemovesLRUUponReachingCapacity) {
    ezbake::common::lrucache::LRUCache<std::string, std::string> cache(3);

    cache.put("Key1", "Value1");
    cache.put("Key2", "Value2");
    cache.put("Key3", "Value3");

    //recently used order: Key3, Key2, Key1
    EXPECT_TRUE(cache.containsKey("Key1"));
    EXPECT_TRUE(cache.containsKey("Key2"));
    EXPECT_TRUE(cache.containsKey("Key3"));

    EXPECT_EQ(static_cast<unsigned int>(3), cache.size());
    EXPECT_TRUE(cache.isFull());

    //get Key2 so it becomes our LRU
    EXPECT_EQ("Value2", cache.get("Key2").get());

    //recently used order: Key2, Key3, Key1
    EXPECT_TRUE(cache.containsKey("Key1"));
    EXPECT_TRUE(cache.containsKey("Key2"));
    EXPECT_TRUE(cache.containsKey("Key3"));

    //add new key which will remove the LRU
    cache.put("Key4", "Value4");

    //recently used order: Key4, Key2, Key3
    EXPECT_EQ(static_cast<unsigned int>(3), cache.size());
    EXPECT_TRUE(cache.isFull());
    EXPECT_TRUE(cache.containsValue("Value2"));
    EXPECT_TRUE(cache.containsValue("Value3"));
    EXPECT_TRUE(cache.containsValue("Value4"));
}

TEST(LRUCacheTest, MultiMappedKeys) {
    ezbake::common::lrucache::LRUCache<std::string, std::string> cache(5);

    cache.put("Key1", "Value1");
    cache.put("Key2", "Value21");
    cache.put("Key2", "Value22");
    cache.put("Key2", "Value23");
    cache.put("Key3", "Value3");

    EXPECT_TRUE(cache.isFull());
    cache.put("Key1", "Value1");

    EXPECT_EQ(static_cast<unsigned int>(5), cache.size());

    EXPECT_EQ(static_cast<unsigned int>(3), cache.valueRange("Key2"));
    EXPECT_EQ(static_cast<unsigned int>(1), cache.valueRange("Key1"));
    EXPECT_EQ(static_cast<unsigned int>(0), cache.valueRange("Key4"));

    EXPECT_TRUE(cache.containsKey("Key1"));
    EXPECT_TRUE(cache.containsKey("Key2"));
    EXPECT_TRUE(cache.containsKey("Key3"));
    EXPECT_FALSE(cache.containsKey("Key4"));

    EXPECT_TRUE(cache.containsValue("Value1"));
    EXPECT_TRUE(cache.containsValue("Value21"));
    EXPECT_TRUE(cache.containsValue("Value22"));
    EXPECT_TRUE(cache.containsValue("Value23"));
    EXPECT_TRUE(cache.containsValue("Value3"));

    EXPECT_EQ("Key2", cache.getKey("Value21").get());
    EXPECT_EQ("Key2", cache.getKey("Value22").get());
    EXPECT_EQ("Key2", cache.getKey("Value23").get());

    EXPECT_EQ(static_cast<size_t>(0), cache.remove("InvalidKey").size());
    EXPECT_FALSE(cache.remove("InvalidKey", "Value1"));

    EXPECT_EQ("Value1", cache.pop("Key1").get());
    EXPECT_EQ(static_cast<unsigned int>(4), cache.size());

    EXPECT_EQ(static_cast<size_t>(3), cache.remove("Key2").size());
    EXPECT_EQ(static_cast<unsigned int>(1), cache.size());

    EXPECT_FALSE(cache.isFull());

    EXPECT_FALSE(cache.containsKey("Key1"));
    EXPECT_FALSE(cache.containsKey("Key2"));
    EXPECT_TRUE(cache.containsKey("Key3"));

    EXPECT_FALSE(cache.containsValue("Value1"));
    EXPECT_FALSE(cache.containsValue("Value21"));
    EXPECT_FALSE(cache.containsValue("Value22"));
    EXPECT_FALSE(cache.containsValue("Value23"));
    EXPECT_TRUE(cache.containsValue("Value3"));
}

