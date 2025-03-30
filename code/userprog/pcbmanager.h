#ifndef PCBMANAGER_H
#define PCBMANAGER_H

#include "bitmap.h"
#include "pcb.h"

class PCBManager{

    public:
        PCBManager(int maxProcesses);
        ~PCBManager();

        PCB* AllocatePCB();
        int DeallocatePCB(PCB* pcb);

    private:
        Bitmap* bitmap;
        PCB** pcbs;
        Lock* pcbManagerLock;     
}

#endif