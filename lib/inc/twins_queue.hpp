/******************************************************************************
 * @brief   TWins - queue template
 * @author  Mariusz Midor
 *          https://bitbucket.org/marmidr/twins
 *          https://github.com/marmidr/twins
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
        growAsNecessary();
        mSize++;
        mItems[mWriteIdx] = std::forward<Tv>(item);

        if (++mWriteIdx == mItems.size())
            mWriteIdx = 0;
    }

    /** @brief Returns pointer to to the tail item and decrease items counter, or \b nullptr if queue is empty */
    T* pop(void)
    {
        if (mSize)
        {
            mSize--;
            T *p_ret = &mItems[mReadIdx];

            if (++mReadIdx == mItems.size())
                mReadIdx = 0;
            return p_ret;
        }

        return nullptr;
    }

    /** @brief Remove all items */
    void clear(void)
    {
        mItems.clear();
        mSize = 0;
    }

    /** @brief Return queue size */
    uint16_t size(void) const { return mSize; }

private:
    void growAsNecessary(void)
    {
        if (mSize + 1 >= mItems.size())
        {
            auto new_size = mSize + 1;
            if (new_size < 8)
                new_size = 8;
            else
                new_size = (new_size * 13) / 8;

            mItems.resize(new_size);
        }
    }

    Vector<T> mItems;
    uint16_t  mSize = 0;
    uint16_t  mWriteIdx = 0;
    uint16_t  mReadIdx = 0;
};

// -----------------------------------------------------------------------------

} // namespace
