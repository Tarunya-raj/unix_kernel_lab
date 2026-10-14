#include <iostream>
#include "kernel/ds/bufferCache.h"
using namespace std;

int main()
{
    cout << "UNIX DESIGN- BUFFER CACHE WORKING\n";

    BufferCache cache;

    std::cout << "\n--- SCENARIO 1: READ BLOCK 10 ---\n";
    Buffer* b1 = cache.bread(1, 10);
    std::cout << "buffer = " << b1 << '\n';
    cache.brelse(b1);


    std::cout << "\n--- SCENARIO 2: READ BLOCK 10 AGAIN ---\n";
    Buffer* b2 = cache.bread(1, 10);
    std::cout << "buffer = " << b2 << '\n';
    cache.brelse(b2);


    std::cout << "\n--- SCENARIO 3: READ BLOCK 20 ---\n";
    Buffer* b3 = cache.bread(1, 20);
    std::cout << "buffer = " << b3 << '\n';
    cache.brelse(b3);


    std::cout << "\n--- SCENARIO 4: READ BLOCK 20 AGAIN ---\n";
    Buffer* b4 = cache.bread(1, 20);
    std::cout << "buffer = " << b4 << '\n';
    cache.brelse(b4);


    std::cout << "\n--- SCENARIO 5: READ BLOCK 10 AGAIN ---\n";
    Buffer* b5 = cache.bread(1, 10);
    std::cout << "buffer = " << b5 << '\n';
    cache.brelse(b5);
    return 0;
}
