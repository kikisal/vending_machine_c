#pragma once

#include "vm.h"
#include <assert.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <math.h>

#define TEXTURE_OFFSET_SIZE 1024
#define GRID_SIZE           64
#define GRID_MARGIN_Y       55
#define GRID_MARGIN_X       60
#define GRID_TEXT_MARGIN    10

typedef struct texture_offset_st {
    const char* frame;
    int         x_offset;
    int         y_offset;
    int         width;
    int         height;
} texture_offset_t;

typedef struct spritesheet_st {
    const char*         name;
    texture_offset_t    offset[TEXTURE_OFFSET_SIZE];
    size_t              offset_count;
    Texture2D           texture;
} spritesheet_t;

VM_API void sheet_get_frame_size(spritesheet_t* sheet, const char* frame, int* out_w, int* out_h);

typedef struct vm_sprite_st {
    float          x, y;
    float          rot;
    spritesheet_t* sheet;
    const char*    frame;
} vm_sprite_t;

typedef size_t vm_obj_type;

#define POOL_SLOT_COUNT    10*1024     // 10K Slot units
#define POOL_DATA_CAPACITY 4*1024*1024 // 4 MB
#define POOL_BLOCK_SIZE    512         // Block can be thought of as a "alloc page size"
#define VM_OBJECT_CAPACITY 1024*10

typedef enum vm_slot_type_enum {
    VM_SLOT_FREE,
    VM_SLOT_SINGLE,
    VM_SLOT_MULTIPLE,
    VM_SLOT_CONT
} vm_slot_type;

typedef struct vm_obj_pool_slot {
    vm_slot_type   type;
    size_t         g_count;  // generation count
    size_t         size;     // slot size in bytes
    size_t         capacity; // Multiple of BLOCK_SIZE
    void*          data;     // pointer to the pool data field. 
} vm_obj_pool_slot_t;

#include <stdint.h>

typedef struct vm_obj_pool_st {
    vm_obj_pool_slot_t slots[POOL_SLOT_COUNT];
    uint8_t            data[POOL_DATA_CAPACITY];
    size_t             slot_count;
    size_t             data_count;
} vm_obj_pool_t;

typedef struct {
    size_t index;
    size_t g_count;
} obj_id;

#define obj_null (obj_id) {.index = -1, .g_count = 0};

typedef void (*vm_render_fn)(obj_id id);
typedef void (*vm_update_fn)(obj_id id);

typedef struct vm_obj_base_st {
    vm_obj_type  type;
    vm_sprite_t  sprite;
    vm_render_fn render_fn;
    vm_update_fn update_fn;
} vm_obj_base_t;

#define OBJ_BASE vm_obj_base_t _Base;

VM_API obj_id vm_obj_alloc(vm_obj_pool_t* pool, size_t size, vm_obj_type type);
VM_API bool   vm_obj_validate_index(vm_obj_pool_slot_t slot, obj_id obj);
VM_API void   vm_obj_init(vm_obj_pool_t* pool, obj_id obj, void* data);
VM_API void*  vm_obj_get(vm_obj_pool_t* pool, obj_id obj);
VM_API void   vm_obj_free(vm_obj_pool_t* pool, obj_id id);
VM_API void   vm_obj_pool_print(vm_obj_pool_t* pool);

typedef struct vm_object_array_st {
    vm_obj_pool_t* pool;
    obj_id         data[VM_OBJECT_CAPACITY];
    size_t         count;
} vm_object_array_t;

VM_API void vm_object_array_add(vm_object_array_t* arr, obj_id obj);
VM_API void vm_object_array_remove(vm_object_array_t* arr, obj_id obj);

VM_API void vm_object_array_add(vm_object_array_t* arr, obj_id obj) {
    
    // find free obj
    for (int i = 0; i < arr->count; ++i) {
        vm_obj_pool_slot_t slot = arr->pool->slots[arr->data[i].index];
        if (slot.type == VM_SLOT_FREE) {
            arr->data[i] = obj;
            return;
        }
    }
    
    assert(arr->count < VM_OBJECT_CAPACITY);
    arr->data[arr->count++] = obj;
}

