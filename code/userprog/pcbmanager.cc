#include "pcbmanager.h"
#include "system.h"  // For ASSERT

PCBManager::PCBManager(int maxProcesses) {
    bitmap = new BitMap(maxProcesses);
    pcbs = new PCB*[maxProcesses];
    pcbManagerLock = new Lock("PCBManagerLock");  // ✅ Initialize lock
}

PCBManager::~PCBManager() {
    delete bitmap;
    delete[] pcbs;  // ✅ Use delete[] for arrays
    delete pcbManagerLock;
}

PCB* PCBManager::AllocatePCB() {
    pcbManagerLock->Acquire();

    int pid = bitmap->Find();
    if (pid == -1) {
        pcbManagerLock->Release();
        return nullptr;  // No available slots
    }

    pcbs[pid] = new PCB(pid);
    pcbManagerLock->Release();
    return pcbs[pid];
}

int PCBManager::DeallocatePCB(PCB* pcb) {
    if (pcb == nullptr || pcb->pid < 0) return -1;

    pcbManagerLock->Acquire();
    bitmap->Clear(pcb->pid);
    delete pcbs[pcb->pid];
    pcbs[pcb->pid] = nullptr;  // Optional: Nullify pointer
    pcbManagerLock->Release();
    return 0;
}

PCB* PCBManager::GetPCBByPID(int pid) {
    if (pid < 0 || pid >= maxProcesses) 
        return nullptr;
    return pcbs[pid];
}
