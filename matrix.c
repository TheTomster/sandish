#include <GL3/gl3w.h>
#include <GL/glfw.h>

#include "matrix.h"

static GLfloat identity[16] = {
  1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 1, 0,
  0, 0, 0, 1
};

// Multiplies two 4x4 matrices and stores the result in r.
void matrix_multiply(GLfloat *r, GLfloat *a, GLfloat *b) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int rindex = i + j * 4;
      r[rindex] = 0;
      for (int x = 0; x < 4; x++) {
        int aindex = i + x * 4;
        int bindex = x + j * 4;
        r[rindex] += a[aindex] * b[bindex];
      }
    }
  }
}

void matrix_invert(GLfloat *r, GLfloat *m) {
  GLfloat t[16];
  for (int i = 0; i < 16; i++) {
    r[i] = identity[i];
    t[i] = m[i];
  }
  for (int pivot_index = 0; pivot_index < 4; pivot_index++) {
    GLfloat pivot_quot = t[pivot_index + pivot_index * 4];
    for (int i = 0; i < 4; i ++) {
      // for each column in the pivot row, divide by the pivot quotient
      t[pivot_index + i * 4] = t[pivot_index + i * 4] / pivot_quot;
      r[pivot_index + i * 4] = r[pivot_index + i * 4] / pivot_quot;
    }
    for (int row = 0; row < 4; row ++) {
      if (row == pivot_index)
        continue;
      // for each row in the pivot column, perform a row operation to make it 0
      GLfloat coeff = t[row + pivot_index * 4];
      for (int col = 0; col < 4; col++) {
        t[row + col * 4] = t[row + col * 4] - coeff * t[pivot_index + col * 4];
        r[row + col * 4] = r[row + col * 4] - coeff * r[pivot_index + col * 4];
      }
    }
  }
}
