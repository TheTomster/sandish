in frag {
  float rand;
} i;

void main() {
  gl_FragData[0] = vec4(1.0, 0.0, 0.0, 1.0);
  pos_color();
  norm_color();
}
