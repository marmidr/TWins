/******************************************************************************
 * @brief   TWins - hash map template
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins_common.hpp"
#include "twins_vector.hpp"

#include <utility>  // std::move
#include <memory>   // new(addr) T()
#include <type_traits>

// -----------------------------------------------------------------------------

namespace twins
{

/**
 * @brief Simple hashing map for Key-Value pairs
 */
template<typename K, typename V>
class Map
{
public:
    using Hash = uint16_t;

    struct Node
    {
        Hash hash;
        K    key;
        V    val;
    };

    using Bucket = Vector<Node>;

public:
    Map()
    {
    }

    ~Map() = default;

    /** @brief Direct access operator; return existing value, creates new otherwise */
    V& operator[](const K &key)
    {
        if (mBuckets.size() == 0)
        {
            // initial buckets count
            mBuckets.resize(4);
        }

        if (mNodes >= (mBuckets.size() * 8))
            growBuckets();

        auto &node = getNode(key);
        return node.val;
    }

    /** @brief Check if given key is known to the map */
    bool contains(const K &key)
    {
        auto hash = hashAny(key);
        auto bidx = getBucketIdx(hash);
        auto &bkt = mBuckets[bidx];

        for (auto &node : bkt)
            if (node.hash == hash && node.key == key)
                return true;

        return false;
    }

    /** @brief Remove entry */
    void remove(const K &key)
    {
        auto hash = hashAny(key);
        auto &bkt = mBuckets[getBucketIdx(hash)];

        for (unsigned i = 0; i < bkt.size(); i++)
        {
            if (bkt[i].hash == hash && bkt[i].key == key)
            {
                bkt.remove(i);
                mNodes--;
                break;
            }
        }
    }

    /** @brief Return number of key-value pairs */
    unsigned size() const
    {
        return mNodes;
    }

    /** @brief Clear all map entries */
    void clear()
    {
        mBuckets.resize(4);

        for (auto &bkt : mBuckets)
            bkt.clear();

        mNodes = 0;
    }

    /** @brief Number of buckets - for test purposes */
    unsigned bucketsCount() const
    {
        return mBuckets.size();
    }

    /** @brief Return given bucket - for test purposes */
    const Bucket* bucket(int idx) const
    {
        if (idx < 0 || (unsigned)idx > bucketsCount())
            return nullptr;

        return &mBuckets[idx];
    }

    /** @brief Return elements distribution 0 (worse)..100% (best) */
    uint8_t distribution()
    {
        unsigned expected_nodes_per_bkt = mNodes / mBuckets.size();
        unsigned over_expected = 0;

        if (expected_nodes_per_bkt < 2)
            return 100;

        for (const auto &bkt : mBuckets)
        {
            if (bkt.size() > expected_nodes_per_bkt)
                over_expected += bkt.size() - expected_nodes_per_bkt;
        }

        return 100 - (100 * over_expected / mNodes);
    }

private:
    Hash hashBuff(const void *data, unsigned length) const
    {
        const char *p = (const char*)data;
        // modified Dan Bernstein hash function for strings
        unsigned hash = 5381;
        for (unsigned i = 0; i < length; i++)
            hash = ((hash << 5) + hash) ^ *p++; // (hash * 33) ^ *p
        return hash;
    }

    template <typename Key, typename std::enable_if<
        std::is_same<const char*, Key>::value || std::is_same<char*, Key>::value, int>::type = 0>
    Hash hashAny(const Key &key) const
    {
        return hashBuff(key, strlen(key));
    }

    template <typename Key, typename std::enable_if<
        std::is_arithmetic<Key>::value || std::is_enum<Key>::value, int>::type = 0>
    Hash hashAny(const Key &key) const
    {
        return hashBuff(&key, sizeof(Key));
    }

    inline unsigned getBucketIdx(Hash hash) const
    {
        // mBuckets.size() must be power of 2
        return hash & (mBuckets.size() - 1);
    }

    Node& getNode(const K &key)
    {
        auto hash = hashAny(key);
        auto bidx = getBucketIdx(hash);
        auto &bkt = mBuckets[bidx];

        for (auto &node : bkt)
            if (node.hash == hash && node.key == key)
                return node;

        auto &node = mBuckets[bidx].append();
        mNodes++;
        node.hash = hash;
        node.key = key;
        return node;
    }

    void growBuckets()
    {
        Vector<Bucket> old_buckets = std::move(mBuckets);
        mBuckets.resize(old_buckets.size() * 2);

        for (const auto &old_bkt : old_buckets)
        {
            for (auto &old_node : old_bkt)
            {
                auto hash = hashAny(old_node.key);
                auto bidx = getBucketIdx(hash);
                auto &node = mBuckets[bidx].append();
                node.hash = hash;
                node.key = old_node.key;
                node.val = std::move(old_node.val);
            }
        }
    }

private:
    Vector<Bucket> mBuckets;
    uint16_t mNodes = 0;
};

// -----------------------------------------------------------------------------

} // namespace
