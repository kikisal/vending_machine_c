#pragma once

#include "./../vm_renderer.h"
#include "./obj_types.h"

obj_id DefaultObj_init(float x, float y, const char* frame);
void   DefaultObj_render(obj_id obj);
void   DefaultObj_update(obj_id obj);

#ifdef DEFAULT_OBJ_IMPL

static float vel = 0.0f;
static float acc = 300.0f;

obj_id DefaultObj_init(float x, float y, const char* frame) {
    vm_renderer_t* r = &g_vmRenderer;

    obj_id id          = vm_obj_alloc(r->obj_pool, sizeof(vm_obj_base_t), OBJ_DEFAULT);
    vm_obj_base_t* obj = vm_obj_get(r->obj_pool, id);
    if (obj == NULL) return obj_null;

    obj->sprite = (vm_sprite_t) {
        .x     = x,
        .y     = y,
        .frame = frame,
        .sheet = &g_SpriteSheet,
        .rot   = 0 
    };

    obj->type      = OBJ_DEFAULT;
    obj->update_fn = DefaultObj_render;
    obj->render_fn = DefaultObj_update;

    return id;
}

void DefaultObj_render(obj_id obj) {
    vm_renderer_t* r    = &g_vmRenderer;
    vm_obj_base_t* base = vm_obj_get(r->obj_pool, obj);
    if (!base) return;

    int frame_width = 0, frame_height = 0;

    sheet_get_frame_size(&g_SpriteSheet, base->sprite.frame, &frame_width, &frame_height);

    vm_draw_sprite(&g_SpriteSheet, base->sprite.frame, 
        base->sprite.x,
        base->sprite.y,
        frame_width * 2, frame_height * 2, base->sprite.rot,
        0.5f, 0.5f, (Color) WHITE
    );
}

void DefaultObj_update(obj_id obj) {
    vm_renderer_t* r    = &g_vmRenderer;
    vm_obj_base_t* base = vm_obj_get(r->obj_pool, obj);
    if (!base) return;

    vel += acc * GetFrameTime();
    base->sprite.y += vel * GetFrameTime();
}
#endif