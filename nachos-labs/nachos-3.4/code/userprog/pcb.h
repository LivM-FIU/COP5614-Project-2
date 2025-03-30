#ifndef PCB_H
#define PCB_H

#include "list.h"
#include "pcbmanager.h"
#include "machine.h"



class Thread;
class PCBManager;
extern PCBManager* pcbManager;

class PCB {

    public:
        PCB(int id);
        ~PCB();
        int pid;
        PCB* parent;
        Thread* thread;
        int exitStatus;
        bool exited; 
        

        int savedRegisters[NumTotalRegs];

        void AddChild(PCB* pcb);
        int RemoveChild(PCB* pcb);
        int forkStartPC;//added here shamim
        bool HasExited();
        void DeleteExitedChildrenSetParentNull();
        void SignalParent();
        bool HasAliveChildren();



    private:
        List* children;

};

#endif // PCB_H