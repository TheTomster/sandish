#version 150

in frag {
  float depth;
} i;

void main() {
  float intensity = smoothstep(-9.5, -4.5, i.depth);
  gl_FragColor = vec4(intensity, intensity, 0, 1.0f);
}
