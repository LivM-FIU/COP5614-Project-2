#include "pcbmanager.h"

PCBManager::PCBManager(int maxProcesses){
    bitmap = new BitMap(maxProcesses);
    pcbs = new PCB*[maxProcesses];
}

PCBManager::~PCBManager(){
    delete bitmap;
    delete pcbs;
}

PCBManager::AllocatePCB(){

    // Aquiere Lock
    int pid = bitmap.Find();
    // Release Lock

    ASSERT(pid =! -1);
    pcbs[pid] = new PCB(pid);
}

PCBManager::DeallocatePCB(PCB* pcb){
    
    // check if pcn is valid
    // Aquiere Lock
    bitmap.Clear(pcb -> pid);
    // Release Lock
    delete pcbs[pcb -> pid];
}