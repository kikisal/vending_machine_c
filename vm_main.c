#define VM_IMPLEMENTATION
#include "vm.h"
#include "./config.h"

int main() {
    vm_state_t vm = vm_init(
        VM_ROWS, VM_COLS, 
        VM_MAX_CELL_QNT, 
        NULL
    );
    
    vm_free(&vm);
    return 0;
}