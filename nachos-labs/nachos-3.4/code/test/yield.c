#include "syscall.h"

int main() {
    PrintString("Before Yield\n");
    Yield();
    PrintString("After Yield\n");
    Exit(0);
}
