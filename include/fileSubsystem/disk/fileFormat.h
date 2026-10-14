//Defining data structures for maintaining files on disk
#include <ctime>
#include <cstdint>
#include "common/common.h"
constexpr uint8_t NDIRECT;

using UID=std::uint32_t;
using GID=std::uint32_t;
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
    BLOCK* data[13];
};

struct dinode
{
    //owner identifier - individal, group set of users who have acces right to a file
    FileType        m_fileType = FileType::Deleted;
    uint16_t        m_accessPermission{}; // 9bits to store rwxrwxrwx
    UID             m_uid{};
    GID             m_gid{};
    std::uint32_t    m_nlink{};
    std::uint64_t   m_fileSize{};
    union {
        struct pipe_inode_info       m_fifo;
        struct block_device_info     m_bdev;
        struct char_device_info      m_cdev;
        struct regular_dir_info      m_regDFile;

    };
    std::time_t m_lastModified;
    std::time_t m_lastAccessed;
    std::time_t m_inodeLastModified;

};
