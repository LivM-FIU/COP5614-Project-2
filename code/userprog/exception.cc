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
#include "addrspace.h"
#include "thread.h"
#define MAX_OPEN_FILES 20
#define FileNameMaxLen 128
static OpenFile *openFileTable[20] = {NULL};
static bool openFileUsed[20] = {false};
// #include "synchconsole.h"
// SynchConsole *synchConsole;
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
    // if (pcb->parent != NULL)
    // {
    //     pcb->parent->RemoveChild(pcb);
    // }

    // Clean up children: delete exited children, nullify parent in others
    pcb->DeleteExitedChildrenSetParentNull();

    // If it has no parent, free the PCB now
    // if (pcb->parent == NULL)
    // {
    //     pcbManager->DeallocatePCB(pcb);
    // }

    pcbManager->DeallocatePCB(pcb);

    // Clean up address space (which includes PCB pointer)
    delete currentThread->space;

    // Finish thread
    currentThread->Finish();
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
    static int forkAttemptCount = 0;
    forkAttemptCount++;

    // First print that Fork was invoked
    printf("System Call: [%d] invoked Fork.\n", currentThread->space->pcb->pid);

    // Step 1: Check if enough memory
    if (currentThread->space->GetNumPages() > mm->GetFreePageCount())
    {
        printf("Not Enough Memory for Child Process %d\n", forkAttemptCount);
        return -1;
    }

    // Step 2: Save parent's user register state
    currentThread->SaveUserState();

    // Step 3: Deep copy of the parent address space
    AddrSpace *childAddrSpace = new AddrSpace(currentThread->space);
    if (!childAddrSpace->valid)
    {
        delete childAddrSpace;
        return -1;
    }

    // Step 4: Create child thread
    Thread *childThread = new Thread("childThread");
    childThread->space = childAddrSpace;

    // Step 5: Allocate PCB and link to parent
    PCB *childPCB = pcbManager->AllocatePCB();
    if (childPCB == nullptr)
    {
        delete childThread;
        delete childAddrSpace;
        return -1;
    }

    childPCB->thread = childThread;
    childPCB->parent = currentThread->space->pcb;
    currentThread->space->pcb->AddChild(childPCB);
    childAddrSpace->pcb = childPCB;

    // Step 6: Copy parent's registers into child
    childThread->CopyUserRegistersFrom(currentThread);
    childThread->SetUserRegister(2, 0); // r2 = 0 in child
    childThread->SetUserRegister(PCReg, functionAddr);
    childThread->SetUserRegister(NextPCReg, functionAddr + 4);
    childThread->SetUserRegister(PrevPCReg, functionAddr - 4);

    // Step 7: Print info about the fork
    // Note: Removed the duplicate Fork invocation message that was here
    printf("Process [%d] Fork: start at address [0x%x] with [%d] pages memory\n",
           currentThread->space->pcb->pid, functionAddr, childAddrSpace->GetNumPages());

    // Step 8: Fork the child thread to jump into user mode
    childThread->Fork([](int)
                      {
        currentThread->space->RestoreState();
        currentThread->RestoreUserState();
        machine->Run(); // never returns
        ASSERT(FALSE); }, 0);

    // Step 9: Restore parent's state and return child PID
    currentThread->space->RestoreState();
    currentThread->RestoreUserState();
    return childPCB->pid; // r2 = PID in parent
}

int doExec(char *filename)
{
    printf("System Call: [%d] invoked Exec\n", currentThread->space->pcb->pid);

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

    printf("Exec Program: [%d] loading [%s]\n", pcb->pid, filename);

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
    printf("System Call: [%d] invoked Join.\n", currentThread->space->pcb->pid);

    PCB *joinPCB = pcbManager->GetPCB(pid);
    if (joinPCB == NULL)
    {
        return -1;
    }

    PCB *pcb = currentThread->space->pcb;
    if (pcb != joinPCB->parent)
    {
        return -1;
    }

    // Wait until the child has exited.
    while (!joinPCB->HasExited())
    {
        currentThread->Yield();
    }

    // Retrieve child's exit status.
    int status = joinPCB->exitStatus;

    // Now, deallocate the child's PCB.
    pcbManager->DeallocatePCB(joinPCB);

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

    // FIXED FORMATTING TO MATCH EXPECTED OUTPUT
    printf("System Call: [%d] invoked Kill.\n", currentThread->space->pcb->pid);
    printf("Process [%d] killed process [%d]\n",
           currentThread->space->pcb->pid, pid);

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

    // Step 7: Deallocate PCB
    pcbManager->DeallocatePCB(victimPCB);

    // Step 8: Return success
    return 0;
}

void doYield()
{
    DEBUG('t', "System Call: [%d] invoked Yield.\n", currentThread->space->pcb->pid);
    printf("System Call: [%d] invoked Yield.\n", currentThread->space->pcb->pid);
    currentThread->Yield();
}

