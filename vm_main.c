#define VM_IMPLEMENTATION
#include "vm.h"
#include "./config.h"

int main() {
    vm_state_t vm = vm_init_rand(VM_ROWS, VM_COLS, VM_MAX_CELL_QNT, VM_PROFIT_MARGIN);

    return 0;
}