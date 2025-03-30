#ifndef PCB_H
#define PCB_H

class PCB{

    public:
<<<<<<< HEAD
        PCB(int pid);
=======
        PCB();
>>>>>>> 9ebf9d60684a6d62655ae7944013a2acf924bb67
        ~PCB();
        int pid;
        PCB* parent;
        List* children;
        Thread* thread;
    
    private:
<<<<<<< HEAD
        void AddChild(PCB* pcb);
=======
        void AddChiled(PCB* pcb);
>>>>>>> 9ebf9d60684a6d62655ae7944013a2acf924bb67
        int RemoveChild(PCB* pcb);
}


#endif