#define VM_IMPLEMENTATION
#include "vm.h"
#define VM_RENDERER_IMPLEMENTATION
#include "vm_renderer.h"
#include "./config.h"

#define OBJECTS_IMPL
#include "./objects/objects.h"

#define VM_WIN_WIDTH  960
#define VM_WIN_HEIGHT 600

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
    vm_sync_slots_with_products(&vm);
    vm_render(&vm, VM_WIN_WIDTH, VM_WIN_HEIGHT, "Vending Machine :)");
    
    vm_free(&vm);
    return 0;
}

void vm_render_init(vm_renderer_t* r) {
    vm_t* vm         = r->vm;

    size_t offset_x  = r->width / 2.0f - (((GRID_SIZE + GRID_MARGIN_X) * vm->cols) / 2.0f); 
    size_t offset_y  = GRID_SIZE / 2 + 10;

    for (int i = 0; i < vm->rows; ++i) {
        for (int j = 0; j < vm->cols; ++j) {

            vm_slot_t* slot    = &vm->slot[i * vm->cols + j];
            product_t* product = vm_find_product(vm, slot->prod_id);
            if (!product) continue;

            int sprite_x = j * (GRID_SIZE + GRID_MARGIN_X) + offset_x;
            int sprite_y = i * (GRID_SIZE + GRID_MARGIN_Y) + offset_y;

            vm_sprite_t sprite = {
                .x     = (float) sprite_x,
                .y     = (float) sprite_y,
                .rot   = 0.0f,
                .sheet = &g_SpriteSheet,
                .frame = product->display_name 
            };

            obj_id slot_obj = ObjSlot_init(
                sprite.x, sprite.y, 0.0f, 
                r->vm, sprite, 
                16.0f * ((float)rand() / (float)RAND_MAX),
                slot
            );
            assert(slot_obj.index >= 0);
            vm_object_array_add(&r->objects, slot_obj);
        }
    }

    g_SceneControlObj = ObjSceneControl_init();
    vm_object_array_add(&r->objects, g_SceneControlObj);
}