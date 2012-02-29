#version 150

uniform sampler2D geo_samp;
uniform sampler2D pos_samp;
uniform sampler2D norm_samp;

const float RADIUS = 5;

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

  float a = uv.x + n.y;
  float b = n.x + uv.y;
  float angle = fract(sin(dot(vec2(a, b), vec2(12.9898, 78.233))) * 43758.5453) * 3.14;

  for (int i = 0; i < 8; i++) {
    vec2 v = sample_vectors[i];
    vec2 rotated;
    // uncomment to disable noise
    //float angle = 3.14159 / 2;
    rotated.x = v.x * cos(angle) - v.y * sin(angle);
    rotated.y = v.x * sin(angle) + v.y * cos(angle);
    float r = RADIUS / size.x / (1 - p.z);
    vec2 coord = uv + rotated * r;
    vec3 p2 = texture(pos_samp, coord).rgb;
    vec3 n2 = texture(norm_samp, coord).rgb;
    float o = p2.z - 0.001 > p.z ? 1.0 : 0.0;;
    if (distance(p2.z, p.z) > r) {
      o = 0.0;
    }
    if (n == n2) {
      // Uncomment to remove occlusion when the normal matches
      //o = 0.0;
    }
    ao += o;
  }
  ao /= 8;

  //gl_FragColor.rgb = 1 - ao;
  gl_FragColor.rgb = gl_FragColor.rgb * (1 - ao);
}
