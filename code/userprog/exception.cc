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
#include "pcb.h" //added for Exec -Trinity


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


// void doExit(int status) {

//     // int pid = 99;
//     int pid = currentThread->space->pcb->pid;

//     printf("System Call: [%d] invoked [Exit]\n", pid);
//     printf ("Process [%d] exits with [%d]\n", pid, status);


//     currentThread->space->pcb->exitStatus = status;

//     // Manage PCB memory As a parent process
//     PCB* pcb = currentThread->space->pcb;

//     // Delete exited children and set parent null for non-exited ones
//     pcb->DeleteExitedChildrenSetParentNull();

//     // Manage PCB memory As a child process
//     if(pcb->parent == NULL) pcbManager->DeallocatePCB(pcb);

//     // Delete address space only after use is completed
//     delete currentThread->space;

//     // Finish current thread only after all the cleanup is done
//     // because currentThread marks itself to be destroyed (by a different thread)
//     // and then puts itself to sleep -- thus anything after this statement will not be executed!
//     currentThread->Finish();

// }

// void doExit(int status) {
//     // Retrieve the Process ID (PID) of the current thread
//     int pid = currentThread->space->pcb->pid;

//     // Print debug information about the exit status and the process exiting
//     printf("System Call: [%d] invoked [Exit]\n", pid);
//     printf("Process [%d] exits with [%d]\n", pid, status);

//     // Set the exit status of the current process (the thread's PCB)
//     currentThread->space->pcb->exitStatus = status;

//     // Retrieve the PCB (Process Control Block) of the current thread
//     PCB* pcb = currentThread->space->pcb;

//     // As a parent process, clean up any exited children processes
//     pcb->DeleteExitedChildrenSetParentNull();

//     // If no parent exists, deallocate the PCB (child process doesn't need a parent anymore)
//     if (pcb->parent == NULL) {
//         pcbManager->DeallocatePCB(pcb);  // Deallocate PCB if no parent exists
//     }

//     // Clean up the address space (i.e., deallocate memory and resources used by the current process)
//     delete currentThread->space;

//     // Finish the current thread, ensuring that the thread is marked as completed and cleaned up
//     currentThread->Finish();

//     // Optionally, you can ensure thread cleanup from scheduler (depending on implementation)
//     scheduler->RemoveThread(currentThread);  // Ensure the thread is removed from the scheduler if needed
// }
// ---Changed
// void doExit(int status) {
//     // Retrieve the Process ID (PID) of the current thread from its PCB
//     int pid = currentThread->space->pcb->pid;

//     // Print debug information about the exit
//     printf("System Call: [%d] invoked [Exit]\n", pid);
//     printf("Process [%d] exits with [%d]\n", pid, status);

//     // Set the exit status in the PCB
//     currentThread->space->pcb->exitStatus = status;

//     // Retrieve the PCB for further cleanup
//     PCB* pcb = currentThread->space->pcb;

//     // Clean up exited children and remove parent references from non-exited ones
//     pcb->DeleteExitedChildrenSetParentNull();

//     // If no parent exists, deallocate the PCB
//     if (pcb->parent == NULL) {
//         pcbManager->DeallocatePCB(pcb);
//     }

//     // Delete the address space of the current thread
//     delete currentThread->space;

//     // Finish the current thread; execution should not return from here.
//     currentThread->Finish();

//     // This line is likely unreachable because Finish() never returns.
//     // scheduler->RemoveThread(currentThread);
// }

// void doExit(int status) {
//     // Get the PID

//     int pid = currentThread->space->pcb->pid;
//     printf("DEBUG: Process %d starting Exit with status %d\n", 
//        currentThread->space->pcb->pid, status);
//     // Print debug info in expected format
//     printf("System Call: [%d] invoked Exit.\n", pid);
//     printf("Process [%d] exits with [%d]\n", pid, status);

//     // Set exit status
//     currentThread->space->pcb->exitStatus = status;

//     // Handle parent-child relationships
//     PCB* pcb = currentThread->space->pcb;
//     pcb->DeleteExitedChildrenSetParentNull();

//     // Clean up PCB if no parent
//     if (pcb->parent == NULL) {
//         pcbManager->DeallocatePCB(pcb);
//     }

//     // Delete address space
//     delete currentThread->space;

//     // Finish thread
//     currentThread->Finish();
// }

// void doExit(int status) {
//     // Store PCB pointer before deleting space
//     PCB* pcb = currentThread->space->pcb;
//     int pid = pcb->pid;
    
