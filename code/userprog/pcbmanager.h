#ifndef PCBMANAGER_H
#define PCBMANAGER_H

#include "bitmap.h"
#include "synch.h"    // needed for Lock
// #include "pcb.h"
class PCB;            // forward declaration
class Lock;

class PCBManager{

    public:
        PCBManager(int maxProcesses);
        ~PCBManager();

        PCB* AllocatePCB();
        PCB* GetPCB(int pid);
        int DeallocatePCB(PCB* pcb);

    private:
        BitMap* bitmap;
        PCB** pcbs;
        Lock* pcbManagerLock;     
};

#endif