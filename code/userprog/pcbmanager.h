#ifndef PCBMANAGER_H
#define PCBMANAGER_H

<<<<<<< HEAD
#include "bitmap.h"
#include "pcb.h"

=======
>>>>>>> 9ebf9d60684a6d62655ae7944013a2acf924bb67
class PCBManager{

    public:
        PCBManager(int maxProcesses);
        ~PCBManager();

        PCB* AllocatePCB();
        int DeallocatePCB(PCB* pcb);

    private:
        Bitmap* bitmap;
<<<<<<< HEAD
        PCB** pcbs;
        Lock* pcbManagerLock;     
=======
        PCB** pcbs;     
>>>>>>> 9ebf9d60684a6d62655ae7944013a2acf924bb67
}

#endif