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
#define SLOT_PTR(slots) (vm_slot_t*) (slots);

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

extern void         vm_init_rand();
extern void         vm_fill();
extern void         vm_print_slots(int limit);
extern vm_buy_err_t vm_buy(int slot_idx, float cash, float* change);
extern const char*  vm_buy_result(vm_buy_err_t err);
extern void         vm_restock(int slot_idx);
extern void         vm_slot_set_qnt(int slot_idx, int qnt);

#ifdef VM_IMPLEMENTATION

static vm_slot_t vm_slot[VM_ROWS][VM_COLS];

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

void vm_fill() {
    for (int i = 0; i < VM_ROWS; ++i) {
        for (int j = 0; j < VM_COLS; ++j) {
            float margin = PROFIT_MARGIN;
            int p_idx = rand() % ARR_SIZE(products);
            vm_slot[i][j].prod_id = p_idx;
            vm_slot[i][j].qnt     = rand() % (MAX_CELL_QNT) + 1;
            vm_slot[i][j].price   = (products[p_idx].unit_cost / 100.0f) * (1 + margin);
        }
    }
}

void vm_print_slots(int limit) {
    for (int i = 0; i < VM_ROWS * VM_COLS && i < limit; ++i) {
        vm_slot_t* p = SLOT_PTR(vm_slot);
        if ((i % VM_COLS) == 0 && i > 0) {
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

vm_buy_err_t vm_buy(int slot_idx, float cash, float* change) {
    if (slot_idx < 0 || slot_idx >= VM_ROWS * VM_COLS)
        return VM_BUY_INVALID_SLOT; // invalid slot

    vm_slot_t* slots = SLOT_PTR(vm_slot);

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

void vm_init_rand() {
    srand(time(NULL));
    vm_fill();
}

void vm_restock(int slot_idx) {
    vm_slot_t* slots    = SLOT_PTR(vm_slot);
    slots[slot_idx].qnt = MAX_CELL_QNT;
}

void vm_slot_set_qnt(int slot_idx, int qnt) {
    vm_slot_t* slots    = SLOT_PTR(vm_slot);
    slots[slot_idx].qnt = qnt;

    // clamping.
    if (slots[slot_idx].qnt < 0) {
        slots[slot_idx].qnt = 0;
    } else {
        if (slots[slot_idx].qnt > MAX_CELL_QNT)
            slots[slot_idx].qnt = MAX_CELL_QNT;
    }
}

#endif // VM_IMPLEMENTATION
