#pragma once

#include "./../vm_renderer.h"
#include "./obj_types.h"

typedef struct ObjSceneControl_st {
    OBJ_BASE;
    vm_slot_t* hoveringSlot;
} ObjSceneControl_t;

extern obj_id g_SceneControlObj;

obj_id  ObjSceneControl_init();
void    ObjSceneControl_update(obj_id obj);
// void    ObjSceneControl_set_hoveringSlot(obj_id obj, vm_slot_t* slot);

#ifdef OBJ_SCENE_CONTROL_IMPL

obj_id g_SceneControlObj;

obj_id ObjSceneControl_init() {
    vm_renderer_t* r = &g_vmRenderer;

    obj_id scene_id              = vm_obj_alloc(r->obj_pool, sizeof(ObjSceneControl_t), OBJ_SCENE_CONTROL);
    ObjSceneControl_t* scene_obj = vm_obj_get(r->obj_pool, scene_id);
    if (scene_obj == NULL) return obj_null;

    scene_obj->_Base.type        = OBJ_SCENE_CONTROL;
    scene_obj->_Base.update_fn   = ObjSceneControl_update;
    scene_obj->_Base.render_fn   = NULL;
    scene_obj->hoveringSlot      = NULL;

    return scene_id;
}

void ObjSceneControl_update(obj_id oid) {
    ObjSceneControl_t* obj = vm_obj_get(g_vmRenderer.obj_pool, oid);
    if (!obj) return;

    if (obj->hoveringSlot) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            obj->hoveringSlot->prod_id = 0;
        }
    } else {
        SetMouseCursor(MOUSE_CURSOR_ARROW);
    }
}

#endif