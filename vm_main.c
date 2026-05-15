#define VM_IMPLEMENTATION
#include "vm.h"

int main() {
    vm_init_rand();
    vm_print_slots(4);
    return 0;
}