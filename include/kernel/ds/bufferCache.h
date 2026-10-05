/*
 * Buffer cache task - 1. synchronize access to disk block, only one copy of block is in kernel memory.
 *                     2. only one kernel thread uses it at a time.
 *                     3. cache LRU algo.
 *                     4. statically allocated buffer pool.
 *                     5. buffer cache -Fast look up through cache- spinn lock- buffer cache process should never go to sleep
 *                     //Later
 *                     6. Buffer -read write from disk slow operations- sleep lock
 *
 * */

#ifndef BUFFER_CACHE_H
#define BUFFER_CACHE_H

#include "buffer.h"
#include <mutex>
#include <condition_variable>
class BufferCache
{
    struct Buffer preDefinedBuffers[NBUFFER];
    //TO-DO static allocation of buffers. kernel heap is limited
    struct Buffer hashQueueHeaders[NHASH]; // each header pointing one hash queue;
    struct Buffer freeListHeader;
    std::condition_variable cacheCV; //protects linked list/hash queue
    std::mutex cacheMtx; //locking maintained during hash queu searching (ideally spin lock)

    inline size_t hashFunction(DEVICE dev, BLOCK blk)
	{
		return (dev+blk)%NHASH;
	}

    struct Buffer* findBuffer(struct Buffer& , DEVICE , BLOCK ) const;
    struct Buffer* blockInHashQueue(DEVICE dev, BLOCK blk);
    struct Buffer* getFreeBuffer();

    bool freeListEmpty() const;
    void removeBufferFromFreeList(struct Buffer*);
    void removeBufferFromHashQueue(struct Buffer*);
    void addToHashQueue(struct Buffer* lockedBuffer, std::size_t hashID);

    void putAtTailOfFreeList(struct Buffer* freeBuffer);

    void doAsyncWrite(Buffer* writeBuffer);
    public:
        BufferCache();
        ~BufferCache();
        struct Buffer* getblk(DEVICE dev, BLOCK blk);
};

#endif
