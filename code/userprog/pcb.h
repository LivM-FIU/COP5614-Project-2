#ifndef PCB_H
#define PCB_H

class PCB{

    public:
        PCB();
        ~PCB();
        int pid;
        PCB* parent;
        List* children;
        Thread* thread;
    
    private:
        void AddChiled(PCB* pcb);
        int RemoveChild(PCB* pcb);
}


#endif