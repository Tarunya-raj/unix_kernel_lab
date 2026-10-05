#include "../../../include/kernel/ds/bufferCache.h"
#include <cassert>
#include <stdlib.h>

BufferCache::BufferCache()
{
    // Initialize a spin lock for optimization.
    for(std::size_t i=0; i<NHASH ;i++)
    {
        hashQueueHeaders[i].nextHashNode=&hashQueueHeaders[i];
        hashQueueHeaders[i].prevHashNode=&hashQueueHeaders[i];
        hashQueueHeaders[i].nextFreeNode=nullptr;
        hashQueueHeaders[i].prevFreeNode=nullptr;

    }
    freeListHeader.nextFreeNode=&freeListHeader;
    freeListHeader.prevFreeNode=&freeListHeader;

    //Putting all buffer in frelist itialyy
    for(std::size_t i=0; i< NBUFFER ; i++)
    {
        Buffer* bptr= new Buffer(); //dynamically created at kernel heap.
        if(posix_memalign(reinterpret_cast<void**>(&bptr->data), BLOCK_SIZE,BLOCK_SIZE)!=0)
        {
            std::abort();
        }
        bptr->invalid=true;
        putAtTailOfFreeList(bptr);
    }

}

struct Buffer* BufferCache::getblk(DEVICE dev, BLOCK blk)
{
	while(true)
	{
        struct Buffer* lockedBuffer=nullptr;
        //using result of assignment without using paranthesis
        if((lockedBuffer=blockInHashQueue(dev,blk))) //buffer in hash queu
        {
            if(lockedBuffer->locked)
            {
                //sleep for event that buffer becoes free
                //sleep();
                continue;
            }
            else
            {
                lockedBuffer->locked=true;
                removeBufferFromFreeList(lockedBuffer);
                return lockedBuffer;

             }


        }
        else
        {
            if(freeListHeader.nextFreeNode==&freeListHeader) //freeList empty
            {
                //sleep wait for any buffer
                continue;

            }
            lockedBuffer= getFreeBuffer();
            removeBufferFromFreeList(lockedBuffer);
            if(lockedBuffer->delayedWrite)
            {
                doAsyncWrite(lockedBuffer);
                continue;
            }
            removeBufferFromHashQueue(lockedBuffer); //remove from old hash que
            lockedBuffer->locked=true;
            lockedBuffer->invalid=true;
            lockedBuffer->deviceNo=dev;
            lockedBuffer->blockNo=blk;
            addToHashQueue(lockedBuffer, hashFunction(dev,blk));
            return lockedBuffer;
        }
    }

}
void BufferCache::doAsyncWrite(Buffer* buf)
{
    //TO-DO implement logic
    return;
}
struct Buffer* BufferCache::getFreeBuffer()
{
    assert(!freeListEmpty());
    return freeListHeader.nextFreeNode;

}

void BufferCache::addToHashQueue(struct Buffer* lockedBuffer, std::size_t hashID)
{
    //lock operation
    lockedBuffer->nextHashNode=hashQueueHeaders[hashID].nextHashNode;
    lockedBuffer->prevHashNode=&hashQueueHeaders[hashID];
    (hashQueueHeaders[hashID].nextHashNode)->prevHashNode=lockedBuffer;
    hashQueueHeaders[hashID].nextHashNode= lockedBuffer;

}
void BufferCache::removeBufferFromFreeList(struct Buffer* buf)
{
    //lock;
    (buf->prevFreeNode)->nextFreeNode=buf->nextFreeNode;
    (buf->nextFreeNode)->prevFreeNode= buf->prevFreeNode;
    buf->nextFreeNode=nullptr;
    buf->prevFreeNode= nullptr;
    //lock.unlock();
}

void BufferCache::removeBufferFromHashQueue(struct Buffer* buf)
{
    //lock;
    (buf->prevHashNode)->nextHashNode=buf->nextHashNode;
    (buf->nextHashNode)->prevHashNode= buf->prevHashNode;
    buf->nextHashNode=nullptr;
    buf->prevHashNode= nullptr;
    //lock.unlock();
}

struct Buffer* BufferCache::blockInHashQueue(DEVICE dev, BLOCK blk)
{
    struct Buffer& hashqueueHeader= BufferCache::hashQueueHeaders[hashFunction(dev,blk)];
    return findBuffer(hashqueueHeader,dev, blk);
}
struct Buffer* BufferCache::findBuffer(struct Buffer& bufferHeader, DEVICE dev, BLOCK blk) const
{
    struct Buffer* buffer= bufferHeader.nextHashNode;
    while(buffer != &bufferHeader)

    {
        if(buffer->deviceNo == dev && buffer->blockNo == blk)
            return buffer;
        else
            buffer=buffer->nextHashNode;
    }
    return nullptr;
}

void BufferCache::putAtTailOfFreeList(Buffer* bptr )
{
    bptr->nextFreeNode= &freeListHeader;
    bptr->prevFreeNode= freeListHeader.prevFreeNode;
    (bptr->nextFreeNode)->prevFreeNode=bptr;
    (bptr->prevFreeNode)->nextFreeNode=bptr;
}

bool BufferCache::freeListEmpty() const
{
    return freeListHeader.nextFreeNode == &freeListHeader;
}
BufferCache::~BufferCache()
{
    Buffer* bptr= freeListHeader.nextFreeNode;
    while(bptr != &freeListHeader)
    {
        Buffer* next=bptr->nextFreeNode;
        std::free(bptr->data);
        delete bptr;
        bptr=next;

    }
}
