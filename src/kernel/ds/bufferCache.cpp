#include "bufferCache.h"
#include <cassert>
#include <stdlib.h>
#include <cstddef>
#include <cstring>
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
        Buffer* bptr= &preDefinedBuffers[i];
        if(posix_memalign(reinterpret_cast<void**>(&bptr->data), BLOCK_SIZE,BLOCK_SIZE)!=0)
        {
            std::abort();
        }
        bptr->invalid=true;
        //raise processor execution level - TO-DO
        putAtTailOfFreeList(bptr);
    }

}

void raiseProcessorExecutionLevel()
{
    //TO-DO
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
                lockedBuffer->indemand=true;
                //sleep();
                continue;
            }
            else
            {
                lockedBuffer->locked=true;
                //raise processor execution level - TO-DO

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
            //raise processor execution level - TO-DO
            removeBufferFromFreeList(lockedBuffer);
            if(lockedBuffer->delayedWrite)
            {
                lockedBuffer->locked=true;
                doAsyncWrite(lockedBuffer);
                continue;
            }
            //raise processor execution level - TO-DO
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
    bwrite(buf);

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
    //Hardware interrupts must be turned off before modification - TO-Do
    if(buf->nextFreeNode == &(*buf) && buf->prevFreeNode ==&(*buf)) return;
    (buf->prevFreeNode)->nextFreeNode=buf->nextFreeNode;
    (buf->nextFreeNode)->prevFreeNode= buf->prevFreeNode;
    buf->nextFreeNode=nullptr;
    buf->prevFreeNode= nullptr;

    //Hardware interrupts must be turned on after modification
    //lock.unlock();
}

void BufferCache::removeBufferFromHashQueue(struct Buffer* buf)
{
    //lock;
    //Hardware interrupts must be turned off before modification TO-DO
    (buf->prevHashNode)->nextHashNode=buf->nextHashNode;
    (buf->nextHashNode)->prevHashNode= buf->prevHashNode;
    buf->nextHashNode=nullptr;
    buf->prevHashNode= nullptr;

    //Hardware interrupts must be turned on after modification
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


void BufferCache::putAtHeadOfFreeList(Buffer* bptr )
{
    bptr->nextFreeNode= freeListHeader.nextFreeNode;
    bptr->prevFreeNode= &freeListHeader;
    (bptr->nextFreeNode)->prevFreeNode=bptr;
    (bptr->prevFreeNode)->nextFreeNode=bptr;
}

bool BufferCache::freeListEmpty() const
{
    return freeListHeader.nextFreeNode == &freeListHeader;
}
BufferCache::~BufferCache()
{    
    for(auto & buf : preDefinedBuffers)
    {
        std::free(buf.data);
    }
}

void BufferCache::brelse(Buffer* buffer)
{
    //TO-DO
    // Wakeup all process, event waiting for any buffer to become free.
    // wake up all process, event waiting for that buffer to become free.

    //raise processor execution level
    // Block interrrupts  disableInterupts();
    //acquire(bufferCacheLock);

    if(buffer->invalid || buffer->delayedWrite)
    {
        putAtHeadOfFreeList(buffer);
    }
    else
    {
        putAtTailOfFreeList(buffer);
    }

    //lower the processor execution level
    buffer->locked= false;


}

Buffer* BufferCache::bread(DEVICE dev, BLOCK blk)
{
    Buffer* buff= getblk(dev, blk);
    if(buff->invalid)
    {
        //TO-DO
        //initiateDiskRead(buff); //Low level mechanism to initiate communicaiton with device driver- chapert 120
        //sleep();
        std::memset(buff->data, 0, BLOCK_SIZE ); buff->invalid=false;  //DUMMY disk read.
        assert(!buff->invalid);

    }
    return buff;
}

Buffer* BufferCache::breada(DEVICE dev, BLOCK currentBlock, BLOCK nextBlock)
{
    Buffer* currentBuffer=nullptr;
    bool firstCached= blockInHashQueue(dev, currentBlock) != nullptr;
    if(!firstCached) // Buffer not in cache
    {
        currentBuffer= getblk(dev, currentBlock);
        if(currentBuffer->invalid)
        {
            //initiateDiskRead(currentBuffer); - TO-DO
            //synchronous

        }

    }
    if(blockInHashQueue(dev, nextBlock) == nullptr) //next block in hashque
    {
        Buffer* nextBuffer= getblk(dev, nextBlock);
        if(nextBuffer->invalid)
        {
            //buffer->async_read= true;
            //How will interrupt decide if this buffer is asyn read?
            nextBuffer->write=false;
            nextBuffer->asyncRead=true;
            //initiateDiskRead(buffer);
        }
        else
        {
            brelse(nextBuffer);
        }
    }
    if(firstCached)
    {
        currentBuffer= bread(dev, currentBlock);
        return currentBuffer;
    }
    if(currentBuffer->invalid)
    {
        //sleep(event first buffer conatins valid data);
    }
    return currentBuffer;


}

void BufferCache::bwrite(Buffer* buf)
{
    //initiateDiskWrite(buf);
    if(buf->write && !buf->delayedWrite) //synchronous write
    {
        //sleep(i/o completion);
        brelse(buf);
    }
    else if (buf->delayedWrite)//asyncwrite)
    {
        //mark buffer to put at head of free list?

    }



}