// Helper function to read a null-terminated string from user space
char* readString(int virtualAddr) {
    char* str = new char[256];
    int i = 0;
    unsigned int physicalAddr = currentThread->space->Translate(virtualAddr);
    bcopy(&(machine->mainMemory[physicalAddr]), &str[i], 1);
    while (str[i] != '\0' && i < 255) {
        virtualAddr++;
        i++;
        physicalAddr = currentThread->space->Translate(virtualAddr);
        bcopy(&(machine->mainMemory[physicalAddr]), &str[i], 1);
    }
    str[255] = '\0';
    return str;
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// doOpen - Open syscall implementation
OpenFileId doOpen(char* fileName) {
    printf("Syscall Call: [%d] invoked Open.\n", currentThread->space->pcb->pid);
    OpenFile *file = fileSystem->Open(fileName);
    if (file == NULL) {
        printf("doOpen: Failed to open file %s\n", fileName);
        return -1;
    }
    for (int i = 2; i < MAX_OPEN_FILES; i++) {
        if (!openFileUsed[i]) {
            openFileTable[i] = file;
            openFileUsed[i] = true;
            return i;
        }
    }
    delete file;
    return -1;
}

// doClose - Close syscall implementation
bool doClose(int fileId) {
    printf("Syscall Call: [%d] invoked Close.\n", currentThread->space->pcb->pid);
    if (fileId < 0 || fileId >= MAX_OPEN_FILES || !openFileUsed[fileId]) {
        return false;
    }
    delete openFileTable[fileId];
    openFileTable[fileId] = nullptr;
    openFileUsed[fileId] = false;
    return true;
}

// doCreate - Create syscall implementation
void doCreate(char* fileName) {
    int pid = currentThread->space->pcb->pid;
    printf("Syscall Call: [%d] invoked Create.\n", pid);
    bool success = fileSystem->Create(fileName, 0);
    machine->WriteRegister(2, success ? 1 : -1);
}


// ------------------------------------------------------------------
// doRead - Read syscall implementation
int doRead(int fileId, char* buffer, int size) {
    printf("Syscall Call: [%d] invoked Read.\n", currentThread->space->pcb->pid);
    if (fileId == ConsoleInput) {
        char line[256];
        if (fgets(line, sizeof(line), stdin) == NULL) return 0;
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        if (len > size) len = size;
        memcpy(buffer, line, len);
        return len;
    }

    if (fileId < 0 || fileId >= MAX_OPEN_FILES || !openFileUsed[fileId]) return -1;
   
    return openFileTable[fileId]->Read(buffer, size);
}

// ------------------------------------------------------------------
// doWrite - Write syscall implementation
int doWrite(int fileId, char* buffer, int size) {
    printf("Syscall Call: [%d] invoked Write.\n", currentThread->space->pcb->pid);
    if (fileId == ConsoleOutput) {
        for (int i = 0; i < size; i++) printf("%c", buffer[i]);
        return size;
    }
    if (fileId < 0 || fileId >= MAX_OPEN_FILES || !openFileUsed[fileId]) return -1;
    return openFileTable[fileId]->Write(buffer, size);
}

void ExceptionHandler(ExceptionType which) {
    int type = machine->ReadRegister(2);

    if (which == SyscallException && type == SC_Halt) {
        DEBUG('a', "Shutdown initiated by user program.\n");
        interrupt->Halt();
    }
    else if (which == SyscallException && type == SC_Create) {
        char* name = readString(machine->ReadRegister(4));
        doCreate(name);
        delete[] name;
        incrementPC();
    }
    else if (which == SyscallException && type == SC_Open) {
        char* name = readString(machine->ReadRegister(4));
        OpenFileId fid = doOpen(name);
        machine->WriteRegister(2, fid);
        delete[] name;
        incrementPC();
    }
    else if (which == SyscallException && type == SC_Read) {
        int fileId = machine->ReadRegister(4);
        int virtAddr = machine->ReadRegister(5);
        int size = machine->ReadRegister(6);
        if (size <= 0 || virtAddr < 0) {
            machine->WriteRegister(2, -1);
        } else {
            char* buffer = new char[size];
            int bytesRead = doRead(fileId, buffer, size);
            for (int i = 0; i < bytesRead; i++) {
                machine->WriteMem(virtAddr + i, 1, buffer[i]);
            }
            delete[] buffer;
            machine->WriteRegister(2, bytesRead);
        }
        incrementPC();
    }
    else if (which == SyscallException && type == SC_Write) {
        int fileId = machine->ReadRegister(4);
        int virtAddr = machine->ReadRegister(5);
        int size = machine->ReadRegister(6);
        char* buffer = new char[size];
        for (int i = 0; i < size; i++) {
            int val;
            machine->ReadMem(virtAddr + i, 1, &val);
            buffer[i] = (char)val;
        }
        int bytesWritten = doWrite(fileId, buffer, size);
        delete[] buffer;
        machine->WriteRegister(2, bytesWritten);
        incrementPC();
    }
    else if (which == SyscallException && type == SC_Close) {
        int fileId = machine->ReadRegister(4);
        bool success = doClose(fileId);
        machine->WriteRegister(2, success ? 0 : -1);
        incrementPC();
    }
    else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }
}