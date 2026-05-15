#define VM_IMPLEMENTATION
#include "vm.h"
#include "./config.h"

int main() {
    vm_t vm = vm_init_rand(
        VM_ROWS, VM_COLS, 
        VM_MAX_CELL_QNT,
        g_StockProducts,
        ARR_SIZE(g_StockProducts)
    );
    
    vm_print_slots(&vm, -1);
    vm_free(&vm);
    return 0;
}