//     printf("DEBUG: Process %d starting Exit with status %d\n", pid, status);
//     printf("System Call: [%d] invoked Exit.\n", pid);
//     printf("Process [%d] exits with [%d]\n", pid, status);

//     // Set exit status
//     pcb->exitStatus = status;
//     pcb->exited = true;  // Mark as exited

//     // Handle parent-child relationships
//     pcb->DeleteExitedChildrenSetParentNull();

//     // Signal any waiting parent
//     if (pcb->parent != NULL) {
//         // Wake up parent if it's waiting on this process
//         pcb->SignalParent();  // You'll need to implement this method
//     } else {
//         // No parent, clean up immediately
//         pcbManager->DeallocatePCB(pcb);
//     }

//     // Delete address space
//     AddrSpace* space = currentThread->space;
//     currentThread->space = NULL;  // Prevent future accesses to deleted space
//     delete space;

//     // Finish thread
//     currentThread->Finish();
// }
//Final 
// void doExit(int status) {
//     // Print debug and required output
//     int pid = currentThread->space->pcb->pid;
//     printf("DEBUG: Process %d starting Exit with status %d\n", pid, status);
//     printf("System Call: [%d] invoked Exit.\n", pid);
//     printf("Process [%d] exits with [%d]\n", pid, status);

//     // Set exit status
//     PCB* pcb = currentThread->space->pcb;
//     pcb->exitStatus = status;
//     pcb->exited = true;

//     // Signal parent and handle child relationships
//     if (pcb->parent != NULL) {
//         printf("DEBUG: Signaling parent (PID %d) that child (PID %d) has exited\n", 
//                pcb->parent->pid, pid);
        
//         // Simply put parent in ready queue without checking its status
//         // This works because ReadyToRun should have checks for threads already in ready state
//         if (pcb->parent->thread != NULL) {
//             scheduler->ReadyToRun(pcb->parent->thread);
//             printf("DEBUG: Parent thread added to ready queue\n");
//         }
        
//         pcb->SignalParent();
//     }
    
//     // Clean up child relationships
//     pcb->DeleteExitedChildrenSetParentNull();

//     // Clean up address space
//     delete currentThread->space;
//      currentThread->space = NULL;   
//     // Finish the thread (this will not return)
//     currentThread->Finish();
// }

// void doExit(int status) {
//     PCB* pcb = currentThread->space->pcb;
//     int pid = pcb->pid;

//     printf("DEBUG: Process %d starting Exit with status %d\n", pid, status);
//     printf("System Call: [%d] invoked Exit.\n", pid);
//     printf("Process [%d] exits with [%d]\n", pid, status);

//     pcb->exitStatus = status;
//     pcb->exited = true;

//     // Handle exited children
//     pcb->DeleteExitedChildrenSetParentNull();

//     // Handle parent
//     if (pcb->parent != NULL) {
//         printf("DEBUG: Signaling parent (PID %d) that child (PID %d) has exited\n",
//                pcb->parent->pid, pid);
//         pcb->SignalParent();

//         if (pcb->parent->thread != NULL) {
//             scheduler->ReadyToRun(pcb->parent->thread);
//             printf("DEBUG: Parent thread added to ready queue\n");
//         }
//     } else {
//         pcbManager->DeallocatePCB(pcb);
//     }

//     // Free memory
//     delete currentThread->space;
//     currentThread->space = NULL;
//     currentThread->Finish();
//     ASSERT(false); 
// }
// void doExit(int status) {
//     PCB* pcb = currentThread->space->pcb;
//     int pid = pcb->pid;

//     printf("DEBUG: Process %d starting Exit with status %d\n", pid, status);
//     printf("System Call: [%d] invoked Exit.\n", pid);
//     printf("Process [%d] exits with [%d]\n", pid, status);

//     pcb->exitStatus = status;
//     pcb->exited = true;

//     // 🔁 اگر فرزندی هنوز زنده است، منتظر بمان
//     if (pcb->HasAliveChildren()) {
//         printf("DEBUG: Process %d waiting for children to exit...\n", pid);
//         while (pcb->HasAliveChildren()) {
//             currentThread->Yield();
//         }
//     }

//     // حالا که همه بچه‌ها مردن، cleanup کن
//     pcb->DeleteExitedChildrenSetParentNull();

//     if (pcb->parent != NULL) {
//         pcb->SignalParent();
//     } else {
//         pcbManager->DeallocatePCB(pcb);
//     }

