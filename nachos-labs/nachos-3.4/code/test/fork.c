#include "syscall.h"

int global_cnt = 0;

void sum() {
    int i;
    for (i = 0; i < 100; i++) {
        global_cnt++;
    }
    Exit(i);
}

int main() {
    global_cnt++;
    
    /* First fork */
    Fork(sum);
    
    /* First yield */
    Yield();
    
    /* After first yield */
    global_cnt++;
    Fork(sum);
    
    /* Second yield */
    Yield();
    
    /* After second yield */
    global_cnt++;
    Fork(sum);
    
    /* Third yield */
    Yield();
    
    /* Final exit */
    global_cnt++;
    Exit(global_cnt);
}