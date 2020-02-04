#version 330 core

uniform float zoom;
uniform vec2 offset;

in vec3 vtx_position;
in vec3 vtx_color;

out vec3 var_color;

void main()
{
  gl_Position = vec4(vtx_position.x*zoom + offset.x,
                     vtx_position.y*zoom + offset.y,
                     -vtx_position.z, 
                     1.);
  var_color = vtx_color;
}
