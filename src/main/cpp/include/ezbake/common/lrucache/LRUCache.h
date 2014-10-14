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
 * LRUCache.h
 *
 *  Created on: Apr 7, 2014
 *      Author: oarowojolu
 */

#ifndef EZBAKE_COMMON_LRUCACHE_LRUCACHE_H_
#define EZBAKE_COMMON_LRUCACHE_LRUCACHE_H_

#include <list>
#include <mutex>
#include <unordered_set>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>
#include <boost/utility.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
#include <boost/bimap/support/lambda.hpp>


namespace ezbake { namespace common { namespace lrucache { 


/**
 * A cache implementation with a configurable size limit (default is to not have a limit set)
 * which removes the least recently used entry if an entry is added when full.
 *
 * Get and Put access are synchronized and thread-safe
 */

template <typename K, typename V>
class LRUCache : boost::noncopyable {
public:
    typedef typename std::pair<K, V> Entry;
    typedef typename std::set<V, std::less<V>, std::allocator<V> > ValueSet;
    typedef typename std::set<Entry, std::less<Entry>, std::allocator<Entry> > Set;


protected:
    typedef boost::bimaps::bimap<boost::bimaps::unordered_multiset_of<K>,
                                 boost::bimaps::list_of<V> > CacheType;

    typedef typename CacheType::value_type CacheEntry;
    typedef typename CacheType::left_reference KeyViewRef;
    typedef typename CacheType::right_reference ValueViewRef;
    typedef typename CacheType::left_const_reference KeyViewConstRef;
    typedef typename CacheType::right_const_reference ValueViewConstRef;
    typedef typename CacheType::left_iterator KeyViewItr;
    typedef typename CacheType::right_iterator ValueViewItr;
    typedef typename std::pair<KeyViewItr, KeyViewItr> KeyViewItrRange;
    typedef typename std::pair<ValueViewItr, ValueViewItr> ValueViewItrRange;

public:
    /**
     * Constructor
     *
     * @param capacity of the cache. Default value is zero meaning no limit
     */
    LRUCache(unsigned int capacity = 0) : _capacity(capacity) {}

    /**
     * Destructor
     */
    virtual ~LRUCache() {}

    /**
     * Returns the capacity of the cache
     *
     * @return capacity of cache. If '0' returns, cache is not capacity limited
     */
    virtual unsigned int capacity() const {
        return _capacity;
    }

    /**
     * Returns true if this Cache contains a mapping for the specified key
     */
    bool containsKey(const K& lookupKey) {
        //synchronized
        std::lock_guard<std::recursive_mutex> lock(_m);
        return (_cache.left.find(lookupKey) != _cache.left.end());
    }

    /**
     * Returns true if this Cache contains a mapping for the specified value
     */
    bool containsValue(const V& lookupValue) {
        return (!getKey(lookupValue) ? false :  true);
    }

    /**
     * Removes all of the mappings from the cache.
     */
    virtual void clear() {
        std::lock_guard<std::recursive_mutex> lock(_m);
        _cache.clear();
    }

