/******************************************************************************
 * @brief   TWins - common definitions
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins_common.hpp"

#include <stdint.h>
#include <assert.h>
#include <string.h>

// -----------------------------------------------------------------------------

namespace twins
{

template <typename T>
class RingBuff
{
public:
    RingBuff() = default;

    /** @brief Initialize with external static buffer of size N */
    template<unsigned N>
    RingBuff(T (&buffer)[N])
    {
        mpBuff = buffer;
        mCapacity = N;
        mStaticBuff = true;
        mHead = mTail = mSize = 0;
    }

    ~RingBuff()
    {
        if (mpBuff && !mStaticBuff)
            pPAL->memFree(mpBuff);
    }

    /** @brief Initialize the internal buffer using twins::IOS memAlloc() */
    void init(uint16_t bufferSize)
    {
        // if static buffer given or already initialized - exit
        if (mpBuff)
            return;

        assert(pPAL);
        mpBuff = (T*)pPAL->memAlloc(bufferSize * sizeof(T));
        mCapacity = bufferSize;
    }

    /** @brief Reset the read/write pointers */
    void clear()
    {
        mSize = 0;
        mHead = mTail = 0;
    }

    /** @brief Returns data length written */
    uint16_t size() const { return mSize; }

    /** @brief Returns buffer capacity */
    uint16_t capacity() const { return mCapacity; }

    /** @brief returns true if buffer is full */
    bool isFull() const { return mSize == mCapacity; }

    /** @brief Write \p data into the buffer
     *  @return false if buffer is full
     */
    bool write(T data)
    {
        assert(mpBuff);
        if (isFull())
            return false;

        mpBuff[mTail++] = data;
        if (mTail == mCapacity)
            mTail = 0;
        mSize++;
        return true;
    }

    /** @brief Write array \p data of \p dataSize elements into the buffer
     *  @return false if no space left for entire data
     */
    bool write(const T *data, uint16_t dataSize)
    {
        assert(mpBuff);
        if (mSize + dataSize > mCapacity)
            return false;
        mSize += dataSize;

        while (dataSize--)
        {
            mpBuff[mTail++] = *data++;
            if (mTail == mCapacity)
                mTail = 0;
        }
        return true;
    }

    /** @brief Write null-terminated C string */
    //template<class = std::enable_if<std::is_same<T, char>::value>>
    bool write(const char* data)
    {
        return write(data, strlen(data));
    }

    /** @brief Returns a pointer to head element or \b nullptr if buffer is empty;
     *         moves read pointer forward
     */
    T* read()
    {
        assert(mpBuff);
        if (mSize == 0)
            return nullptr;

        T *ret = mpBuff[mHead++];
        mSize--;
        if (mHead == mCapacity)
            mHead = 0;
        return ret;
    }

    /** @brief Read \p count elements into the \p buffer, but no more that \p size()
     *  @return number of elements copied
     */
    uint16_t read(T *buffer, uint16_t count)
    {
        assert(mpBuff);
        if (mSize == 0)
            return 0;

        if (count > mSize)
            count = mSize;

        uint16_t n = count;
        mSize -= count;

        while (n--)
        {
            *buffer++ = mpBuff[mHead++];
            if (mHead == mCapacity)
                mHead = 0;
        }

        return count;
    }

    /** @brief Copy \p count elements into the \p buffer, but no more that \p size()
     *         not changing the internal state
     *  @return number of elements copied
     */
    uint16_t copy(T *buffer, uint16_t count) const
    {
        assert(mpBuff);
        if (mSize == 0)
            return 0;

        if (count > mSize)
            count = mSize;

        uint16_t n = count;
        uint16_t head = mHead;

        while (n--)
        {
            *buffer++ = mpBuff[head++];
            if (head == mCapacity)
                head = 0;
        }

        return count;
    }

    /** @brief Returns a pointer to contiguous data, not changing the read pointer.
     *  @param if \p dataSz is given, set value to contiguous data size
     */
    T* getContiguousData(uint16_t *dataSz = nullptr)
    {
        assert(mpBuff);
        if (mSize == 0)
            return nullptr;

        if (dataSz)
        {
            if (mHead < mTail)
            {
                // ...H...T..
                *dataSz = mSize;
            }
            else
            {
                // ...T...H..
                *dataSz = mCapacity - mHead;
            }
        }

        return &mpBuff[mHead];
    }

    /** @brief Moves read pointer forward by \p count items, but no more than \p size elements */
    void skip(uint16_t count)
    {
        if (count > mSize)
            count = mSize;

        mSize -= count;
        mHead += count;
        if (mHead >= mCapacity)
            mHead -= mCapacity;
    }

protected:
    /** write index */
    uint16_t mTail = 0;
    /** read index */
    uint16_t mHead = 0;
    /** items count */
    uint16_t mSize = 0;
    /** size of mpBuff[] */
    uint16_t mCapacity = 0;
    /**  */
    bool     mStaticBuff = false;
    /** data buffer */
    T *      mpBuff = {};
};

// -----------------------------------------------------------------------------

} // namespace
