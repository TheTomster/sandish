in frag {
  float rand;
} i;

void main() {
  float intensity = 0.25 + (0.5 * i.rand);
  gl_FragData[0] = vec4(intensity, intensity, intensity, 1.0f);
  pos_color();
  norm_color();
}
