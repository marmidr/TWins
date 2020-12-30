/******************************************************************************
 * @brief   TWins - queue template
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins_common.hpp"
#include "twins_vector.hpp"

// -----------------------------------------------------------------------------

namespace twins
{

/**
 * @brief Template for simple self-managed Queue class able to hold any type of value
 */
template<typename T>
class Queue
{
public:
    /** @brief Write new item to the queue */
    template <typename Tv>
    void push(Tv && item)
    {
        reserve();
        mSize++;
        mItems[mWriteIdx] = std::forward<Tv>(item);

        if (++mWriteIdx == mItems.capacity())
            mWriteIdx = 0;
    }

    /** @brief Returns pointer to to the tail item and decrease items counter, or \b nullptr if queue is empty */
    T* pop()
    {
        if (mSize)
        {
            mSize--;
            T *p_ret = &mItems[mReadIdx];

            if (++mReadIdx == mItems.capacity())
                mReadIdx = 0;
            return p_ret;
        }

        return nullptr;
    }

    /** @brief Remove all items */
    void clear(bool force = false)
    {
        mItems.clear();
        mSize = 0;
    }

    /** @brief Return queue size */
    uint16_t size() const { return mSize; }

private:
    void reserve()
    {
        if (mItems.size() == mItems.capacity())
            mItems.resize(mItems.capacity() + 8);
    }

private:
    Vector<T> mItems;
    uint16_t  mSize = 0;
    uint16_t  mWriteIdx = 0;
    uint16_t  mReadIdx = 0;
};

// -----------------------------------------------------------------------------

} // namespace
