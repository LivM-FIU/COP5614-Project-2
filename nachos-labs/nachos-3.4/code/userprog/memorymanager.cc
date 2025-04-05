// memorymanager.cc

#include "memorymanager.h"
#include "machine.h"

MemoryManager::MemoryManager() {
    bitmap = new BitMap(NumPhysPages);
}

MemoryManager::~MemoryManager() {
    delete bitmap;
}

int MemoryManager::AllocatePage() {
    return bitmap->Find();
}

int MemoryManager::FreePage(int which) {  // ← اینجا هم همین
    if (bitmap->Test(which) == false) return -1;
    bitmap->Clear(which);
    return 0;
}

unsigned int MemoryManager::GetFreePageCount() {
    return bitmap->NumClear();
}
