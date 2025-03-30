#include "pcb.h"

<<<<<<< HEAD
PCB:: PCB(int id){

    parent = NULL;
    children = new List();
    thread = NULL;
    pid = id;
=======
PCB:: PCB(){

    parent = NULL;
      = new List();
    thread = NULL;
>>>>>>> 9ebf9d60684a6d62655ae7944013a2acf924bb67
}

PCB:: ~PCB(){
    delete children;
}

void PCB:: AddChild(PCB* pcb){
    children -> Append(pcb);
}

<<<<<<< HEAD
int PCB:: RemoveChild(PCB* pcb){
=======
int PCB:: RemoveChiled(PCB* pcb){
>>>>>>> 9ebf9d60684a6d62655ae7944013a2acf924bb67
    return children -> RemoveItem(pcb);
}