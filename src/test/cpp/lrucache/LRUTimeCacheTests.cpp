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
 * LRUTimeCacheTests.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: oarowojolu
 */

#include "../AllTests.h"
#include <ezbake/common/lrucache/LRUTimedCache.h>
#include <string>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <stdexcept>


using namespace ezbake::common::lrucache;

typedef LRUTimedCache<std::string, std::string> TestCache;

TEST(LRUTimedCacheTest, GetAfterExpiration) {
    TestCache cache(5, 2);

    EXPECT_EQ(static_cast<uint64_t>(2),
              cache.expiration());

    EXPECT_FALSE(cache.isFull());
    EXPECT_TRUE(cache.isEmpty());

    EXPECT_FALSE(cache.get("Key1"));
    cache.put("Key1", "Value1");
    EXPECT_EQ("Value1", cache.get("Key1").get());

    boost::this_thread::sleep(boost::posix_time::seconds(2));
    EXPECT_FALSE(cache.get("Key1"));
}

TEST(LRUTimedCacheTest, GetKey) {
    TestCache cache(2);

    EXPECT_FALSE(cache.getKey("Value1"));
    cache.put("Key1", "Value1");
    EXPECT_EQ("Key1", cache.getKey("Value1").get());
}

TEST(LRUTimedCacheTest, Clear) {
    TestCache cache(3);

    EXPECT_TRUE(cache.isEmpty());
    cache.put("Key1", "Value1");
    cache.put("Key2", "Value21");
    cache.put("Key2", "Value22");
    EXPECT_FALSE(cache.isEmpty());

    cache.clear();
    EXPECT_TRUE(cache.isEmpty());
}

TEST(LRUTimedCacheTest, EntrySet) {
    TestCache cache(3);

    cache.put("Key1", "Value1");
    cache.put("Key2", "Value21");
    cache.put("Key2", "Value22");

    EXPECT_TRUE(cache.isFull());

    TestCache::Set set = cache.entrySet();
    EXPECT_EQ(static_cast<unsigned int>(3), set.size());
    EXPECT_NE(set.end(), set.find(TestCache::Entry("Key1", "Value1")));
    EXPECT_NE(set.end(), set.find(TestCache::Entry("Key2", "Value21")));
    EXPECT_NE(set.end(), set.find(TestCache::Entry("Key2", "Value22")));

    TestCache::ValueSet valueSet = cache.valueSet("Key2");
    EXPECT_EQ(static_cast<unsigned int>(2), valueSet.size());
    EXPECT_NE(valueSet.end(), valueSet.find("Value21"));
    EXPECT_NE(valueSet.end(), valueSet.find("Value22"));
}

TEST(LRUTimedCacheTest, ContainsValue) {
    TestCache cache(3, 1);

    cache.put("Key1", "Value1");
    EXPECT_FALSE(cache.containsValue("Value2"));
    EXPECT_TRUE(cache.containsValue("Value1"));

    //wait for entry to expire
    boost::this_thread::sleep(boost::posix_time::seconds(1));
    EXPECT_FALSE(cache.containsValue("Value1"));
    EXPECT_TRUE(cache.isEmpty());
}

TEST(LRUTimedCacheTest, RemoveBeforeExpiration) {
    TestCache cache(5, 2);

    cache.put("Key1", "Value1");
    cache.put("Key2", "Value21");
    cache.put("Key2", "Value22");
    cache.put("Key3", "Value3");
    EXPECT_EQ(static_cast<unsigned int>(4), cache.size());

    EXPECT_EQ("Value1", cache.get("Key1").get());
    EXPECT_EQ("Value21", cache.get("Key2").get());

    std::list<TestCache::CacheValueType> result = cache.remove("Key2");
    EXPECT_EQ(static_cast<size_t>(2), result.size());

    EXPECT_EQ(static_cast<unsigned int>(2), cache.size());
    EXPECT_EQ("Value1", cache.get("Key1").get());

    result = cache.remove("Key1", "Value1");
    EXPECT_EQ(static_cast<size_t>(1), result.size());

    EXPECT_EQ("Value3", cache.pop("Key3").get());

    EXPECT_TRUE(cache.isEmpty());
}

TEST(LRUTimedCacheTest, BoostSharedPtr) {
    LRUTimedCache<std::string, boost::shared_ptr<std::string> > cache(5);

    boost::shared_ptr<std::string> val1 = boost::make_shared<std::string>("Value1");
    cache.put("Key1", val1);

    boost::shared_ptr<std::string> val2 = boost::make_shared<std::string>("Value2");
    cache.put("Key2", val2);

    EXPECT_TRUE(cache.containsKey("Key1"));
    EXPECT_TRUE(cache.containsKey("Key2"));

    EXPECT_TRUE(cache.containsValue(val1));
    EXPECT_TRUE(cache.containsValue(val2));

    EXPECT_EQ("Value1", *(cache.get("Key1").get()));
    EXPECT_EQ("Value2", *(cache.get("Key2").get()));
}

TEST(LRUTimedCacheTest, BoostSharedPtrInherited) {
    LRUTimedCache<std::string, boost::shared_ptr<std::runtime_error> > cache(5);

    boost::shared_ptr<std::overflow_error> val1 = boost::make_shared<std::overflow_error>(std::overflow_error("Value1"));
    cache.put("Key1", val1);

    boost::shared_ptr<std::underflow_error> val2 = boost::make_shared<std::underflow_error>(std::underflow_error("Value2"));
    cache.put("Key2", val2);

    EXPECT_TRUE(cache.containsKey("Key1"));
    EXPECT_TRUE(cache.containsKey("Key2"));

    EXPECT_TRUE(cache.containsValue(val1));
    EXPECT_TRUE(cache.containsValue(val2));

    boost::shared_ptr<std::overflow_error> res1 = boost::dynamic_pointer_cast<std::overflow_error>(cache.get("Key1").get());
    EXPECT_EQ("Value1", std::string(res1->what()));

    boost::shared_ptr<std::underflow_error> res2 = boost::dynamic_pointer_cast<std::underflow_error>(cache.get("Key2").get());
    EXPECT_EQ("Value2", std::string(res2->what()));
}

TEST(LRUTimedCacheTest, RawPointerContainer) {
    LRUTimedCache<std::string, void* > cache(5);

    boost::shared_ptr<std::string> val1 = boost::make_shared<std::string>("Value1");
    cache.put("Key1", val1.get());

    boost::shared_ptr<std::string> val2 = boost::make_shared<std::string>("Value2");
    cache.put("Key2", val2.get());

    EXPECT_TRUE(cache.containsKey("Key1"));
    EXPECT_TRUE(cache.containsKey("Key2"));

    EXPECT_TRUE(cache.containsValue(val1.get()));
    EXPECT_TRUE(cache.containsValue(val2.get()));

    EXPECT_EQ("Value1", *(reinterpret_cast<std::string*>(cache.get("Key1").get())));
    EXPECT_EQ("Value2", *(reinterpret_cast<std::string*>(cache.get("Key2").get())));
}

