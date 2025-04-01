// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "machine.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void doExit(int status)
{
    int pid = currentThread->space->pcb->pid;

    printf("System Call: [%d] invoked [Exit]\n", pid);
    printf("Process [%d] exits with status [%d]\n", pid, status);

    delete currentThread->space;     // Clean up address space
    currentThread->Finish();         // Finish the current thread (will not return)
}

void incrementPC()
{
    int oldPCReg = machine->ReadRegister(PCReg);

    machine->WriteRegister(PrevPCReg, oldPCReg);
    machine->WriteRegister(PCReg, oldPCReg + 4);
    machine->WriteRegister(NextPCReg, oldPCReg + 8);
}

// Restore the state of registers
// currentThread -> RestoreUserState()
// Restore the page table for the child
// currentThread -> space -> RestoreState()
// PCReg == ReadRegister(PCReg)
// print message for chid creation(pid, PcReg,
// currentThread -> space -> GetNumPages())
// machine -> Run()

void childFunction(int pid)
{
    // Get the PCB of the child process
    PCB *childPCB = pcbManager->pcbs[pid];
    Thread *childThread = childPCB->thread;

    // Set the currentThread to the child (precautionary, often already correct)
    currentThread = childThread;

    // Restore the user CPU registers and page table state
    currentThread->RestoreUserState();    // Restore the saved user registers
    currentThread->space->RestoreState(); // Restore the page table for this process

    // Debug message
    printf("Child process [%d] starting at PC=%d with %d pages\n",
           pid, machine->ReadRegister(PCReg), currentThread->space->GetNumPages());

    // Run the machine — this does not return
    machine->Run();

    // If machine->Run returns, it means something went wrong
    ASSERT(FALSE);
}

// 1.Check if sufficirnt memory exist to create a new proces
// currentThread -> space -> GetNumPages() <= mm -> GetFreePageCount()
// if check fails, return -1
// 2. SaveUserState for the parent
// currentThread-> SaveUserState()
// 3. Create a new address space for child by copyng the parent address space
// Parent: currentThread -> space
// Child: new AddrSpace(currentThread -> space)
// 4. Create a new thread for the child and set its addrSpace
// childThread = new Thread("childThread")
// child -> space = childAddSpace;
// 5. Create a PCB for the child and connect it all up
// pcb: pcbManager -> AllocatePCB();
// pcb -> thread = childThread
// set parent for child pcb
// add child for parent
// 6. Set up machine registers for child and save it to child trhead
// PCReg: functionAddr
// PrevPCReg: functionAddr - 4
// NextPCReg: functionAddr + 4
// childThread -> SaveUserState()
// 7. Call thread -> fork on child
// childThread -> Fork(childFunction, pcb -> pid)
// 8. Restore registe state of parent user-level process
// currentThread -> RestoreUserState()
// 9. return pcb -> pid

