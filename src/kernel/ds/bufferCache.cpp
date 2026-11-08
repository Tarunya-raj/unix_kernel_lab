#include "kernel/ds/bufferCache.h"
#include <cassert>
#include <stdlib.h>
#include <cstddef>
#include <cstring>
#include <iostream>
BufferCache::BufferCache()
{
    std::cout << "\n[INITIALZING BUFFER CACHE] \n";
    // Initialize a spin lock for optimization.
    for(std::size_t i=0; i<NHASH ;i++)
    {
        list_init(&(hashQueueHeaders[i].hashNode));
     

    }    
        list_init(&(freeListHeader.freeNode));

    //Putting all buffer in frelist itialyy

    std::cout << "\n[STARTUP ALL BUFFER ARE ADD TO FREE LIST] \n";
    for(std::size_t i=0; i< NBUFFER ; i++)
    {
        Buffer* bptr= &preDefinedBuffers[i];
        list_init(&bptr->freeNode);
        list_init(&bptr->hashNode);
        if(posix_memalign(reinterpret_cast<void**>(&bptr->data), BLOCK_SIZE,BLOCK_SIZE)!=0)
        {
            std::abort();
        }
        bptr->invalid=true;
        //raise processor execution level - TO-DO
        list_add_tail(&(bptr->freeNode),&freeListHeader.freeNode);
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
        if((lockedBuffer=blockInHashQueue(dev,blk))) //buffer in hash queu
        {

            std::cout << "[CACHE HIT] buffer=" << lockedBuffer
                      << " dev=" << lockedBuffer->deviceNo
                      << " block=" << lockedBuffer->blockNo << '\n';
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

            std::cout << "[CACHE MISS] buffer="
                      << " dev=" << dev
                      << " block=" << blk << '\n';

            if(list_empty(&(freeListHeader.freeNode)))
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

            std::cout << "[BUFFER ALLOCATED] buffer="
                      << lockedBuffer
                      << " dev=" << dev
                      << " block=" << blk << '\n';
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

    ListNode* headNode = &freeListHeader.freeNode;
     assert(!list_empty(headNode));
    return container_of(headNode->next,Buffer, freeNode);

}

void BufferCache::addToHashQueue(struct Buffer* lockedBuffer, std::size_t hashID)
{
    //lock operation

    std::cout << "\n[ALLOCATE BUFFER] buffer="
              << " dev=" << lockedBuffer->deviceNo
              << " block=" << lockedBuffer->blockNo << '\n';
    list_add_head(&(lockedBuffer->hashNode),&(hashQueueHeaders[hashID].hashNode));
   
}
void BufferCache::removeBufferFromFreeList(struct Buffer* buf)
{
    //lock;
    //Hardware interrupts must be turned off before modification - TO-Do
    
    list_del(&(buf->freeNode));

    //Hardware interrupts must be turned on after modification
    //lock.unlock();
}

void BufferCache::removeBufferFromHashQueue(struct Buffer* buf)
{
    //lock;
    //Hardware interrupts must be turned off before modification TO-DO
    list_del(&(buf->hashNode));

    //Hardware interrupts must be turned on after modification
    //lock.unlock();
}

struct Buffer* BufferCache::blockInHashQueue(DEVICE dev, BLOCK blk)
{
    struct Buffer& hashqueueHeader= BufferCache::hashQueueHeaders[hashFunction(dev,blk)];
    return findBuffer(hashqueueHeader,dev, blk);
}
Buffer* BufferCache::findBuffer(Buffer& bufferHeader,DEVICE dev,BLOCK blk) const
{
    ListNode* node = bufferHeader.hashNode.next;

    while (node != &bufferHeader.hashNode)
    {
        Buffer* buffer = container_of(node, Buffer, hashNode);

        if (buffer->deviceNo == dev &&
            buffer->blockNo == blk)
        {
            return buffer;
        }

        node = node->next;
    }

    return nullptr;
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

    std::cout << "[BUFFER RELEASE] buffer="
              << buffer
              << " dev=" << buffer->deviceNo
              << " block=" << buffer->blockNo << '\n';

    if(buffer->invalid || buffer->delayedWrite)
    {
        list_add_head(&(buffer->freeNode),&(freeListHeader.freeNode));
    }
    else
    {
        list_add_tail(&(buffer->freeNode),&(freeListHeader.freeNode));
    }

    //lower the processor execution level
    buffer->locked= false;


}

Buffer* BufferCache::bread(DEVICE dev, BLOCK blk)
{
    Buffer* buff= getblk(dev, blk);
    if(buff->invalid)
    {
        std::cout << "[DISK READ] dev=" << dev
                  << " block=" << blk << '\n';
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