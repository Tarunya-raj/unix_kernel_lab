#ifndef BUFFER_H
#define BUFFER_H

#include <cstdint>

using DEVICE = std::uint32_t;
using BLOCK = std::uint32_t;

constexpr std::size_t BLOCK_SIZE = 4096;
constexpr std::size_t NBUFFER = 500;
constexpr std::size_t NHASH= 10;
constexpr std::size_t BUFFER_SIZE= BLOCK_SIZE;


struct Buffer
{
    struct Buffer* nextHashNode=nullptr;
    struct Buffer* prevHashNode=nullptr;
    struct Buffer* nextFreeNode=nullptr;
    struct Buffer* prevFreeNode=nullptr;
    DEVICE deviceNo     = 0;
    BLOCK blockNo       = 0;
    bool locked         = false;
    bool invalid        = true;
    bool indemand       = false;
    bool delayedWrite   = false;
    bool write          = false;
    char  *data         = nullptr;

    /* TO-DO Implement synchonizaion - chapter 6 sleep and wakeup conditions
    std::mutex bufferMtx; //Protects individual mutex
    std::condition_variable bufferCV;  // sleep lock - buffer read write from disk can happen
    */
};
#endif