//     delete currentThread->space;
//     currentThread->space = NULL;
//     currentThread->Finish();
// }
// void doExit(int status) {
//     PCB* pcb = currentThread->space->pcb;
//     int pid = pcb->pid;

//     printf("DEBUG: Process %d starting Exit with status %d\n", pid, status);
//     printf("System Call: [%d] invoked Exit.\n", pid);
//     printf("Process [%d] exits with [%d]\n", pid, status);

//     pcb->exitStatus = status;
//     pcb->exited = true;

//     // 🟡 اگر بچه‌های زنده داریم، پدر صبر می‌کنه
//     while (pcb->HasAliveChildren()) {
//         printf("DEBUG: PID %d waiting for alive children to exit...\n", pid);
//         currentThread->Yield();
//     }

//     pcb->DeleteExitedChildrenSetParentNull();

//     if (pcb->parent != NULL) {
//         printf("DEBUG: Signaling parent (PID %d) that child (PID %d) has exited\n",
//                pcb->parent->pid, pid);

//         if (pcb->parent->thread != NULL) {
//             scheduler->ReadyToRun(pcb->parent->thread);
//             printf("DEBUG: Parent thread added to ready queue\n");
//         }

//         pcb->SignalParent();
//     } else {
//         pcbManager->DeallocatePCB(pcb);
//     }

//     delete currentThread->space;
//     currentThread->space = NULL;
//     currentThread->Finish();

//     ASSERT(false);
// }

void doExit(int status) {
    PCB* pcb = currentThread->space->pcb;
    int pid = pcb->pid;

    printf("DEBUG: Process %d starting Exit with status %d\n", pid, status);
    printf("System Call: [%d] invoked Exit.\n", pid);
    printf("Process [%d] exits with [%d]\n", pid, status);

    pcb->exitStatus = status;
    pcb->exited = true;

    // Wait for any living children to exit
    while (pcb->HasAliveChildren()) {
        printf("DEBUG: PID %d waiting for alive children to exit...\n", pid);
        currentThread->Yield();
    }

    pcb->DeleteExitedChildrenSetParentNull();

    if (pcb->parent != NULL) {
        printf("DEBUG: Signaling parent (PID %d) that child (PID %d) has exited\n",
               pcb->parent->pid, pid);

        if (pcb->parent->thread != NULL) {
            scheduler->ReadyToRun(pcb->parent->thread);
            printf("DEBUG: Parent thread added to ready queue\n");
        }

        pcb->SignalParent();
    } else {
        pcbManager->DeallocatePCB(pcb);
    }

    // Clean up address space
    delete currentThread->space;
    currentThread->space = NULL;

    // Check if this is the last thread before finishing
    Thread* nextThread = scheduler->FindNextToRun();
    if (nextThread == NULL) {
        printf("DEBUG: No more threads to run, shutting down system\n");
        interrupt->Halt();  // Clean shutdown
        // Won't reach this point
    } else {
        printf("DEBUG: Still %d thread(s) in ready queue, finishing normally\n", 
               1); // We know there's at least one thread
    }
    
    // Normal thread termination
    currentThread->Finish();

    // This point should never be reached
    ASSERT(false);
}

void incrementPC() {
    int oldPCReg = machine->ReadRegister(PCReg);

    machine->WriteRegister(PrevPCReg, oldPCReg);
    machine->WriteRegister(PCReg, oldPCReg + 4);
    machine->WriteRegister(NextPCReg, oldPCReg + 8);
}


// void childFunction(int pid) {

//     // 1. Restore the state of registers
//     // currentThread->RestoreUserState()

//     // 2. Restore the page table for child
//     // currentThread->space->RestoreState()

//     // machine->Run();
//     currentThread->RestoreUserState(); 
//     currentThread->space->RestoreState(); 
//     machine->Run(); 
//correct one
// // }
// void childFunction(int pid) {
//     printf("DEBUG: Child function starting for PID %d\n", pid);
//     currentThread->RestoreUserState();
//     currentThread->space->RestoreState();
//     machine->Run(); 
// }
// void childFunction(int pid) {
//     printf("DEBUG: Child function starting for PID %d\n", pid);

//     PCB* pcb = pcbManager->GetPCB(pid);
//     int functionAddr = pcb->forkStartPC;

//     printf("DEBUG: functionAddr = 0x%x\n", functionAddr);

