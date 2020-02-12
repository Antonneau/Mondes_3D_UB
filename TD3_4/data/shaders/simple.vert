#version 330 core

uniform float views;
uniform mat4 mat_obj;
uniform mat4 mat_cam;
uniform mat4 mat_persp;

in vec3 vtx_position;
in vec3 vtx_color;

out vec3 var_color;

void main()
{
  
  gl_Position = mat_persp * (mat_cam * mat_obj * vec4(vtx_position, 1.));
  
  var_color = vtx_color;
}
