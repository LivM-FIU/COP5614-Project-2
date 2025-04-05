// memorymanager.h

#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "bitmap.h"

class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();

    int AllocatePage();
    int FreePage(int which); // ← اسمش رو اینجا تغییر بده
    unsigned int GetFreePageCount();

private:
    BitMap* bitmap;
};

#endif