    /**
     * Returns a set of the mappings contained in this cache
     *
     * @return a copy set of entries
     */
    Set entrySet() {
        Set set;

        {//synchronized
            std::lock_guard<std::recursive_mutex> lock(_m);
            BOOST_FOREACH(KeyViewConstRef entry, _cache.left) {
                set.insert(Entry(entry.first, entry.second));
            }
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

        {//synchronized
            std::lock_guard<std::recursive_mutex> lock(_m);
            KeyViewItrRange range = _cache.left.equal_range(key);
            KeyViewItr itr = range.first;
            do {
                set.insert(itr->second);
                ++itr;
            } while (itr != range.second);
        }

        return set;
    }

    /**
     * Get an element from the cache. If the key specified maps to multiple values,
     * the least recently accessed value is returned.
     *
     * @param key used for lookup
     *
     * @return boost optional for the value associated with the key
     */
    boost::optional<V> get(const K& key) {
        boost::optional<V> retVal;
        KeyViewItr entry;

        {//synchronized
            std::lock_guard<std::recursive_mutex> lock(_m);

            if (getLruEntry(key, entry)) {
                retVal = entry->second;

                /*
                 * Relocate the entry based on our list view (right view of bimap)
                 * This marks the entry as the most recently used
                 */
                _cache.right.relocate(_cache.right.end(), _cache.project_right(entry));
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
            std::lock_guard<std::recursive_mutex> lock(_m);
            BOOST_FOREACH(ValueViewConstRef valueEntry, _cache.right) {
                if (valueEntry.first == lookupValue) {
                    key = valueEntry.second;
                    break;
                }
            }
        }

        return key;
    }

    /**
     * Returns true if this cache is empty
     */
    virtual bool isEmpty() {
        return (size() == 0);
    }

    /**
     * Returns true if this cache is full and no new entry can be added
     * without removing the least recently used entry
     */
    virtual bool isFull() {
        return (_capacity == 0) ? false : (size() >= _capacity);
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
        KeyViewItr entry;

        {//synchronized
            std::lock_guard<std::recursive_mutex> lock(_m);

            if (getLruEntry(key, entry)) {
                retVal = entry->second;
                _cache.left.erase(entry); //remove entry from the cache
            }
        }

        return retVal;
    }

    /**
     * Put an element into the cache replacing duplicate Key-Value pairs.
     * Cache is treated as a multimap for insertion.
     *
     * @param key used for lookup
     * @param value associated with key
     *
     */
    void put(const K& key, const V& value) {
        {//synchronized
            std::lock_guard<std::recursive_mutex> lock(_m);

            //check for a duplicate Key-Value pair
            KeyViewItr itr = findEntry(key, value);
            if (itr != _cache.left.end()) {
                //We found a duplicate Key-Value pair. Remove
                _cache.left.erase(itr);
            }

            //if we've reached capacity
            if (_capacity && (_cache.size() >= _capacity)) {
                //remove least recently used Key-Value pair
                _cache.right.erase(_cache.right.begin());
            }

            //add to cache
            _cache.insert(CacheEntry(key, value));
        }
    }

    /**
     * Removes all values associated with the specified key
     *
     * @param key for lookup
     *
     * @return list of values removed. Returns empty list if no mapping found
     */
    std::list<V> remove(const K& key) {
        std::list<V> valuesRemoved;

        {//synchronized
            std::lock_guard<std::recursive_mutex> lock(_m);

            KeyViewItrRange range = _cache.left.equal_range(key);
            for(KeyViewItr itr = range.first; itr != range.second; itr++) {
                valuesRemoved.push_back(itr->second);
            }

            //remove all values associated with key
            _cache.left.erase(key);
        }

        return valuesRemoved;
    }

    /**
     * Removes a specific value from the cache
     *
     * @param key for lookup
     * @param value for lookup
     *
     * @return a boost optional set with the value removed if the mapping existed
     */
    boost::optional<V> remove(const K& key, const V& value) {
        boost::optional<V> valueRemoved;

        {//synchronized
            std::lock_guard<std::recursive_mutex> lock(_m);

            KeyViewItr itr = findEntry(key, value);
            if (itr != _cache.left.end()) {
                valueRemoved = (itr->second);
                _cache.left.erase(itr);
            }
        }

        return valueRemoved;
    }

    /**
     * Return the current size of the cache
     */
    virtual unsigned int size() {
        //synchronized
        std::lock_guard<std::recursive_mutex> lock(_m);
        return static_cast<unsigned int>(_cache.size());
    }

    /**
     * Returns the number of values that map to the specified key
     */
    virtual unsigned int valueRange(const K& key) {
        //synchronized
        std::lock_guard<std::recursive_mutex> lock(_m);
        KeyViewItrRange range = _cache.left.equal_range(key);
        return static_cast<unsigned int>(std::distance(range.first, range.second));
    }

protected:
    std::recursive_mutex& mutex() {
        return _m;
    }

    CacheType& cache() {
        return _cache;
    }

private:
    KeyViewItr findEntry(const K& key, const V& value) {
        KeyViewItrRange range = _cache.left.equal_range(key);
        for(KeyViewItr itr = range.first; itr != range.second; itr++) {
            if(itr->second == value) {
                return itr;
            }
        }
        return _cache.left.end();
    }

    bool getLruEntry(const K& key, KeyViewItr& entry) {
        if (_cache.left.find(key) == _cache.left.end()) {
            //Key doesn't exist in cache.
            return false;
        }

        KeyViewItrRange range = _cache.left.equal_range(key);
        entry = range.first;
        for(KeyViewItr itr = range.first; itr != range.second; itr++) {
            //search the range of values that map to the same key and get the least recently used
            if(std::distance(_cache.right.begin(), _cache.project_right(itr)) <
               std::distance(_cache.right.begin(), _cache.project_right(entry))) {
                entry = itr;
            }
        }

        return true;
    }

private:
    //synchronization mutex
    std::recursive_mutex _m;

    //maximum capacity of cache
    unsigned int _capacity;

    //map container representing cache
    CacheType _cache;
};

}}} // namespace ::ezbake::common::lrucache

#endif /* EZBAKE_COMMON_LRUCACHE_LRUCACHE_H_ */

