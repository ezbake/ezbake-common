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
 * LRUTimedCache.h
 *
 *  Created on: Apr 7, 2014
 *      Author: oarowojolu
 */

#ifndef EZBAKE_COMMON_LRUCACHE_LRUTIMEDCACHE_H_
#define EZBAKE_COMMON_LRUCACHE_LRUTIMEDCACHE_H_

#include <stdint.h>

#include <ezbake/common/lrucache/LRUCache.h>
#include <boost/date_time.hpp>
#include <boost/format.hpp>

namespace ezbake { namespace common { namespace lrucache {

template <typename T>
class CacheValue {
public:
    CacheValue(const T& val) :
        _timestamp((boost::posix_time::microsec_clock::universal_time() -
                    boost::posix_time::from_time_t(0)).total_seconds()),
        _value(val)
    {}

    virtual ~CacheValue() {}

    bool operator==(const CacheValue& rhs) const {
        return ((this->_timestamp == rhs._timestamp) &&
                (this->_value == rhs._value));
    }

    bool operator<(const CacheValue& rhs) const {
        return ((this->_value == rhs._value) ?
                (this->_timestamp < rhs._timestamp) :
                (this->_value < rhs._value));
    }

    const uint64_t& timestamp() const {
        return _timestamp;
    }

    const T& value() const {
        return _value;
    }
private:
    uint64_t _timestamp;
    T _value;
};


/**
 * A cache implementation with support for timed expiration of entries
 */
template <typename K, typename V>
class LRUTimedCache : public virtual LRUCache<K, CacheValue<V> > {
public:
    typedef typename std::pair<K, V> Entry;
    typedef typename std::set<V, std::less<V>, std::allocator<V> > ValueSet;
    typedef typename std::set<Entry, std::less<Entry>, std::allocator<Entry> > Set;

    typedef CacheValue<V> CacheValueType;


protected:
    typedef LRUCache<K, CacheValueType > TimedCacheType;
    typedef typename TimedCacheType::Entry TCEntry;
    typedef typename TimedCacheType::Set TCSet;
    typedef typename TimedCacheType::KeyViewConstRef TCKey;
    typedef typename TimedCacheType::ValueViewConstRef TCValue;
    typedef typename TimedCacheType::KeyViewItr TCKeyItr;
    typedef typename TimedCacheType::ValueViewItr TCValueItr;
    typedef typename TimedCacheType::KeyViewItrRange TCKeyItrRange;
    typedef typename TimedCacheType::ValueViewItrRange TCValueItrRange;

public:
    static const unsigned int DEFAULT_MAX_CAPACITY = 1000;
    static const uint64_t DEFAULT_CACHE_EXPIRATION = 43200L;

public:
    /**
     * Create a new LRUTimedCache
     *
     * @param capacity
     * @param expiration in seconds
     */
    LRUTimedCache(unsigned int capacity = DEFAULT_MAX_CAPACITY,
                  uint64_t expiration = DEFAULT_CACHE_EXPIRATION)
        : TimedCacheType(capacity),
          _expiration(expiration) {}

    virtual ~LRUTimedCache() {}

    /**
     * Returns true if this Cache contains a mapping for the specified value
     */
    bool containsValue(const V& lookupValue) {
        return (!getKey(lookupValue) ? false :  true);
    }

    /**
     * Return the configured expiration for the cache
     *
     * @return duration in seconds
     */
    uint64_t expiration() const {
        return _expiration;
    }

    /**
     * Return a set of key and values from the cache
     *
     * @return a copy of the Key, Value pairs in the cache
     */
    Set entrySet() {
        Set set;
        BOOST_FOREACH(const TCEntry& entry, TimedCacheType::entrySet()) {
            set.insert(Entry(entry.first, entry.second.value()));
        }
        return set;
    }

    /**
     * Returns a set of the values contained in this cache
     * that have the specified key
     *
     * @param key used for lookup
     *
     * @return a copy set of values that map to the specified key
     */
    ValueSet valueSet(const K& key) {
        ValueSet set;
        BOOST_FOREACH(const CacheValueType& entry, TimedCacheType::valueSet(key)) {
            set.insert(entry.value());
        }
        return set;
    }

