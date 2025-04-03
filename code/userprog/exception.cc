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
#include "system.h"

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
    PCB *pcb = currentThread->space->pcb;
    int pid = pcb->pid;

    printf("System Call: [%d] invoked [Exit]\n", pid);
    printf("Process [%d] exits with [%d]\n", pid, status);

    // Mark process as exited
    pcb->exitStatus = status;

    // Remove from parent-child relationships
    if (pcb->parent != NULL)
    {
        pcb->parent->RemoveChild(pcb);
    }

    // Clean up children: delete exited children, nullify parent in others
    pcb->DeleteExitedChildrenSetParentNull();

    // If it has no parent, free the PCB now
    if (pcb->parent == NULL)
    {
        pcbManager->DeallocatePCB(pcb);
    }

    // Clean up address space (which includes PCB pointer)
    delete currentThread->space;

    // Finish thread
    currentThread->Finish();

    // NOTE: Nothing after this line will execute
}

void incrementPC()
{
    int oldPCReg = machine->ReadRegister(PCReg);

    machine->WriteRegister(PrevPCReg, oldPCReg);
    machine->WriteRegister(PCReg, oldPCReg + 4);
    machine->WriteRegister(NextPCReg, oldPCReg + 8);
}

void childFunction(int pid)
{
    currentThread->RestoreUserState();
    currentThread->space->RestoreState();
    machine->Run(); // Never returns
    ASSERT(FALSE);
}

int doFork(int functionAddr)
{
    // Step 1: Check for sufficient memory
    if (currentThread->space->GetNumPages() > mm->GetFreePageCount())
    {
        return -1; // Not enough memory
    }

    // Step 2: Save parent user register state
    currentThread->SaveUserState();

    // Step 3: Create new address space (copy constructor)
    AddrSpace *childAddrSpace = new AddrSpace(currentThread->space);
    if (!childAddrSpace->valid)
    {
        delete childAddrSpace;
        return -1;
    }

    // Step 4: Create new thread for child
    Thread *childThread = new Thread("childThread");
    childThread->space = childAddrSpace;

    // Step 5: Create and link PCB
    PCB *childPCB = pcbManager->AllocatePCB();
    if (childPCB == NULL)
    {
        delete childThread;
        delete childAddrSpace;
        return -1;
    }

    childPCB->thread = childThread;
    childPCB->parent = currentThread->space->pcb;
    currentThread->space->pcb->AddChild(childPCB);
    childAddrSpace->pcb = childPCB;

    // Step 6: Setup child's registers
    childThread->SaveUserState();        // Saves default 0s; we overwrite key registers next
    childThread->space->InitRegisters(); // sets PC, Stack, etc.
    childThread->space->RestoreState();

    machine->WriteRegister(PCReg, functionAddr);
    machine->WriteRegister(NextPCReg, functionAddr + 4);
    machine->WriteRegister(PrevPCReg, functionAddr - 4);

    childThread->SaveUserState(); // save updated regs for when child starts

    // Step 7: Restore parent state
    currentThread->RestoreUserState();

    // Step 8: Fork the child thread
    childThread->Fork((VoidFunctionPtr)childFunction, childPCB->pid);

    DEBUG('t', "Fork: Created child process with PID %d\n", childPCB->pid);
    return childPCB->pid;
}

int doExec(char *filename)
{
    printf("System Call: [%d] invoked Exec on [%s]\n", currentThread->space->pcb->pid, filename);

    // 1. Open the executable file
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL)
    {
        printf("Exec Error: Unable to open file %s\n", filename);
        return -1;
    }

    // 2. Save the existing PCB before replacing the address space
    PCB *pcb = currentThread->space->pcb;
    delete currentThread->space;

    // 3. Create a new address space
    space = new AddrSpace(executable);

    // 4. Close the executable file
    delete executable;

    // 5. Check if address space creation succeeded
    if (!space->valid)
    {
        printf("Exec Error: Could not create address space for file %s\n", filename);
        return -1;
    }

    // 6. Reuse the existing PCB
    space->pcb = pcb;

    // 7. Set the new address space for the current thread
    currentThread->space = space;

    // 8. Initialize registers for the new program
    space->InitRegisters();

    // 9. Load the page table into the MMU
    space->RestoreState();

    // 10. Begin execution of the new program
    machine->Run();

    // This line should never be reached unless something fails inside Run()
    ASSERT(FALSE);
    return 0;
}

