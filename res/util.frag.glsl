#version 150

in fragutil {
  smooth vec4 pos;
  flat vec4 normal;
} ifu;

void pos_color() {
  vec4 retc = vec4(0, 0, 0, 1);
  retc.xy = smoothstep(-3, 3, ifu.pos.xy);
  retc.z = 1 - ifu.pos.z / 15;
  gl_FragData[1].rgb = retc.rgb;
}

void norm_color() {
  vec4 retc = ifu.normal * 0.5 + 0.5;
  gl_FragData[2] = retc;
}
