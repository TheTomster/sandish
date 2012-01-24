#version 150

in fragment {
  flat uint board_data;
} i;

void main() {
  if (i.board_data == 0) {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
  } else {
    gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
  }
}