VM_API void vm_object_array_remove(vm_object_array_t* arr, obj_id obj) {
    vm_obj_free(arr->pool, obj);
    
    // this works because in the pool, the slot associated with obj
    // will be freed (VM_SLOT_FREE)
}

typedef struct vm_renderer_st {
    vm_t*             vm;
    size_t            width;
    size_t            height;
    float             start_time;
    vm_obj_pool_t*    obj_pool;
    vm_object_array_t objects;
    Vector2           mouseWorld;
} vm_renderer_t;

VM_API void          vm_render(vm_t* vm, size_t ww, size_t wh, const char* wtitle);
VM_API spritesheet_t vm_load_spritesheet(const char* path, texture_offset_t* offset, size_t offset_count);
VM_API void          vm_draw_sprite(spritesheet_t* sh, const char* frame, int x, int y, int dw, int dh, float rotation, float anchor_x, float anchor_y, Color tint);
VM_API void          vm_render_init(vm_renderer_t* r);

// private functions
static void MatrixPrint(Matrix m);
static void vm__loop(vm_renderer_t* vm_r);

VM_API vm_renderer_t g_vmRenderer;
VM_API spritesheet_t g_SpriteSheet;
VM_API Camera2D      g_Camera;

#ifdef VM_RENDERER_IMPLEMENTATION

#include "./vm_spritesheet.h"

vm_renderer_t g_vmRenderer = {0};

spritesheet_t g_SpriteSheet;
Camera2D      g_Camera;

static vm_obj_pool_t g_Objpool;

void sheet_get_frame_size(spritesheet_t* sheet, const char* frame, int* out_w, int* out_h) {
    for (int i = 0; i < sheet->offset_count; ++i) {
        if (strcmp(sheet->offset[i].frame, frame) == 0) {
            if (out_w) *out_w = sheet->offset[i].width;
            if (out_h) *out_h = sheet->offset[i].height;
        }
    }
}

void vm_render(vm_t* vm, size_t ww, size_t wh, const char* wtitle) {
    g_vmRenderer.vm     = vm;
    g_vmRenderer.width  = ww;
    g_vmRenderer.height = wh;

    InitWindow(ww, wh, wtitle);
    SetTargetFPS(60);
    
    g_SpriteSheet           = vm_load_spritesheet("./res/icons/spritesheet.png", g_Items_SpriteSheet.offset, g_Items_SpriteSheet.offset_count);
    g_vmRenderer.start_time = GetTime();

    g_Camera = (Camera2D) {
        .offset     = {0, 0},
        .rotation   = 0.0f,
        .target     = {0, 0},
        .zoom       = 1.0f
    };
    g_vmRenderer.obj_pool      = &g_Objpool;
    g_vmRenderer.objects.count = 0;
    g_vmRenderer.objects.pool  = &g_Objpool;

    for (int i = 0; i < g_vmRenderer.obj_pool->slot_count; ++i) {
        g_vmRenderer.obj_pool->slots[i].type = VM_SLOT_FREE;
    }

    vm_render_init(&g_vmRenderer);

    printf("--- Obj Pool status ---\n");
    vm_obj_pool_print(&g_Objpool);

    while (!WindowShouldClose()) {
        BeginDrawing();
            BeginMode2D(g_Camera);

            if (IsKeyDown(KEY_DOWN))  g_Camera.offset.y -= 10.0f;
            if (IsKeyDown(KEY_UP))    g_Camera.offset.y += 10.0f;
            if (IsKeyDown(KEY_RIGHT)) g_Camera.offset.x -= 10.0f;
            if (IsKeyDown(KEY_LEFT))  g_Camera.offset.x += 10.0f;
            
            // ClearBackground((Color) {34, 36, 42, 255});
            ClearBackground((Color) {0, 0, 0, 255});
            vm__loop(&g_vmRenderer);
            EndMode2D();
        EndDrawing();
    }
}

