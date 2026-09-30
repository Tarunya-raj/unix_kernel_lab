#include "buffer.h"
class BufferCache
{
    //struct Buffer hashqueHeaders[NBUFFER];
    struct Buffer hashqueuHeaders[NHASH]; // each header pointing one hash queue;
    struct Buffer freeListHeader;
    inline int hashFunction(DEVICE dev, BLOCK blk)
	{
		return (dev+blk)%NHASH;
	}

    struct Buffer* blockInFreeQueue(DEVICE dev, BLOCK blk);
    struct Buffer* findBuffer(struct Buffer& , DEVICE , BLOCK ) const;
    struct Buffer* blockInHashQueue(DEVICE dev, BLOCK blk);
    struct Buffer* getFreeBuffer();

    void removeBufferFromFreeList(struct Buffer*);
    void removeBufferFromHashQueue(struct Buffer*);
    void addToHashQueue(struct Buffer* lockedBuffer, int hashID);


    public:
        struct Buffer* getblk(DEVICE dev, BLOCK blk);
};
