#ifndef PCBMANAGER_H
#define PCBMANAGER_H

class PCBManager{

    public:
        PCBManager(int maxProcesses);
        ~PCBManager();

        PCB* AllocatePCB();
        int DeallocatePCB(PCB* pcb);

    private:
        Bitmap* bitmap;
        PCB** pcbs;     
}

#endif