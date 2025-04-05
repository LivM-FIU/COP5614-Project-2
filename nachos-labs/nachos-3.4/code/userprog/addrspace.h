// addrspace.h
// Data structures to keep track of executing user programs (address spaces).
// For now, we don't keep any information about address spaces.
// The user-level CPU state is saved and restored in the thread
// executing the user program (see thread.h).

// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
class PCB;  // Forward declaration to avoid circular includes

#define UserStackSize 1024  // Increase this as necessary!

class AddrSpace {
  public:
    AddrSpace(OpenFile *executable);         // Create an address space from an executable file
    AddrSpace(AddrSpace* space);             // Copy constructor for forking
    ~AddrSpace();                            // Destructor

    void InitRegisters();                    // Initialize user-level registers
    void SaveState();                        // Save address space-specific state
    void RestoreState();                     // Restore address space-specific state

    unsigned int GetNumPages();              // Get number of pages in this address space
    TranslationEntry* GetPageTable();        // Return pointer to the page table
    unsigned int Translate(unsigned int virtualAddr);  // Translate virtual to physical address

    PCB* pcb;                                // Pointer to owning process control block
    bool valid;                              // Whether address space was created successfully

    void ReadFile(OpenFile *file, int offset, int virtualAddr, int size); // Read into virtual space
    bool IsValid();                          // Check if address space is valid
    void ReleaseMemory();                    // Free memory resources used by this space

  private:
    TranslationEntry *pageTable;             // Linear page table
    unsigned int numPages;                   // Number of virtual pages
};

#endif // ADDRSPACE_H
