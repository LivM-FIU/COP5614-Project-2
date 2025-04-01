
#ifndef PCB_H
#define PCB_H

#include "list.h"

class Thread; // ✅ Forward declaration


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