//     currentThread->space->RestoreState();       
//     currentThread->RestoreUserState();          

//     machine->WriteRegister(PCReg, functionAddr);
//     machine->WriteRegister(PrevPCReg, functionAddr - 4);
//     machine->WriteRegister(NextPCReg, functionAddr + 4);

//     // 🟢 استفاده از Getter
//     int stackPointer = (currentThread->space->GetNumPages() * PageSize) - 16;
//     machine->WriteRegister(StackReg, stackPointer);

//     machine->Run();  
//     ASSERT(false);
// }
void childFunction(int pid) {
    printf("DEBUG: Child function starting for PID %d\n", pid);

    PCB* pcb = pcbManager->GetPCB(pid);
    int functionAddr = pcb->forkStartPC;
    printf("DEBUG: functionAddr = 0x%x\n", functionAddr);

    // Restore machine state for this address space
    currentThread->space->RestoreState();

    // Load saved registers from PCB directly to the machine
    for (int i = 0; i < NumTotalRegs; i++) {
        machine->WriteRegister(i, pcb->savedRegisters[i]);
    }
    
    // Override program counter registers
    machine->WriteRegister(PCReg, functionAddr);
    machine->WriteRegister(PrevPCReg, functionAddr - 4);
    machine->WriteRegister(NextPCReg, functionAddr + 4);
    
    // Set return value for child to 0
    machine->WriteRegister(2, 0);

    printf("DEBUG: Child process %d about to run with PC=0x%x\n", 
           pid, machine->ReadRegister(PCReg));
    
    // Run the user code
    machine->Run();

    ASSERT(false);  // Should never reach here
}

// int doFork(int functionAddr) {

//     // 1. Check if sufficient memory exists to create new process
//     // currentThread->space->GetNumPages() <= mm->GetFreePageCount()
//     // if check fails, return -1

//     if (currentThread->space->GetNumPages() > mm->GetFreePageCount()) {
//     return -1;
// }

//     // 2. SaveUserState for the parent thread
//     // currentThread->SaveUserState();
//     currentThread->SaveUserState();

//     // 3. Create a new address space for child by copying parent address space
//     // Parent: currentThread->space
//     // childAddrSpace: new AddrSpace(currentThread->space)
//     AddrSpace* childAddrSpace = new AddrSpace(currentThread->space);


//     // 4. Create a new thread for the child and set its addrSpace
//     // childThread = new Thread("childThread")
//     // child->space = childAddSpace;
//     Thread* childThread = new Thread("childThread");
//     childThread->space = childAddrSpace;


//     // 5. Create a PCB for the child and connect it all up
//     // pcb: pcbManager->AllocatePCB();
//     // pcb->thread = childThread
//     // set parent for child pcb
//     // add child for parent pcb
//     // initialize pcb in childAddSpace
//     PCB* childPCB = pcbManager->AllocatePCB();
//     childPCB->thread = childThread;
//     childPCB->parent = currentThread->space->pcb;
//     currentThread->space->pcb->AddChild(childPCB);
//     childAddrSpace->pcb = childPCB;


//     // 6. Set up machine registers for child and save it to child thread
//     // PCReg: functionAddr
//     // PrevPCReg: functionAddr-4
//     // NextPCReg: functionAddr+4
//     // childThread->SaveUserState();
//     machine->WriteRegister(PCReg, functionAddr);
//     machine->WriteRegister(PrevPCReg, functionAddr - 4);
//     machine->WriteRegister(NextPCReg, functionAddr + 4);
//     childThread->SaveUserState();


//     // 7. Restore register state of parent user-level process
//     // currentThread->RestoreUserState()
//     childThread->Fork(childFunction, childPCB->pid);


//     // 8. Call thread->fork on Child
//     // childThread->Fork(childFunction, pcb->pid)
    
//     // pcreg = machine->ReadRegister(PCReg)
//     // print message for child creation (pid,  pcreg, currentThread->space->GetNumPages())

//     // 9. return pcb->pid;
//     int pc = machine->ReadRegister(PCReg);
//     printf("Child Process Created: PID [%d], PCReg [%d], Pages [%d]\n", 
//     childPCB->pid, pc, currentThread->space->GetNumPages());
//     return childPCB->pid;


// }
// // void childFunction(int pid) {
// //     currentThread->RestoreUserState();  // بازگرداندن رجیسترهای پردازش فرزند
// //     currentThread->space->RestoreState();  // تنظیم جدول صفحه برای پردازش جدید
// //     machine->Run();  // اجرای پردازش جدید
// // }
// -----changed 
// int doFork(int functionAddr) {

