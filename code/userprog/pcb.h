#include <list.h>
#include <thread.h>
#ifndef PCB_H
#define PCB_H

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