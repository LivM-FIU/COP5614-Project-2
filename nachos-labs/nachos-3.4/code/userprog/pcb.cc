#include "pcb.h"
#include "list.h"



PCB::PCB(int id) {

    pid = id;
    parent = NULL;
    children = new List();
    thread = NULL;
    exitStatus = -9999;

}



PCB::~PCB() {

    delete children;

}



void PCB::AddChild(PCB* pcb) {

    children->Append(pcb);


}


int PCB::RemoveChild(PCB* pcb){

    return children->RemoveItem(pcb);

}


bool PCB::HasExited() {
    return exitStatus == -9999 ? false : true;
}


void decspn(int arg) {
    PCB* pcb = (PCB*)arg;
    if (pcb->HasExited()) pcbManager->DeallocatePCB(pcb);
    else pcb->parent = NULL;
}


// void decspn(int arg) {
//     PCB* pcb = (PCB*)arg;
//     if (pcb->HasExited()) {
//         // Make sure to release memory pages before deallocating PCB
//         if (pcb->thread && pcb->thread->space) {
//             // Free memory used by this process
//             for (int i = 0; i < pcb->thread->space->GetNumPages(); i++) {
//                 int physPage = pcb->thread->space->GetPhysicalPage(i);
//                 if (physPage != -1) { // Make sure it's a valid page
//                     memoryManager->FreePage(physPage);
//                 }
//             }
//         }
//         pcbManager->DeallocatePCB(pcb);
//     } else {
//         pcb->parent = NULL;
//     }
// }


void PCB::DeleteExitedChildrenSetParentNull() {
    children->Mapcar(decspn);
}

void PCB::SignalParent() {
    // اگر والد هست و منتظر این فرزند هست، می‌تونه یه شرط باشه (قابل گسترشه)
    // ولی فعلاً یه پیام دیباگ ساده می‌زنیم:
    printf("DEBUG: Signaling parent (PID %d) that child (PID %d) has exited\n", 
           parent ? parent->pid : -1, pid);
}


// bool PCB::HasAliveChildren() {
//     bool found = false;
//     int originalCount = 0;

//     // مرحله اول: لیست رو تخلیه کن و چک کن
//     while (!children->IsEmpty()) {
//         PCB* child = (PCB*) children->Remove();
//         if (!child->exited) {
//             found = true;
//         }
//         children->Append((void*)child);
//         originalCount++;
//     }

//     return found;
// }
bool PCB::HasAliveChildren() {
    bool hasAliveChildren = false;
    List tempList;
    
    // Move all children to a temporary list while checking them
    while (!children->IsEmpty()) {
        PCB* child = (PCB*)children->Remove();
        if (!child->HasExited()) { // Use your HasExited method for consistency
            hasAliveChildren = true;
        }
        tempList.Append(child);
    }
    
    // Move all children back to the original list
    while (!tempList.IsEmpty()) {
        PCB* child = (PCB*)tempList.Remove();
        children->Append(child);
    }
    
    return hasAliveChildren;
}



