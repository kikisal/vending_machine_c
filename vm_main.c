#define VM_IMPLEMENTATION
#include "vm.h"
#include "./config.h"

int main() {
    vm_t vm = vm_init(
        VM_ROWS, VM_COLS,
        VM_MAX_CELL_QNT,
        g_StockProducts,
        ARR_SIZE(g_StockProducts), 
        NULL
    );

    vm_load_slots(&vm, "./res/slots.bin");
    vm_free(&vm);
    return 0;
}