in frag {
  float rand;
} i;

void main() {
  float intensity = 0.6 + (0.4 * i.rand);
  gl_FragData[0] = vec4(0, 0, intensity, 1.0f);
  pos_color();
  norm_color();
}