//     if (currentThread->space->GetNumPages() > mm->GetFreePageCount()) {
//         return -1; 
//     }

//     currentThread->SaveUserState();


//     AddrSpace* childAddrSpace = new AddrSpace(currentThread->space);

//     Thread* childThread = new Thread("childThread");
//     childThread->space = childAddrSpace;


//     PCB* childPCB = pcbManager->AllocatePCB();
//     childPCB->thread = childThread;
//     childPCB->parent = currentThread->space->pcb;
//     currentThread->space->pcb->AddChild(childPCB);
//     childAddrSpace->pcb = childPCB;

//     machine->WriteRegister(PCReg, functionAddr);
//     machine->WriteRegister(PrevPCReg, functionAddr - 4);
//     machine->WriteRegister(NextPCReg, functionAddr + 4);
//     childThread->SaveUserState();


//     childThread->Fork(childFunction, childPCB->pid);

//     int pc = machine->ReadRegister(PCReg);
//     printf("Child Process Created: PID [%d], PCReg [%d], Pages [%d]\n", 
//            childPCB->pid, pc, currentThread->space->GetNumPages());

//     return childPCB->pid;
// }

// int doFork(int functionAddr) {
//     if (currentThread->space->GetNumPages() > mm->GetFreePageCount()) {
//         int pid = childPCB->pid;
//         printf("DEBUG: Fork completed, returning PID %d\n", pid);
//         return pid;
//         // printf("DEBUG: Fork completed, returning PID %d\n", pid);
//         return -1; 
//     }
    
//     // Add this line to match expected output
//     printf("System Call: [%d] invoked Fork.\n", currentThread->space->pcb->pid);
    
//     currentThread->SaveUserState();
//     AddrSpace* childAddrSpace = new AddrSpace(currentThread->space);
//     Thread* childThread = new Thread("childThread");
//     childThread->space = childAddrSpace;
    
//     PCB* childPCB = pcbManager->AllocatePCB();
//     childPCB->thread = childThread;
//     childPCB->parent = currentThread->space->pcb;
//     currentThread->space->pcb->AddChild(childPCB);
//     childAddrSpace->pcb = childPCB;
    
//     machine->WriteRegister(PCReg, functionAddr);
//     machine->WriteRegister(PrevPCReg, functionAddr - 4);
//     machine->WriteRegister(NextPCReg, functionAddr + 4);
//     childThread->SaveUserState();
    
//     // Replace this message with the correct format
//     printf("Process [%d] Fork: start at address [0x%x] with [%d] pages memory\n", 
//            currentThread->space->pcb->pid,  // Parent PID
//            functionAddr,                   // Function address in hex
//            childAddrSpace->GetNumPages()); // Number of pages
    
//     childThread->Fork(childFunction, childPCB->pid);
    
//     return childPCB->pid;
// }
// correct one
// int doFork(int functionAddr) {
//     // Check if NOT enough memory
//     if (currentThread->space->GetNumPages() > mm->GetFreePageCount()) {
//         // Not enough memory, return error
//         return -1;
//     }
    
//     // Print debug info for system call
//     printf("System Call: [%d] invoked Fork.\n", currentThread->space->pcb->pid);
    
//     // Save parent's state
//     currentThread->SaveUserState();
    
//     // Create a new address space for child by copying parent's address space
//     AddrSpace* childAddrSpace = new AddrSpace(currentThread->space);
    
//     // Create a new thread for the child and set its address space
//     Thread* childThread = new Thread("childThread");
//     childThread->space = childAddrSpace;
    
//     // Create a PCB for the child and set up relationships
//     PCB* childPCB = pcbManager->AllocatePCB();
//     childPCB->thread = childThread;
//     childPCB->parent = currentThread->space->pcb;
//     currentThread->space->pcb->AddChild(childPCB);
//     childAddrSpace->pcb = childPCB;
    
//     // Set up machine registers for child
//     machine->WriteRegister(PCReg, functionAddr);
//     machine->WriteRegister(PrevPCReg, functionAddr - 4);
//     machine->WriteRegister(NextPCReg, functionAddr + 4);
//     childThread->SaveUserState();
    
//     // Print debug message in the expected format
//     printf("Process [%d] Fork: start at address [0x%x] with [%d] pages memory\n", 
//            currentThread->space->pcb->pid,  // Parent PID
//            functionAddr,                   // Function address
//            childAddrSpace->GetNumPages()); // Number of pages
    
