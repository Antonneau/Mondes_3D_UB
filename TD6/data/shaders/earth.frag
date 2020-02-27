#version 330 core

in vec3 v_color;
in vec3 v_normal;
in vec3 v_view;
in vec2 v_texcoord;

uniform vec3 lightDir;
// Earth
uniform sampler2D tex2D;
// Clouds
uniform sampler2D tex2D_2;
// Night
uniform sampler2D tex2D_3;

out vec4 out_color;

void main(void) {
  float theta = max(dot(normalize(v_normal), lightDir), 0);
  vec4 tex2D_color = texture(tex2D, v_texcoord);
  vec4 tex2D_2_color = texture(tex2D_2, v_texcoord);
  vec4 tex2D_3_color = texture(tex2D_3, v_texcoord);
  vec4 day_color = mix(tex2D_color, tex2D_2_color, tex2D_2_color);
  vec4 night_color = mix(tex2D_3_color, tex2D_2_color, tex2D_2_color * theta-0.1);
  out_color =  mix(night_color, day_color, theta);
}
