#include "pcb.h"

PCB:: PCB(){

    parent = NULL;
      = new List();
    thread = NULL;
}

PCB:: ~PCB(){
    delete children;
}

void PCB:: AddChild(PCB* pcb){
    children -> Append(pcb);
}

int PCB:: RemoveChiled(PCB* pcb){
    return children -> RemoveItem(pcb);
}