int doFork(int functionAddr)
{
    // 1. Check if sufficient memory exists
    if (currentThread->space->GetNumPages() > mm->GetFreePageCount())
    {
        printf("doFork: Not enough memory to fork a new process.\n");
        return -1;
    }

    // 2. Save parent user state
    currentThread->SaveUserState();

    // 3. Create a new address space by copying the parent address space
    AddrSpace *childSpace = new AddrSpace(currentThread->space);
    if (!childSpace->valid)
    {
        printf("doFork: Failed to create child address space.\n");
        delete childSpace;
        return -1;
    }

    // 4. Create new thread and set address space
    Thread *childThread = new Thread("child thread");
    childThread->space = childSpace;

    // 5. Allocate PCB and connect relationships
    PCB *childPCB = pcbManager->AllocatePCB();
    if (!childPCB)
    {
        printf("doFork: Failed to allocate PCB for child.\n");
        delete childSpace;
        delete childThread;
        return -1;
    }

    childPCB->thread = childThread;
    childPCB->parent = currentThread->space->pcb;
    currentThread->space->pcb->children->Append((void *)childPCB);
    childSpace->pcb = childPCB;

    // 6. Set up register state for child and save it
    childThread->SaveUserState();
    childThread->setUserRegister(PCReg, functionAddr);
    childThread->setUserRegister(PrevPCReg, functionAddr - 4);
    childThread->setUserRegister(NextPCReg, functionAddr + 4);

    // 7. Fork the child
    childThread->Fork((VoidFunctionPtr)childFunction, (int)childPCB->pid);

    // 8. Restore parent’s register state
    currentThread->RestoreUserState();

    // 9. Return the PID of the child
    return childPCB->pid;
}

    // Use progtest.cc: StartProcess() as a guide
    // Open file and check validity

    // OpenFile *executable = fileSystem->Open(filename);
    // AddrSpace *space;

    // if (executable == NULL) {
    // printf("Unable to open file %s\n", filename);
    // return -1 ;
    // }

    // Create new address space
    // space = new AddrSpace(executable)
    // Check if AddrSpace creation was succesful
    // if(space -> valid != true)
    // printf("Could not create an address space \n")
    // return -1

    // Create a new PCB for the new address space
    //  Can you reuse existing pcb?
    // PCB* pcb = pcbManager -> AllocatePCB()
    //  Initialize parent
    //  pcb -> parent = currentThread
    // space -> pcb = pcb
    //  Set the Thread for the new pcb
    //  pcb -> thread = currentThread
    //  Delete current address space
    //  delete currentThread -> space -> pcb -> parent
    //  set the address space for currentTthread
    //  currentThread ->space = space

    //  delete executable;

    // Initialize registers for new addrSpace
    // space -> InitRegisters()
    // Runthe machine
    // machine - > run()
    // ASSERT(FALSE)
    // return 0

int doExec(char *filename)
{
    // 1. Open the executable file
    OpenFile *executable = fileSystem->Open(filename);
    if (executable == NULL)
    {
        printf("Unable to open file %s\n", filename);
        return -1;
    }

    // 2. Create a new address space using the executable
    AddrSpace *space = new AddrSpace(executable);
    delete executable;

    if (!space->valid)
    {
        printf("Could not create address space for %s\n", filename);
        delete space;
        return -1;
    }

    // 3. Allocate a new PCB for this process
    PCB *pcb = pcbManager->AllocatePCB();
    if (pcb == NULL)
    {
        printf("No available PCB for %s\n", filename);
        delete space;
        return -1;
    }

    // 4. Set parent relationship
    pcb->parent = currentThread->space->pcb;
    pcb->parent->children->Append((void *)pcb);

    // 5. Create and bind a thread to the new PCB
    Thread *execThread = new Thread("exec_thread");
    pcb->thread = execThread;
    space->pcb = pcb;

    // 6. Assign the new address space to the new thread
    execThread->space = space;

    // 7. Start the process
    execThread->Fork([](int) {
        currentThread->space->InitRegisters();
        currentThread->space->RestoreState();
        machine->Run(); // Does not return
        ASSERT(FALSE);  // Safety catch
    }, 0);

    return pcb->pid;
}


char *translate(int virtAddr)
{
    unsigned int pageNumber = virtAddr / 128;
    unsigned int pageOffset = virtAddr % 128;
    unsigned int frameNumber = machine->pageTable[pageNumber].physicalPage;
    unsigned int physicalAddr = frameNumber * 128 + pageOffset;
    char *fileName = &(machine->mainMemory[physicalAddr]);

    return fileName;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt))
    {
        DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
    }
    else if ((which == SyscallException) && (type == SC_Exit))
    {
        doExit(machine->ReadRegister(4));
    }
    else if ((which == SyscallException) && (type == SC_Fork))
    {
        int ret = doFork(machine->ReadRegister(4));
        machine->WriteRegister(2, ret);
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Exec))
    {
        int virtAddr = machine->ReadRegister(4);
        char *fileName = translate(virtAddr); 
        int ret = doExec(fileName);
        machine->WriteRegister(2, ret);
        incrementPC();
    }
    else
    {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }
}
