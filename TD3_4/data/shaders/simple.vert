#version 330 core

uniform float views;

uniform mat4 mat_obj;
uniform mat4 mat_cam;
uniform mat4 mat_persp;
uniform mat3 mat_normal;

in vec3 vtx_position;
in vec3 vtx_color;
in vec3 vtx_normal;

out vec3 var_color;
out vec3 var_view;
out vec3 var_normal;
out vec3 var_light;

void main()
{
  vec4 P = mat_cam * mat_obj * vec4(vtx_position, 1.);
  var_view = -normalize(P.xyz);
  var_normal = normalize(mat_normal * vtx_normal);
  vec4 light = mat_cam * vec4(0.0, -5.0, -5.0, 0.0);
  var_light = normalize(light.xyz);
  var_color = vtx_color;

  gl_Position = mat_persp * P;
}