int doJoin(int pid)
{
    // Step 1: Validate that PID exists
    PCB *childPCB = pcbManager->GetPCB(pid);
    if (childPCB == NULL)
    {
        return -1; // Invalid PID
    }

    // Step 2: Make sure the PID belongs to a child of the calling process
    PCB *parentPCB = currentThread->space->pcb;
    if (childPCB->parent != parentPCB)
    {
        return -1; // Not a child
    }

    // Step 3: Wait for child to finish (using busy wait + yield)
    while (!childPCB->HasExited())
    {
        currentThread->Yield();
    }

    // Step 4: Retrieve and return child's exit status
    int status = childPCB->exitStatus;

    // Step 5: Clean up PCB
    pcbManager->DeallocatePCB(childPCB);

    return status;
}

int doKill(int pid)
{
    PCB *victimPCB = pcbManager->GetPCB(pid);

    // Step 1: Validate PID
    if (victimPCB == NULL)
    {
        printf("Kill Error: Invalid PID [%d]\n", pid);
        return -1;
    }

    // Step 2: If the current thread is being killed, just call doExit
    if (victimPCB == currentThread->space->pcb)
    {
        printf("Kill Info: Process [%d] is self; calling doExit(0)\n", pid);
        doExit(0);
        return 0;
    }

    printf("System Call: [%d] invoked Kill on [%d]\n", currentThread->space->pcb->pid, pid);

    // Step 3: Remove from parent's children list if parent exists
    if (victimPCB->parent != NULL)
    {
        victimPCB->parent->RemoveChild(victimPCB);
    }

    // Step 4: Set children's parent to null
    victimPCB->DeleteExitedChildrenSetParentNull();

    // Step 5: Remove the address space and memory
    delete victimPCB->thread->space;

    // Step 6: Remove thread from ready list or mark to be destroyed
    if (victimPCB->thread == currentThread)
    {
        threadToBeDestroyed = currentThread;
    }
    else
    {
        scheduler->RemoveThread(victimPCB->thread);
        delete victimPCB->thread;
    }

<<<<<<< HEAD
    pcbManager->DeallocatePCB(victim);
    mm->FreePages(victim->thread->space);
    delete victim->thread->space;

    // Remove thread from scheduler
    scheduler->Kill(victim->thread); // You'll have to implement this if not present
=======
    // Step 7: Deallocate PCB
    pcbManager->DeallocatePCB(victimPCB);
>>>>>>> 50ea8a8dc39c68f86083c13dd09f2ea52799cd89

    // Step 8: Return success
    return 0;
}

void doYield()
{
    currentThread->Yield();
}

// This implementation (discussed in one of the videos) is broken!
// Try and figure out why.
char *readString1(int virtAddr)
{

    unsigned int pageNumber = virtAddr / 128;
    unsigned int pageOffset = virtAddr % 128;
    unsigned int frameNumber = machine->pageTable[pageNumber].physicalPage;
    unsigned int physicalAddr = frameNumber * 128 + pageOffset;

    char *string = &(machine->mainMemory[physicalAddr]);

    return string;
}

// This implementation is correct!
// perform MMU translation to access physical memory
char *readString(int virtualAddr)
{
    int i = 0;
    char *str = new char[256];
    unsigned int physicalAddr = currentThread->space->Translate(virtualAddr);

    // Need to get one byte at a time since the string may straddle multiple pages that are not guaranteed to be contiguous in the physicalAddr space
    bcopy(&(machine->mainMemory[physicalAddr]), &str[i], 1);
    while (str[i] != '\0' && i != 256 - 1)
    {
        virtualAddr++;
        i++;
        physicalAddr = currentThread->space->Translate(virtualAddr);
        bcopy(&(machine->mainMemory[physicalAddr]), &str[i], 1);
    }
    if (i == 256 - 1 && str[i] != '\0')
    {
        str[i] = '\0';
    }

    return str;
}

void doCreate(char *fileName)
{
    printf("Syscall Call: [%d] invoked Create.\n", currentThread->space->pcb->pid);
    fileSystem->Create(fileName, 0);
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
        // Implement Exit system call
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
        char *fileName = readString(virtAddr);
        int ret = doExec(fileName);
        machine->WriteRegister(2, ret);
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Join))
    {
        int ret = doJoin(machine->ReadRegister(4));
        machine->WriteRegister(2, ret);
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Kill))
    {
        int ret = doKill(machine->ReadRegister(4));
        machine->WriteRegister(2, ret);
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Yield))
    {
        doYield();
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Create))
    {
        int virtAddr = machine->ReadRegister(4);
        char *fileName = readString(virtAddr);
        doCreate(fileName);
        incrementPC();
    }
    else
    {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }
}