//     // Fork the child thread with the child function
//     childThread->Fork(childFunction, childPCB->pid);
    
//     // Return the child's PID
//     return childPCB->pid;
// }
// int doFork(int functionAddr) {
//     // اگر حافظه کافی برای ساخت فرزند وجود نداره، خطا بده
//     if (currentThread->space->GetNumPages() > mm->GetFreePageCount()) {
//         return -1;
//     }

//     // چاپ دیباگ طبق فرمت مورد انتظار
//     printf("System Call: [%d] invoked Fork.\n", currentThread->space->pcb->pid);

//     // ذخیره وضعیت رجیسترهای parent
//     currentThread->SaveUserState();

//     // ایجاد address space جدید برای فرزند با کپی از والد
//     AddrSpace* childAddrSpace = new AddrSpace(currentThread->space);

//     // ساخت نخ جدید برای فرزند و اتصال address space
//     Thread* childThread = new Thread("childThread");
//     childThread->space = childAddrSpace;

//     // ساخت PCB جدید برای فرزند و تنظیم روابط والد-فرزند
//     PCB* childPCB = pcbManager->AllocatePCB();
//     childPCB->thread = childThread;
//     childPCB->parent = currentThread->space->pcb;
//     currentThread->space->pcb->AddChild(childPCB);
//     childAddrSpace->pcb = childPCB;

//     // 🟢 این خط خیلی مهمه: انتقال آدرس تابع sum به PCB فرزند
//     childPCB->forkStartPC = functionAddr;

//     // چاپ پیغام مورد انتظار برای پروژه
//     printf("Process [%d] Fork: start at address [0x%x] with [%d] pages memory\n",
//            currentThread->space->pcb->pid,
//            functionAddr,
//            childAddrSpace->GetNumPages());

//     // اجرای نخ فرزند با childFunction(pid)
//     childThread->Fork(childFunction, childPCB->pid);

//     // برگشت PID فرزند به عنوان خروجی syscall
//     return childPCB->pid;
// }

// int doFork(int functionAddr) {
//     // بررسی حافظه‌ی کافی برای ساخت فرزند
//     if (currentThread->space->GetNumPages() > mm->GetFreePageCount()) {
//         return -1;
//     }

//     // چاپ پیام طبق فرمت مورد انتظار
//     printf("System Call: [%d] invoked Fork.\n", currentThread->space->pcb->pid);

//     // ذخیره وضعیت والد قبل از فورک
//     currentThread->SaveUserState();

//     // ساخت آدرس‌فضای جدید برای فرزند با کپی از والد
//     AddrSpace* childAddrSpace = new AddrSpace(currentThread->space);
//     if (!childAddrSpace->valid) return -1;

//     // ساخت نخ و اتصال آن به فضای آدرس جدید
//     Thread* childThread = new Thread("childThread");
//     childThread->space = childAddrSpace;

//     // ساخت PCB برای فرزند و تنظیم روابط والد-فرزند
//     PCB* childPCB = pcbManager->AllocatePCB();
//     childPCB->thread = childThread;
//     childPCB->parent = currentThread->space->pcb;
//     childAddrSpace->pcb = childPCB;
//     currentThread->space->pcb->AddChild(childPCB);

//     // ذخیره آدرس تابع sum در PCB فرزند
//     childPCB->forkStartPC = functionAddr;

//     // چاپ پیام دیباگ طبق فرمت پروژه
//     printf("Process [%d] Fork: start at address [0x%x] with [%d] pages memory\n",
//            currentThread->space->pcb->pid,
//            functionAddr,
//            childAddrSpace->GetNumPages());

//     // اجرای تابع فرزند
//     childThread->Fork(childFunction, childPCB->pid);

