// Vending Machine header file. Not virtual machine.
// TODO: Support multiple platforms.

#pragma once

#include "./vm_config.h"

#ifdef VM_IMPLEMENTATION
#   if PLATFORM == PLATFORM_OS
#       include <stdio.h>
#       include <stdlib.h>
#       include <time.h>
#       include <stdbool.h>
#       include <assert.h>
#       include <string.h>
#   else
#        error "PLATFORM NOT IMPLEMENTED!"
#   endif
#endif // VM_IMPLEMENTATION


#define ARR_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))
// #define SLOT_PTR(slots) (vm_slot_t*) (slots);

typedef int vm_buy_err_t;

#define VM_BUY_SUCCESS          0
#define VM_BUY_INVALID_SLOT    -1
#define VM_BUY_INVALID_SLOT    -1
#define VM_BUY_EMPTY_SLOT      -2
#define VM_BUY_NOT_ENOUGH_CASH -3

typedef struct product_st {
    const char* name;
    // used by the graphics library to fetch texture
    const char* display_name;
    float unit_cost; // in cents
    float profit_margin;
} product_t;

typedef struct vm_slot_st {
    int prod_id;
    int qnt;
    float price;
} vm_slot_t;

typedef struct vm_st {
    vm_slot_t* slot;
    int        slot_count;
    int        rows;
    int        cols;
    product_t* products;
    size_t     product_count;

    int        max_cell_qnt;
    bool       is_static;
} vm_t;

#define VM_API extern

VM_API vm_t         vm_init_rand(int rows, int cols, int max_cell_qnt, product_t* products, size_t product_count);
VM_API void         vm_fill_rand(vm_t* vm);
VM_API vm_t         vm_init(int rows, int cols, int max_cell_qnt, product_t* products, size_t product_count, vm_slot_t* static_storage);
VM_API void         vm_print_slots(vm_t* vm, int limit);
VM_API vm_buy_err_t vm_buy(vm_t* vm, int slot_idx, float cash, float* change);
VM_API const char*  vm_buy_result(vm_buy_err_t err);
VM_API void         vm_restock(vm_t* vm, int slot_idx);
VM_API void         vm_slot_set_qnt(vm_t* vm, int slot_idx, int qnt);
VM_API void         vm_free(vm_t* vm);
VM_API size_t       vm_indexof_product(vm_t* vm, const char* display_name);
VM_API void         vm_slot_update_product_price(vm_t* vm, size_t prod_id);
VM_API void         vm_product_set_margin(vm_t* vm, const char* display_name, float margin);
VM_API void         vm_product_set_unitcost(vm_t* vm, const char* display_name, float cost);
VM_API int          vm_load_slots_from_memory(vm_t* vm, vm_slot_t* slots, size_t count);
VM_API void         vm_bake_slots(vm_t* vm, const char* fp);

static void         vm__fwrite_str(const char* str, FILE* fh);


#ifdef VM_IMPLEMENTATION

static product_t g_DefaultProducts[] = {
    {
        .name      = "Bad Dog",
        .unit_cost = 50.0f,
    },
    {
        .name      = "Coca Cola",
        .unit_cost = 65.0f
    },
    {
        .name      = "Monster",
        .unit_cost = 65.0f,
    },
    {
        .name      = "Chips",
        .unit_cost = 40.0f,
    },
};

void vm_fill_rand(vm_t* vm) {
    for (int i = 0; i < vm->rows; ++i) {
        for (int j = 0; j < vm->cols; ++j) {
            int p_idx = rand() % vm->product_count;
            float margin = vm->products[p_idx].profit_margin;
            vm->slot[i*vm->cols + j].prod_id = p_idx;
            vm->slot[i*vm->cols + j].qnt     = rand() % (vm->max_cell_qnt) + 1;
            vm->slot[i*vm->cols + j].price   = (vm->products[p_idx].unit_cost / 100.0f) * (1 + margin);
        }
    }
}

void vm_print_slots(vm_t* vm, int limit) {
    if (limit < 0 || limit > vm->slot_count)
        limit = vm->slot_count;
    
    for (int i = 0; i < limit; ++i) {
        vm_slot_t* p = vm->slot;

        if ((i % vm->cols) == 0 && i > 0) {
            printf("\n");
        }

        #ifdef PRINT_NAMES
            printf("(%02d, %2d, %2d, %.2f, \"%s\") ", i, p[i].prod_id, p[i].qnt, p[i].price, vm->products[p[i].prod_id].name);
        #else
            printf("(%02d, %02d, %02d, %0.2f) ", i, p[i].prod_id, p[i].qnt, p[i].price);
        #endif //  PRINT_NAMES
    }

    printf("\n");
}

vm_buy_err_t vm_buy(vm_t* vm, int slot_idx, float cash, float* change) {
    if (slot_idx < 0 || slot_idx >= vm->slot_count)
        return VM_BUY_INVALID_SLOT; // invalid slot

    vm_slot_t* slots = vm->slot;

    if (slots[slot_idx].qnt <= 0)
        return VM_BUY_EMPTY_SLOT; // empty slot

    if (cash < slots[slot_idx].price)
        return VM_BUY_NOT_ENOUGH_CASH; // not enough cash

    slots[slot_idx].qnt -= 1;

    if (change) {
        *change = cash - slots[slot_idx].price;
    }    

    return VM_BUY_SUCCESS;
}

