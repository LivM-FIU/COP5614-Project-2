#include "syscall.h"

int main() {
    int i;
    for (i = 0; i < 10; i++) {
        Yield();  // Yield the CPU to another thread
        printf("Yield test: %d\n", i); // Print the current number using printf
    }
    Exit(0);  // Exit after printing the numbers
}