    /**
     * Get objects out of the cache by key
     *
     * @param key to lookup
     * @return optional value containing the returned object from the cache if the key exists
     */
    boost::optional<V> get(const K& key) {
        boost::optional<V> retVal;
        boost::optional<CacheValueType> cacheValue = TimedCacheType::get(key);

        if (cacheValue) {
            CacheValueType value = cacheValue.get();
            if (expired(value.timestamp())) {
                /*
                 * Key exists in cache, but is expired.
                 * Expunge entry from cache and return empty value
                 */
                TimedCacheType::remove(key, value);
            } else {
                retVal = value.value();
            }
        }

        return retVal;
    }

    /**
     * Reverse lookup a key giving the value
     *
     * @param lookupvalue to search for
     *
     * @return boost optional key if value is found in cache
     */
    boost::optional<K> getKey(const V& lookupValue) {
        boost::optional<K> key;

        {//synchronized
            std::lock_guard<std::recursive_mutex> lock(TimedCacheType::mutex());

            TCSet expiredEntries;

            BOOST_FOREACH(TCValue entry, TimedCacheType::cache().right) {
                if (entry.first.value() == lookupValue) {
                    if (expired(entry.first.timestamp())) {
                        //entry has expired
                        expiredEntries.insert(TCEntry(entry.second, entry.first));
                    } else {
                        key = entry.second;
                        break;
                    }
                }
            }

            //remove expired entries
            BOOST_FOREACH(const TCEntry& entry, expiredEntries) {
                TimedCacheType::remove(entry.first, entry.second);
            }
        }

        return key;
    }

    /**
     * Pop an element from the cache. If the key specified maps to multiple values,
     * the least recently accessed value is returned. After retrieval, the element is
     * removed form the cache
     *
     * @param key used for lookup
     *
     * @return boost optional for the value associated with the key
     */
    boost::optional<V> pop(const K& key) {
        boost::optional<V> retVal;
        boost::optional<CacheValueType> cacheValue = TimedCacheType::pop(key);

        if (cacheValue) {
            CacheValueType value = cacheValue.get();
            if (!expired(value.timestamp())) {
                /*
                 * Key exists in cache and has not expired.
                 */
                retVal = value.value();
            }
        }

        return retVal;
    }

    /**
     * Put objects in the cache
     *
     * @param key to store
     * @param value to store
     */
    void put(const K& key, const V& value) {
        TimedCacheType::put(key, CacheValueType(value));
    }

    /**
     * Removes all values associated with the specified key
     *
     * @param key for lookup
     *
     * @return list of values removed. Returns empty list if no mapping found
     */
    std::list<CacheValueType> remove(const K& key) {
        return TimedCacheType::remove(key);
    }

    /**
     * Removes a specific value from the cache, ignoring expirations
     *
     * @param key for lookup
     * @param value for lookup
     *
     * @return list of values removed. Returns empty list if no mapping found
     */
    std::list<CacheValueType> remove(const K& key, const V& value) {
        std::list<CacheValueType> entriesRemoved;

        {//synchronized
            std::lock_guard<std::recursive_mutex> lock(TimedCacheType::mutex());
            TCSet entriesToRemove;

            TCKeyItrRange range = TimedCacheType::cache().left.equal_range(key);
            for(TCKeyItr itr = range.first; itr != range.second; itr++) {
                if (itr->second.value() == value) {
                    entriesToRemove.insert(TCEntry(itr->first, itr->second));
                }
            }

            //remove the entries
            BOOST_FOREACH(const TCEntry& entry, entriesToRemove) {
                TimedCacheType::remove(entry.first, entry.second);

                entriesRemoved.push_back(entry.second);
            }
        }

        return entriesRemoved;
    }

protected:
    virtual bool expired(uint64_t timestamp) const {
        if (0 == _expiration) {
            return false;
        }

        return (boost::posix_time::microsec_clock::universal_time() >=
                (boost::posix_time::from_time_t(timestamp) + ::boost::posix_time::seconds(_expiration)));
    }

private:
    uint64_t _expiration;
};

}}} // namespace ::ezbake::common::lrucache 

#endif /* EZBAKE_COMMON_LRUCACHE_LRUTIMEDCACHE_H_ */

