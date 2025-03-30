// #include "pcbmanager.h"


// PCBManager::PCBManager(int maxProcesses) {

//     bitmap = new BitMap(maxProcesses);
//     pcbs = new PCB*[maxProcesses];

//     for(int i = 0; i < maxProcesses; i++) {
//         pcbs[i] = NULL;
//     }

// }


// PCBManager::~PCBManager() {

//     delete bitmap;

//     delete pcbs;

// }


// PCB* PCBManager::AllocatePCB() {

//     // Aquire pcbManagerLock

//     int pid = bitmap->Find();

//     // Release pcbManagerLock

//     ASSERT(pid != -1);

//     pcbs[pid] = new PCB(pid);

//     return pcbs[pid];

// }


// int PCBManager::DeallocatePCB(PCB* pcb) {

//     // Check is pcb is valid -- check pcbs for pcb->pid

//      // Aquire pcbManagerLock

//     bitmap->Clear(pcb->pid);

//     // Release pcbManagerLock

//     int pid = pcb->pid;

//     delete pcbs[pid];

//     pcbs[pid] = NULL;

//     return 0;

// }

// PCB* PCBManager::GetPCB(int pid) {
//     return pcbs[pid];
// }
#include "pcbmanager.h"
#include "synch.h"  // Include it here in the .cc file

PCBManager::PCBManager(int maxProcesses) {
    bitmap = new BitMap(maxProcesses);
    pcbs = new PCB*[maxProcesses];
    pcbManagerLock = new Lock("PCBManagerLock");  // Create lock

    for(int i = 0; i < maxProcesses; i++) {
        pcbs[i] = NULL;
    }
}

PCBManager::~PCBManager() {
    delete bitmap;
    delete [] pcbs;  // Corrected to delete[] for array
    delete pcbManagerLock;  // Delete lock
}

PCB* PCBManager::AllocatePCB() {
    pcbManagerLock->Acquire();  // Acquire lock

    int pid = bitmap->Find();
    
    if (pid == -1) {
        pcbManagerLock->Release();
        return NULL;
    }
    
    // Add 1 to PID to start from 1 instead of 0
    pcbs[pid] = new PCB(pid + 1);
    
    pcbManagerLock->Release();  // Release lock
    return pcbs[pid];
}

int PCBManager::DeallocatePCB(PCB* pcb) {
    pcbManagerLock->Acquire();  // Acquire lock

    // Adjust for 1-based PIDs, subtract 1 to get bitmap position
    int bitmapPos = pcb->pid - 1;
    bitmap->Clear(bitmapPos);

    int pid = pcb->pid;
    
    // Use bitmap position (pid-1) for array index
    delete pcbs[bitmapPos];
    pcbs[bitmapPos] = NULL;
    
    pcbManagerLock->Release();  // Release lock
    return 0;
}

PCB* PCBManager::GetPCB(int pid) {
    pcbManagerLock->Acquire();  // Acquire lock
    
    // Check if pid is valid - just make sure it's positive
    if (pid <= 0) {
        pcbManagerLock->Release();
        return NULL;
    }
    
    // Adjust for 1-based PIDs, use pid-1 as array index
    int index = pid - 1;
    
    // Check if the index is within bounds and the bitmap bit is set
    if (index < 0 || !bitmap->Test(index)) {
        pcbManagerLock->Release();
        return NULL;
    }
    
    PCB* result = pcbs[index];
    
    pcbManagerLock->Release();  // Release lock
    return result;
}