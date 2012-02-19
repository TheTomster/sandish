in frag {
  float rand;
} i;

void main() {
  float intensity = 0.7 + (0.1 * i.rand);
  gl_FragData[0] = vec4(intensity, intensity, 0, 1.0f);
  pos_color();
  norm_color();
}
