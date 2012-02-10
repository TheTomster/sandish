// cam holds information about the camera

#pragma once

#include "vec4.h"

typedef struct camera * cam_handle;

cam_handle cam_new(vec4 pos, GLfloat rx, GLfloat ry);

void cam_delete(cam_handle c);

vec4 cam_get_pos(cam_handle c);

void cam_set_pos(cam_handle c, vec4 newpos);

GLfloat cam_get_rx(cam_handle c);

GLfloat cam_get_ry(cam_handle c);

void cam_get_rot_mat(GLfloat * r, cam_handle c);

// Returns a newly allocated matrix containing the position and orientation
// transformations for the camera.
void cam_get_matrix(GLfloat * r, cam_handle c);

// The angle should be in radians.
void cam_rotate(cam_handle c, GLfloat dx, GLfloat dy);
