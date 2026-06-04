#pragma once

#include "./../vm_renderer.h"
#include "./scene_control.h"

typedef struct {
    OBJ_BASE;
    float      x, y;
    float      rot;
    float      phase;
    vm_t*      vm;
    vm_slot_t* vm_slot;
} ObjSlot_t;

obj_id ObjSlot_init(float x, float y, 
    float rot, vm_t* vm, vm_sprite_t sprite, 
    float phase, vm_slot_t* slot
);
void ObjSlot_render(obj_id obj);
void ObjSlot_update(obj_id obj);


#ifdef SLOT_OBJ_IMPL


obj_id ObjSlot_init(float x, float y, float rot, 
    vm_t* vm, vm_sprite_t sprite, 
    float phase, vm_slot_t* slot) {
    obj_id slot_obj = vm_obj_alloc(&g_Objpool, sizeof(ObjSlot_t), OBJ_SLOT);

    ObjSlot_t obj;
    obj.x               = x;
    obj.y               = y;
    obj.rot             = 0.0f;
    obj.vm              = vm;
    obj._Base.type      = OBJ_SLOT;
    obj._Base.sprite    = sprite;
    obj._Base.update_fn = ObjSlot_update;
    obj._Base.render_fn = ObjSlot_render;

    obj.phase           = 16.0f * ((float)rand() / (float)RAND_MAX);
    obj.vm_slot         = slot;
    vm_obj_init(&g_Objpool, slot_obj, &obj);
    return slot_obj;
}


// user defined object functions //
void ObjSlot_render(obj_id obj) {
    vm_renderer_t* r    = &g_vmRenderer;
    vm_obj_base_t* base = (vm_obj_base_t*) vm_obj_get(r->obj_pool, obj);

    if (!base) return;
    
    ObjSlot_t* slot_obj = (ObjSlot_t*) base;
    vm_slot_t* slot     = slot_obj->vm_slot;
    product_t* product  = vm_find_product(slot_obj->vm, slot->prod_id);
    if (!product) return;
    int qnt = slot->qnt;
    if (qnt > 4)
        qnt = 4;

    unsigned char tint = 255;
    int sprite_x = base->sprite.x;
    int sprite_y = base->sprite.y;

    for (int k = qnt - 1; k >= 0; --k) {
        float alpha_f = (1.0f - (float) k / (float)slot->qnt);
        unsigned char alpha = alpha_f * 200;

        float sprite_slot_x = sprite_x + k * 10.0f; 
        float sprite_slot_y = sprite_y - k * 10.0f;

        vm_draw_sprite(&g_SpriteSheet, product->display_name, 
            sprite_slot_x,
            sprite_slot_y,
            GRID_SIZE, GRID_SIZE, base->sprite.rot + 8.0f * k,
            0.5f, 0.5f, (Color) {.r = tint * alpha_f, .g = tint * alpha_f, .b = tint * alpha_f, .a = alpha}
        );
    }

    size_t fontSize  = 16;
    size_t textWidth = MeasureText(product->display_name, fontSize);

    DrawText(
        product->display_name,
        base->sprite.x - textWidth / 2,
        base->sprite.y + GRID_SIZE / 2.0f + GRID_TEXT_MARGIN,
        fontSize, WHITE
    );
}

void ObjSlot_update(obj_id obj) {
    vm_renderer_t* r    = &g_vmRenderer;
    vm_obj_base_t* base = (vm_obj_base_t*) vm_obj_get(r->obj_pool, obj);
    if (!base) return;
    
    ObjSlot_t* slot_obj = (ObjSlot_t*) base;
    vm_slot_t* slot     = slot_obj->vm_slot;
    Vector2 mouse       = r->mouseWorld;

    ObjSceneControl_t* scene = vm_obj_get(r->obj_pool, g_SceneControlObj);
    if (scene) {
        if (fabsf(mouse.x - base->sprite.x) < GRID_SIZE / 2.0f && fabsf(mouse.y - base->sprite.y) < GRID_SIZE / 2.0f) {
            scene->hoveringSlot = slot;
            slot_obj->rot = 10.0f;
        } else {
            slot_obj->rot = 0.0f;
        }
    }

    float t           = GetTime();
    float duration    = 2.0f;

    float y   = 26.0f * sin(t * 2.0f * PI / 1.0f + slot_obj->phase);
    float rot = 16.0f * cos(t * 2 * PI * 2.1f);
    float _t  = t - r->start_time;
    y         = y   * (1 - (_t/duration));
    rot       = rot * (1 - (_t/duration));
    
    if (t - r->start_time > duration) {
        y   = 0;
        rot = 0.0f;
        // r->start_time = t;
    }

    base->sprite.rot = rot + slot_obj->rot;
    base->sprite.y   = slot_obj->y + y;
}
#endif