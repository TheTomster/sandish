#include "vec4.h"

void vec4_to_translation_matrix(GLfloat *r, vec4 v) {
  for (int i = 0; i < 16; i++) {
    switch (i) {
      case 0:
      case 5:
      case 10:
      case 15:
        r[i] = 1;
        break;
      case 12:
        r[i] = v.x;
        break;
      case 13:
        r[i] = v.y;
        break;
      case 14:
        r[i] = v.z;
        break;
      default:
        r[i] = 0;
        break;
    }
  }
}

vec4 vec4_mat_mult(GLfloat * m, vec4 v) {
  //row + col*numrows
  vec4 r;
  r.x = v.x * m[0 + 0 * 4] + v.y * m[0 + 1 * 4] + v.z * m[0 + 2 * 4] + v.w * m[0 + 3 * 4];
  r.y = v.x * m[1 + 0 * 4] + v.y * m[1 + 1 * 4] + v.z * m[1 + 2 * 4] + v.w * m[1 + 3 * 4];
  r.z = v.x * m[2 + 0 * 4] + v.y * m[2 + 1 * 4] + v.z * m[2 + 2 * 4] + v.w * m[2 + 3 * 4];
  r.w = v.x * m[3 + 0 * 4] + v.y * m[3 + 1 * 4] + v.z * m[3 + 2 * 4] + v.w * m[3 + 3 * 4];
  return r;
}
