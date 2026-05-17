#define VM_IMPLEMENTATION
#include "vm.h"
#include "./config.h"

int main() {
    vm_t vm = vm_init(
        VM_ROWS, VM_COLS,
        VM_MAX_CELL_QNT,
        vm_load_products_from_memory(
            g_StockProducts,
            ARR_SIZE(g_StockProducts), 
            VM_PRODUCT_STATIC_STORAGE,
            __stock_product_count
        ),
        NULL
    );

    vm_load_slots(&vm, "./res/slots.bin");
    vm_sync_slots(&vm);
    vm_print_slots(&vm, -1);

    vm_free(&vm);
    return 0;
}