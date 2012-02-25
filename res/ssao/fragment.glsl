#version 150

uniform sampler2D geo_samp;
uniform sampler2D pos_samp;
uniform sampler2D norm_samp;

const float RADIUS = 2;

void main() {
  vec2 size = textureSize(geo_samp, 0);
  float u = gl_FragCoord.x / size.x;
  float v = gl_FragCoord.y / size.y;
  vec2 uv = vec2(u, v);

  // Color from geometric render
  gl_FragColor.rgb = texture(geo_samp, uv).rgb;

  vec3 p = texture(pos_samp, uv).rgb;
  //gl_FragColor.rgb = p;
  vec3 n = texture(norm_samp, uv).rgb;
  //gl_FragColor.rgb = n;

  // Don't do ssao on fragments that aren't part of the actual scene
  if (p == vec3(0, 0, 0)) {
    return;
  }

  //SSAO calcs

  const vec2 sample_vectors[8] = vec2[8](
    vec2(1, 0),
    vec2(-1, 0),
    vec2(0, 1),
    vec2(0, -1),
    vec2(0.707, 0.707),
    vec2(-0.707, 0.707),
    vec2(0.707, 0.707),
    vec2(0.707, -0.707)
  );

  vec3 ao = vec3(0, 0, 0);

  for (int i = 0; i < 8; i++) {
    float r = RADIUS / size.x / (1 - p.z);
    vec2 coord = uv + sample_vectors[i] * r;
    vec3 p2 = texture(pos_samp, coord).rgb;
    float o = p2.z - 0.001 > p.z ? 1.0 : 0.0;;
    if (distance(p2.z, p.z) > r) {
      o = 0.0;
    }
    ao += o;
  }
  ao /= 8;

  //gl_FragColor.rgb = 1 - ao;
  gl_FragColor.rgb = gl_FragColor.rgb * (1 - ao);
}