void vm__loop(vm_renderer_t* r) {
    // TODO: Instead of straight up rendering the vm cells
    // Prepare the sprite pool first, and then render them.
    vm_t* vm          = r->vm;
    // view matrix.
    Matrix cam_matrix       = MatrixInvert(GetCameraMatrix2D(g_Camera));
    vm_slot_t* hoveringSlot = NULL;
    Vector2 mouse           = GetMousePosition();
    mouse                   = Vector2Transform(mouse, cam_matrix);

    r->mouseWorld           = mouse;

    for (int i = 0; i < r->objects.count; ++i) {
        obj_id  obj_idx    = r->objects.data[i];

        vm_obj_base_t* obj = (vm_obj_base_t*) vm_obj_get(r->obj_pool, r->objects.data[i]);
        if (!obj) continue;

        if (obj->update_fn != NULL) obj->update_fn(obj_idx);
        if (obj->render_fn != NULL) obj->render_fn(obj_idx);
    }

    // if (hoveringSlot) {
    //     SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    //     if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    //         hoveringSlot->prod_id = 0;
    //     }
    // } else {
    //     SetMouseCursor(MOUSE_CURSOR_ARROW);
    // }
}

VM_API void vm_draw_sprite(
    spritesheet_t* sh, 
    const char* frame, 
    int x, int y, 
    int dw, int dh,
    float rotation, 
    float anchor_x, float anchor_y, Color tint
) {
    texture_offset_t* offset = NULL;

    for (int i = 0; i < sh->offset_count; ++i) {
        if (strcmp(sh->offset[i].frame, frame) == 0) {
            offset = &sh->offset[i];
            break;
        }
    }

    if (!offset) return;

    Rectangle src_rect  = {offset->x_offset, offset->y_offset, offset->width, offset->height};
    Rectangle dest_rect = {x, y, dw, dh};

    DrawTexturePro(sh->texture, src_rect, dest_rect, (Vector2) {dest_rect.width * anchor_x, dest_rect.height * anchor_y}, rotation, tint);
}

VM_API spritesheet_t vm_load_spritesheet(const char* path, texture_offset_t* offset, size_t offset_count) {
    spritesheet_t sh;
    sh.name         = path;
    sh.texture      = LoadTexture(path);
    sh.offset_count = offset_count;

    if (offset) {
        memcpy(sh.offset, offset, offset_count * sizeof(sh.offset[0]));
    }
    
    return sh;
}

void MatrixPrint(Matrix m) {
    float* d = (float*)&m;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            printf("%.3f, ", d[i * 4 + j]);
        }
        printf("\n");
    }
}

