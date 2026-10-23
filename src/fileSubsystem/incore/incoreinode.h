#ifndef INCOREINODE_H
#define INCOREINODE_H
#include "common/common.h"
#include <cstdint>
constexpr int NHASH =4;
constexpr int NINODE= 10; // a llowing on 10 incoreinodes to be held open.
using INODENO= uint32_t;
struct incoreInode
{
    //sleep lock
    DEVICE              m_devNo=0;
    INODENO             m_inodeNo=0;
    incoreInode*        m_nextHashQueue=nullptr;
    incoreInode*        m_prevHashQueue= nullptr;
    incoreInode*        m_nextFreeList= nullptr;
    incoreInode*        m_prevFreeList = nullptr;
    uint32_t            m_refrenceCount=0;
    uint16_t            m_mode{};
    UID                 m_uid{};
    GID                 m_gid{};
    std::uint16_t       m_nlinks{};
    std::uint64_t       m_fileSize{}; //32 bits can cover only 4GB file size
    //13 member array
    blockNumber         m_blockAddress[NBLOCKADDR];
    std::int64_t        m_lastModified;
    std::int64_t        m_lastAccessed;
    std::int64_t        m_inodeLastModified;
    //status
    bool                locked=false;
    bool                indemand=false;
    bool                inodeModified=false;
    bool                dataModified=false;
    bool                mountFile=false;





};
/* like buffer cache iget working
 * 1. device no+ inode number hashing - segregate in queus
 * 2. find inode number in cache
 * 3. if inode found check if locked - wait for inode free
 * 4. if inode found in cache and unlocked - mark it lock increse the refrence count return locked inode
 * 5. if inode not found search free list - lock inode, remove from free list, add in hash queu return locked inode.
 * 6. if inode not found on cache search free list, if free list empty return error.
*/
class incoreInodeTable
{

public:
    incoreInodeTable();

    incoreInode iget(DEVICE dev, INODENO inodeNo);
private:
    inline uint32_t hashFunction(DEVICE dev, INODENO inodeNo)
    {
        return (dev+inodeNo)%NHASH;
    }

    void init();

private:
    //sleep lock
    incoreInode predefinedInodes[NINODE]; //10 inodes
    incoreInode inodeHashQueus[NHASH];
    incoreInode freeList;

};

#endif // INCOREINODE_H
