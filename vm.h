// Vending Machine header file. Not virtual machine.
// TODO: Support multiple platforms.

#pragma once

// DO NOT TOUCH
#define PLATFORM_OS       1
#define PLATFORM_EMBEDDED 2

#include "./config.h"
#include <assert.h>

#ifdef VM_IMPLEMENTATION
#   if PLATFORM == PLATFORM_OS
#       include <stdio.h>
#       include <stdlib.h>
#       include <time.h>
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
    float unit_cost; // in cents
} product_t;

typedef struct vm_product_st {
    int prod_id;
    int qnt;
    float price;
} vm_slot_t;


typedef struct vm_state_st {
    vm_slot_t* slot;
    int        slot_count;
    int        rows;
    int        cols;

    int        max_cell_qnt;
    float      profit_margin;
} vm_state_t;


extern vm_state_t   vm_init_rand(int rows, int cols, int max_cell_qnt, float profit_margin);
extern void         vm_fill(vm_state_t* vm);
extern void         vm_print_slots(vm_state_t* vm, int limit);
extern vm_buy_err_t vm_buy(vm_state_t* vm, int slot_idx, float cash, float* change);
extern const char*  vm_buy_result(vm_buy_err_t err);
extern void         vm_restock(vm_state_t* vm, int slot_idx);
extern void         vm_slot_set_qnt(vm_state_t* vm, int slot_idx, int qnt);

#ifdef VM_IMPLEMENTATION

static product_t products[] = {
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

void vm_fill(vm_state_t* vm) {
    for (int i = 0; i < vm->rows; ++i) {
        for (int j = 0; j < vm->cols; ++j) {
            float margin = vm->profit_margin;
            int p_idx = rand() % ARR_SIZE(products);
            vm->slot[i*vm->cols + j].prod_id = p_idx;
            vm->slot[i*vm->cols + j].qnt     = rand() % (vm->max_cell_qnt) + 1;
            vm->slot[i*vm->cols + j].price   = (products[p_idx].unit_cost / 100.0f) * (1 + margin);
        }
    }
}

void vm_print_slots(vm_state_t* vm, int limit) {
    if (limit < 0 || limit > vm->slot_count)
        limit = vm->slot_count;
    
    for (int i = 0; i < limit; ++i) {
        vm_slot_t* p = vm->slot;

        if ((i % vm->cols) == 0 && i > 0) {
            printf("\n");
        }

        #ifdef PRINT_NAMES
            printf("(%d, %d, %.2f, \"%s\") ", p[i].prod_id, p[i].qnt, p[i].price, products[p[i].prod_id].name);
        #else
            printf("(%d, %d, %.2f) ", p[i].prod_id, p[i].qnt, p[i].price);
        #endif //  PRINT_NAMES
    }

    printf("\n");
}

vm_buy_err_t vm_buy(vm_state_t* vm, int slot_idx, float cash, float* change) {
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

vm_state_t vm_init_rand(int rows, int cols, int max_cell_qnt, float profit_margin) {
    vm_slot_t* slots = malloc(sizeof(vm_slot_t) * rows * cols);
    vm_state_t vm;

    vm.rows          = rows;
    vm.cols          = cols;
    vm.slot_count    = rows*cols;
    vm.slot          = slots;
    vm.max_cell_qnt  =  max_cell_qnt;
    vm.profit_margin = profit_margin;
    
    srand(time(NULL));
    vm_fill(&vm);
    return vm;
}

void vm_restock(vm_state_t* vm, int slot_idx) {
    if (!vm) return;
    vm->slot[slot_idx].qnt = vm->max_cell_qnt;
}

void vm_slot_set_qnt(vm_state_t* vm, int slot_idx, int qnt) {
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

#endif // VM_IMPLEMENTATION
