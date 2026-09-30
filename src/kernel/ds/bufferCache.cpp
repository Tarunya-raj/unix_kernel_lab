#include "../../../include/kernel/ds/bufferCache.h"
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
                //asynchronous write
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

struct Buffer* BufferCache::getFreeBuffer()
{
    return freeListHeader.nextFreeNode;

}

void BufferCache::addToHashQueue(struct Buffer* lockedBuffer, int hashID)
{
    //lock operation
    lockedBuffer->nextHashNode=hashqueuHeaders[hashID].nextHashNode;
    lockedBuffer->prevHashNode=&hashqueuHeaders[hashID];
    (hashqueuHeaders[hashID].nextHashNode)->prevHashNode=lockedBuffer;
    hashqueuHeaders[hashID].nextHashNode= lockedBuffer;

}
void BufferCache::removeBufferFromFreeList(struct Buffer* buf)
{
    //std::lock_guard<std::mutex> lock;
    (buf->prevFreeNode)->nextFreeNode=buf->nextFreeNode;
    (buf->nextFreeNode)->prevFreeNode= buf->prevFreeNode;
    buf->nextFreeNode=nullptr;
    buf->prevFreeNode= nullptr;
    //lock.unlock();
}

void BufferCache::removeBufferFromHashQueue(struct Buffer* buf)
{
    //std::lock_guard<std::mutex> lock;
    (buf->prevHashNode)->nextHashNode=buf->nextHashNode;
    (buf->nextHashNode)->prevHashNode= buf->prevHashNode;
    buf->nextHashNode=nullptr;
    buf->prevHashNode= nullptr;
    //lock.unlock();
}

struct Buffer* BufferCache::blockInHashQueue(DEVICE dev, BLOCK blk)
{
    struct Buffer& hashqueueHeader= BufferCache::hashqueuHeaders[hashFunction(dev,blk)];
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

