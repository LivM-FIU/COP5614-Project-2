#include "pcbmanager.h"

PCBManager::PCBManager(int maxCount)
{
    bitmap = new BitMap(maxCount);
    pcbs = new PCB *[maxCount];
    pcbManagerLock = new Lock("PCBManagerLock");
    maxProcesses  = maxCount;

    for (int i = 0; i < maxCount; i++) {
        pcbs[i] = NULL;
    }
}

PCBManager::~PCBManager()
{
    delete bitmap;
    delete[] pcbs;
    delete pcbManagerLock;
}

PCB* PCBManager::AllocatePCB()
{
    pcbManagerLock->Acquire();  

    int pid = bitmap->Find();

    if (pid == -1) {
        pcbManagerLock->Release();  
        return NULL;
    }

    pcbs[pid] = new PCB(pid);

    pcbManagerLock->Release();  

    return pcbs[pid];
}

int PCBManager::DeallocatePCB(PCB *pcb)
{
    if (pcb == NULL) return -1;

    int pid = pcb->pid;

    // Validate PCB
    if (pid < 0 || pid >= maxCount) return -1;

    pcbManagerLock->Acquire();  

    if (pcbs[pid] == NULL) {
        pcbManagerLock->Release();
        return -1;
    }

    bitmap->Clear(pid);
    delete pcbs[pid];
    pcbs[pid] = NULL;

    pcbManagerLock->Release(); 

    return 0;
}

PCB* PCBManager::GetPCB(int pid)
{
    if (pid < 0 || pid >= maxCount) return NULL;

    pcbManagerLock->Acquire();
    PCB* pcb = pcbs[pid];
    pcbManagerLock->Release();

    return pcb;
}
