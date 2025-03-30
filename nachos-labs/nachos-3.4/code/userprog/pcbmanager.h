#ifndef PCBMANAGER_H
#define PCBMANAGER_H

#include "bitmap.h"
#include "pcb.h"

class PCB;
class Lock;
class PCBManager {

    public:
        PCBManager(int maxProcesses);
        ~PCBManager();

        PCB* AllocatePCB();
        int DeallocatePCB(PCB* pcb);
        PCB* GetPCB(int pid);

    // private:
    //     BitMap* bitmap;
    //     PCB** pcbs;
    //     // Need a lock here
    //     // Lock* pcbManagerLock;
    private:
    BitMap* bitmap;
    PCB** pcbs;
    Lock* pcbManagerLock;  // Uncomment and use this

};

#endif // PCBMANAGER_H