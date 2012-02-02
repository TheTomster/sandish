// cam holds information about the camera

#pragma once

#include "vec4.h"

typedef struct camera * cam_handle;

cam_handle cam_new(vec4 pos, vec4 rotation);

void cam_delete(cam_handle c);

vec4 cam_get_pos(cam_handle c);

void cam_set_pos(cam_handle c, vec4 newpos);
