
#ifndef PCB_H
#define PCB_H

#include "list.h"
#include "synch.h"
#include "thread.h"

class Thread;


class PCB{

    public:
        PCB(int pid);
        ~PCB();
        int pid;
        PCB* parent;
        List* children;
        Thread* thread;
    
    private:
        void AddChild(PCB* pcb);
        int RemoveChild(PCB* pcb);
};


#endif