//     // بازگشت PID فرزند به عنوان نتیجه‌ی system call
//     return childPCB->pid;
// }
int doFork(int functionAddr) {
    // Check if enough memory
    if (currentThread->space->GetNumPages() > mm->GetFreePageCount()) {
        return -1;
    }

    // Print system call message
    printf("System Call: [%d] invoked Fork.\n", currentThread->space->pcb->pid);
    static int forkCount = 0;
    forkCount++;
    printf("DEBUG: doFork() called — Fork count: %d\n", forkCount);


    // Create new address space for child by copying from parent
    AddrSpace* childAddrSpace = new AddrSpace(currentThread->space);
    if (!childAddrSpace->valid) return -1;

    // Create a new thread and attach it to the new address space
    Thread* childThread = new Thread("childThread");
    childThread->space = childAddrSpace;

    // Create PCB for child and set up parent-child relationships
    PCB* childPCB = pcbManager->AllocatePCB();
    childPCB->thread = childThread;
    childPCB->parent = currentThread->space->pcb;
    childAddrSpace->pcb = childPCB;
    currentThread->space->pcb->AddChild(childPCB);

    // Save function address and current machine registers in PCB
    childPCB->forkStartPC = functionAddr;
    
    // Instead of accessing userRegisters directly, read from the machine
    for (int i = 0; i < NumTotalRegs; i++) {
        childPCB->savedRegisters[i] = machine->ReadRegister(i);
    }

    // Debug print message
    printf("Process [%d] Fork: start at address [0x%x] with [%d] pages memory\n",
           currentThread->space->pcb->pid, functionAddr, childAddrSpace->GetNumPages());

    // Execute child function
    childThread->Fork(childFunction, childPCB->pid);
    printf("DEBUG: childThread->Fork called for child PID %d\n", childPCB->pid);


    // Return child PID as system call result
    return childPCB->pid;
}

int doExec(char* filename) {

    // Use progtest.cc:StartProcess() as a guide

    DEBUG('a', "Running doExec for file: %s\n", filename);
    // 1. Open the file and check validity
    // OpenFile *executable = fileSystem->Open(filename);
    // AddrSpace *space;

    //Open exec. file
    OpenFile *executable = fileSystem->Open(filename);

    // if (executable == NULL) {
    //     printf("Unable to open file %s\n", filename);
    //     return -1;
    // }
    if (executable == NULL)
    {
        //Unable to open file
        printf("Unable to open file %s\n", filename);
        return -1;
    }

    // 2. Delete current address space but store current PCB first if using in Step 5.
    // PCB* pcb = currentThread->space->pcb;
    // delete currentThread->space;

    //Save current process's PCB
    PCB* oldPCB = currentThread->space->pcb;

    //deleting current address space
    delete currentThread->space;

    // 3. Create new address space
    // space = new AddrSpace(executable);

    // Creation of new address space
    AddrSpace *space = new AddrSpace(executable);

    // 4.     delete executable;			// close file

    //Deleting file object
    delete executable;

    // 5. Check if Addrspace creation was successful
    // if(space->valid != true) {
    // printf("Could not create AddrSpace\n");
    //     return -1;
    // }

    //Checking if AddrSpace was created
    if (!space->valid)
    {
        printf("Could not create AddrSpace for %s\n", filename);
        return -1;
    }
    
    // 6. Set the PCB for the new addrspace - reused from deleted address space
    // space->pcb = pcb;

    //Reassigning PCB for new addrSpace 
    space->pcb = oldPCB;

    // 7. Set the addrspace for currentThread
    // currentThread->space = space;

    //setting new address for current thread
    currentThread->space = space;

    // 8. Initialize registers for new addrspace
    //  space->InitRegisters();		// set the initial register values

    //print debug output
    printf("Exec Program: [%d] loading [%s]\n", oldPCB->getID(), filename);

    //initialize CPU regs for new program
    space->InitRegisters();

    // 9. Initialize the page table
    // space->RestoreState();		// load page table register

    //restoring address space
    space->RestoreState();

    // 10. Run the machine now that all is set up
    // machine->Run();			// jump to the user progam
    // ASSERT(FALSE); // Execution nevere reaches here

    //begin running program
    machine->Run();

    //failsafe
    ASSERT(FALSE);

    // return success 
    return 0;
}


int doJoin(int pid) {

    // 1. Check if this is a valid pid and return -1 if not
    // PCB* joinPCB = pcbManager->GetPCB(pid);
    // if (pcb == NULL) return -1;

    // 2. Check if pid is a child of current process
    // PCB* pcb = currentThread->space->pcb;
    // if (pcb != joinPCB->parent) return -1;

    // 3. Yield until joinPCB has not exited
    // while(!joinPCB->hasExited) currentThread->Yield();

    // 4. Store status and delete joinPCB
    // int status = joinPCB->exitStatus;
    // delete joinPCB;

    // 5. return status;
    return -1;

}


