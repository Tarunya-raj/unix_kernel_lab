#define BLOCK_SIZE 4096
#define NBUFFER 500
#define NHASH 10
#define BUFFER_SIZE BLOCK_SIZE
#define DEVICE uint
#define BLOCK uint
//#include <cstdint>
#include <sys/types.h>
struct Buffer
{
	struct Buffer* nextHashNode;
	struct Buffer* prevHashNode;
	struct Buffer* nextFreeNode;
	struct Buffer* prevFreeNode;
    uint deviceNo;
	uint blockNo;
	bool locked;
	bool invalid;
	bool indemand;
	bool delayedWrite;
	bool write;
    char  *data;
    Buffer():
        nextHashNode(nullptr),
        prevHashNode(nullptr),
        nextFreeNode(nullptr),
        prevFreeNode(nullptr),
        deviceNo(0),
        blockNo(0),
        locked(false),
        invalid(true),
        indemand(false),
        delayedWrite(false),
        write(false){}
};