const char* vm_buy_result(vm_buy_err_t err) {
    switch (err) {
        case VM_BUY_SUCCESS: 
            return "VM_BUY_SUCCESS";
        case VM_BUY_INVALID_SLOT: 
            return "VM_BUY_INVALID_SLOT";
        case VM_BUY_EMPTY_SLOT: 
            return "VM_BUY_EMPTY_SLOT";
        case VM_BUY_NOT_ENOUGH_CASH: 
            return "VM_BUY_NOT_ENOUGH_CASH";
        default:
            return "VM_UNKNOWN_ERROR";    
    }
}

vm_t vm_init(int rows, int cols, int max_cell_qnt, product_t* products, size_t product_count, vm_slot_t* static_storage) {
    vm_t vm = {0};
    vm.rows          = rows;
    vm.cols          = cols;
    vm.max_cell_qnt  = max_cell_qnt;
    vm.slot_count    = rows * cols;
    vm.products      = products != NULL ? products : g_DefaultProducts;
    vm.product_count = products != NULL ? product_count : ARR_SIZE(g_DefaultProducts);
#if PLATFORM == PLATFORM_OS
    (void) static_storage;
    vm.slot = malloc(vm.slot_count * sizeof(vm.slot[0]));
#elif PLATFORM == PLATFORM_EMBEDDED
    vm.slot      = static_storage;
    vm.is_static = true;
#else
#   error "Platform not implemented"
#endif
    return vm;
}

vm_t vm_init_rand(int rows, int cols, int max_cell_qnt, product_t* products, size_t product_count) {
    vm_t vm = vm_init(rows, cols, max_cell_qnt, products, product_count, NULL);
    srand(time(NULL));
    vm_fill_rand(&vm);
    return vm;
}

void vm_restock(vm_t* vm, int slot_idx) {
    if (!vm) return;
    vm->slot[slot_idx].qnt = vm->max_cell_qnt;
}

void vm_slot_set_qnt(vm_t* vm, int slot_idx, int qnt) {
    if (!vm) return;

    vm->slot[slot_idx].qnt = qnt;

    // clamping.
    if (vm->slot[slot_idx].qnt < 0) {
        vm->slot[slot_idx].qnt = 0;
    } else {
        if (vm->slot[slot_idx].qnt > vm->max_cell_qnt)
            vm->slot[slot_idx].qnt = vm->max_cell_qnt;
    }
}

void vm_free(vm_t* vm) {
    if (!vm) return;
    
    if (vm->slot && !vm->is_static) 
        free(vm->slot);
    *vm = (vm_t){0};
}

size_t vm_indexof_product(vm_t* vm, const char* display_name) {
    size_t idx = -1;
    for (int i = 0; i < vm->product_count; ++i) {
        if (strncmp(vm->products[i].display_name, display_name, strlen(display_name)) == 0) {
            idx = i;
            break;
        }
    }

    return idx;
}

void vm_slot_update_product_price(vm_t* vm, size_t prod_id) {
    for (int i = 0; i < vm->slot_count; ++i) {
        if (vm->slot[i].prod_id == prod_id) {
            product_t p = vm->products[prod_id];
            vm->slot[i].price   = (p.unit_cost / 100.0f) * (1 + p.profit_margin);
        }
    }
}

void vm_product_set_unitcost(vm_t* vm, const char* display_name, float cost) {
    size_t prd_idx = vm_indexof_product(vm, display_name);
    if (prd_idx < 0) return;

    vm->products[prd_idx].unit_cost = cost;
    vm_slot_update_product_price(vm, prd_idx);
}

void vm_product_set_margin(vm_t* vm, const char* display_name, float margin) {
    size_t prd_idx = vm_indexof_product(vm, display_name);
    if (prd_idx < 0) return;

    vm->products[prd_idx].profit_margin = margin;
    vm_slot_update_product_price(vm, prd_idx);
}

int vm_load_slots_from_memory(vm_t* vm, vm_slot_t* slots, size_t count) {
    if (vm->slot_count != count) return -1;

    memcpy(vm->slot, slots, count * sizeof(slots[0]));
    return 0;
}

static void vm__fwrite_str(const char* str, FILE* fh) {
    if (!fh) return;
    fwrite(str, 1, strlen(str), fh);
}

void vm_bake_slots(vm_t* vm, const char* fp) {
    FILE* fh = fopen(fp, "w");
    if (!fh) return;
    char tmp_buff[STR_TMP_BUFF_SIZE];

    vm__fwrite_str("#pragma once\n\n", fh);
    vm__fwrite_str("static vm_slot_t g_Slots[VM_ROWS*VM_COLS] = {\n", fh);
    for (int i = 0; i < vm->slot_count; ++i) {
        vm_slot_t slot = vm->slot[i];
        snprintf(tmp_buff, STR_TMP_BUFF_SIZE, "\t{.prod_id = %2d, .qnt = %2d, .price = %.2f},\n", slot.prod_id, slot.qnt, slot.price);
        vm__fwrite_str(tmp_buff, fh);
    }
    vm__fwrite_str("};\n", fh);
    fclose(fh);
}

#endif // VM_IMPLEMENTATION