int doKill (int pid) {

    // 1. Check if the pid is valid and if not, return -1
    // PCB* joinPCB = pcbManager->GetPCB(pid);
    // if (pcb == NULL) return -1;

    // 2. IF pid is self, then just exit the process
    // if (pcb == currentThread->space->pcb) {
    //         doExit(0);
    //         return 0;
    // }

    // 3. Valid kill, pid exists and not self, do cleanup similar to Exit
    // However, change references from currentThread to the target thread
    // pcb->thread is the target thread

    // 4. Set thread to be destroyed.
    // scheduler->RemoveThread(pcb->thread);

    // 5. return 0 for success!
    return -1;
}


// ---Chanegd
// void doYield() {
//     printf("Yielding CPU from process %d\n", currentThread->space->pcb->pid);
//     currentThread->Yield();
// }
// void doYield() {
//     printf("System Call: [%d] invoked Yield.\n", currentThread->space->pcb->pid);
//     currentThread->Yield();
// }
void doYield() {
    // Print debug message in this function only
    printf("System Call: [%d] invoked Yield.\n", currentThread->space->pcb->pid);
    
    // Save the current state before yielding
    currentThread->SaveUserState();
    
    // Perform the yield operation
    currentThread->Yield();
    
    // Restore state after returning from yield
    currentThread->RestoreUserState();
    
    // Note: PC increment is handled in the ExceptionHandler
}
// This implementation (discussed in one of the videos) is broken!
// Try and figure out why.
char* readString1(int virtAddr) {

    unsigned int pageNumber = virtAddr / 128;
    unsigned int pageOffset = virtAddr % 128;
    unsigned int frameNumber = machine->pageTable[pageNumber].physicalPage;
    unsigned int physicalAddr = frameNumber*128 + pageOffset;

    char *string = &(machine->mainMemory[physicalAddr]);

    return string;

}




// This implementation is correct!
// perform MMU translation to access physical memory
char* readString(int virtualAddr) {
    int i = 0;
    char* str = new char[256];
    unsigned int physicalAddr = currentThread->space->Translate(virtualAddr);

    // Need to get one byte at a time since the string may straddle multiple pages that are not guaranteed to be contiguous in the physicalAddr space
    bcopy(&(machine->mainMemory[physicalAddr]),&str[i],1);
    while(str[i] != '\0' && i != 256-1)
    {
        virtualAddr++;
        i++;
        physicalAddr = currentThread->space->Translate(virtualAddr);
        bcopy(&(machine->mainMemory[physicalAddr]),&str[i],1);
    }
    if(i == 256-1 && str[i] != '\0')
    {
        str[i] = '\0';
    }

    return str;
}

void doCreate(char* fileName)
{
    printf("Syscall Call: [%d] invoked Create.\n", currentThread->space->pcb->pid);
    fileSystem->Create(fileName, 0);
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
	DEBUG('a', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();
    } else  if ((which == SyscallException) && (type == SC_Exit)) {
        doExit(machine->ReadRegister(4));
        incrementPC(); 
    } else if ((which == SyscallException) && (type == SC_Fork)) {
        int ret = doFork(machine->ReadRegister(4));
        machine->WriteRegister(2, ret);
        incrementPC();
    } else if ((which == SyscallException) && (type == SC_Exec)) {
        int virtAddr = machine->ReadRegister(4);
        char* fileName = readString(virtAddr);
        //print syscall trace
        printf("System call: [%d] invoked Exec. \n", currentThread->space->pcb->getID());
        int ret = doExec(fileName);
        machine->WriteRegister(2, ret);
        //cleaning up allocated filename string
        delete[] fileName;
        incrementPC();
    } else if ((which == SyscallException) && (type == SC_Join)) {
        int ret = doJoin(machine->ReadRegister(4));
        machine->WriteRegister(2, ret);
        incrementPC();
    } else if ((which == SyscallException) && (type == SC_Kill)) {
        int ret = doKill(machine->ReadRegister(4));
        machine->WriteRegister(2, ret);
        incrementPC();
    } else if ((which == SyscallException) && (type == SC_Yield)) {
        // doYield();
        // incrementPC();
        printf("DEBUG: Handling Yield syscall\n");
        doYield();
        printf("DEBUG: After doYield(), incrementing PC\n");
        incrementPC();
        printf("DEBUG: PC incremented, returning from exception handler\n");
    } else if((which == SyscallException) && (type == SC_Create)) {
        int virtAddr = machine->ReadRegister(4);
        char* fileName = readString(virtAddr);
        doCreate(fileName);
        incrementPC();
    } else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
    }

}

