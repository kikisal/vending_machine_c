#define VM_IMPLEMENTATION
#include "vm.h"

#define VM_ROWS         12
#define VM_COLS         6
#define VM_MAX_CELL_QNT    6
#define VM_PROFIT_MARGIN 1.0f

int main() {
    vm_state_t vm = vm_init_rand(VM_ROWS, VM_COLS, VM_MAX_CELL_QNT, VM_PROFIT_MARGIN);

    vm_print_slots(&vm, 4);
    vm_buy(&vm, 3, 10.0f, NULL);
    vm_print_slots(&vm, 4);


    return 0;
}