VM_API obj_id vm_obj_alloc(vm_obj_pool_t* pool, size_t size, vm_obj_type type) {
    size_t slot_idx = 0;

    while (slot_idx < pool->slot_count) {
        vm_obj_pool_slot_t* s = &pool->slots[slot_idx];
        
        if (s->type != VM_SLOT_FREE) {
            slot_idx++;
            continue;
        }

        if (s->capacity >= size) {
            s->g_count++;
            s->size = size;
            s->type = VM_SLOT_SINGLE;

            return (obj_id) {
                .g_count = s->g_count,
                .index   = slot_idx
            };
        } else {
            size_t slot_idx_tmp = slot_idx;

            vm_obj_pool_slot_t* last = &pool->slots[pool->slot_count - 1];
            vm_obj_pool_slot_t* s_it = s + 1;
            size_t cap = s->capacity;
            int contigous_count = 0;

            while (cap < size && s_it < last && s_it->type == VM_SLOT_FREE) {
                cap += s_it->capacity;
                contigous_count++;
                s_it++;
                slot_idx++;
            }

            if (cap >= size) {
                // allocate object here.
                s->type = VM_SLOT_MULTIPLE;
                s->size = size;
                s->g_count++;

                for (int i = 0; i < contigous_count; ++i)
                    (s + i + 1)->type = VM_SLOT_CONT;

                return (obj_id) {
                    .g_count = s->g_count,
                    .index   = slot_idx_tmp
                };
            } else {
                slot_idx++;
            }
        }
    }

    if (slot_idx >= pool->slot_count && pool->slot_count < POOL_SLOT_COUNT) {
        // allocate new slot
        size_t tmp_slot_count = pool->slot_count;
        vm_obj_pool_slot_t* slot = &pool->slots[pool->slot_count++];
        size_t alloc_cap = POOL_BLOCK_SIZE;
        int cont_count   = 0;
        while (alloc_cap < size && pool->slot_count < POOL_SLOT_COUNT) {
            vm_obj_pool_slot_t* _s = &pool->slots[pool->slot_count++];
            alloc_cap += POOL_BLOCK_SIZE;
            cont_count++;
        }

        if (alloc_cap < size) {
            pool->slot_count = tmp_slot_count;
            return (obj_id) {
                .index = -1,
                .g_count = 0
            };
        } else {
            size_t dc         = pool->data_count;

            slot->type        = cont_count > 0 ? VM_SLOT_MULTIPLE : VM_SLOT_SINGLE;
            slot->size        = size;
            slot->capacity    = POOL_BLOCK_SIZE;
            slot->g_count     = 0;
            slot->data        = pool->data + pool->data_count;
            pool->data_count += POOL_BLOCK_SIZE;

            if ((uint8_t*)slot->data >= pool->data + POOL_DATA_CAPACITY) {
                pool->slot_count = tmp_slot_count;
                pool->data_count = dc;

                return (obj_id) {
                    .g_count = 0,
                    .index   = -1
                };
            }
            
            for (int i = 0; i < cont_count; ++i) {
                vm_obj_pool_slot_t* next = (slot + i + 1);
                next->type     = VM_SLOT_CONT;
                next->capacity = POOL_BLOCK_SIZE;
                next->g_count  = 0;
            }

            return (obj_id) {
                .g_count = slot->g_count,
                .index   = tmp_slot_count
            };
        }
    } 
}

VM_API bool vm_obj_validate_index(vm_obj_pool_slot_t slot, obj_id obj) {
    if (slot.type == VM_SLOT_FREE) return false;
    if (slot.g_count != obj.g_count) return false;
    return true;
}

VM_API void vm_obj_init(vm_obj_pool_t* pool, obj_id obj, void* data) {
    vm_obj_pool_slot_t slot = pool->slots[obj.index];
    if (!vm_obj_validate_index(slot, obj)) return;

    assert(slot.data != NULL);
    memcpy(slot.data, data, slot.size);
}

VM_API void*  vm_obj_get(vm_obj_pool_t* pool, obj_id obj) {
    if (!pool) return NULL;
    vm_obj_pool_slot_t slot = pool->slots[obj.index];
    if (!vm_obj_validate_index(slot, obj)) return NULL;

    return slot.data;
}

VM_API void vm_obj_free(vm_obj_pool_t* pool, obj_id id) {
    vm_obj_pool_slot_t* slot     = &pool->slots[id.index];
    vm_obj_pool_slot_t* slot_end = &pool->slots[POOL_SLOT_COUNT];
    if (!vm_obj_validate_index(*slot, id)) return;

    if (slot->type == VM_SLOT_SINGLE) {
        slot->type = VM_SLOT_FREE;
    } else {
        assert(slot->type == VM_SLOT_MULTIPLE);
        slot->type = VM_SLOT_FREE;
        slot++;

        while (slot != slot_end && slot->type == VM_SLOT_CONT) {
            slot->type = VM_SLOT_FREE;
            slot++;
        }
    }
}

VM_API void   vm_obj_pool_print(vm_obj_pool_t* pool) {
    int free_slots = 0;
    for (int i = 0; i < pool->slot_count; ++i) {
        if (pool->slots[i].type == VM_SLOT_FREE) {
            free_slots++;
        }
    }

    printf("Allocated slots: %d/%d\n", pool->slot_count, POOL_SLOT_COUNT);
    printf("Free slots: %d/%d\n", free_slots, pool->slot_count);
    printf("Allocated bytes: %d/%d\n", pool->data_count, POOL_DATA_CAPACITY);
}

#endif