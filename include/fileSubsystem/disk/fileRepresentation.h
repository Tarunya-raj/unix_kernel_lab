//Defining data structures for maintaining files on disk

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <ctime>
#include <cstdint>
#include "common/common.h"



 

struct dInode
{ 
    uint16_t        m_mode{}; // 9bits(LSB) to store rwxrwxrwx + MSB File type
    UID             m_uid{};
    GID             m_gid{};
    std::uint16_t   m_nlinks{};
    std::uint64_t   m_fileSize{};
    blockNumber     m_blockaddress[NBLOCKADDR];
    std::int64_t     m_lastModified;
    std::int64_t     m_lastAccessed;
    std::int64_t     m_inodeLastModified;

};
struct Directory{
    char name[28];
    uint32_t inode_num;
};

struct SuperBlock
{
    //size of file system
    //number of free blocks
    // list of free blocks on file system
    // index of the next free block in the free block list
    // a lsot pf free inode in the file systm
    //the indix of the next freee inode int the free inode list
    //lock fields for the free block and free inode lust
    //a flag indicating suoer block is modified.


};
class X_Filesystem{
    //bootblock
    //super block
    //DILB
    //DBs
};

#endif

/*
 //owner identifier - individal, group set of users who have acces right to a file
    //FileType        m_fileType = FileType::Deleted;
enum class FileType : uint8_t //1byte, 8bits
{
    Deleted     =0,
    Regular     =1,
    Directory   =2,
    Pipe        =3,
    Ch_Device   =4,
    Blk_Device  =5
};
struct pipe_inode_info
{

};
struct block_device_info
{
    uint16_t majorNumber;
    uint16_t minorNumber;


};

struct char_device_info
{

};
struct regular_dir_info
{
    blocknumber     m_blockaddress[NBLOCKADDR];
};


*/