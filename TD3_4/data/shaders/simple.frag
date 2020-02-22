#version 330 core

uniform float white_filled;
uniform float intensity;
uniform float exponent;
uniform vec3 vtx_spec;

in vec3 var_color;
in vec3 var_view;
in vec3 var_normal;
in vec3 var_light;

out vec4 out_color;

vec3 blinn(vec3 norm, vec3 view, vec3 light, vec3 diffuse, vec3 spec, float exp)
{
  float theta = dot(norm, light);
  if(theta < 0){
    theta = 0;
  }

  float beta = dot(norm, normalize(view - light));
  if(beta < 0){
    beta = 0;
  }
  beta = pow(beta, exp);

  vec3 ps = spec*beta;
  return ps + diffuse;
}

void main(void) 
{
  vec3 blinn = blinn(var_normal, var_view, var_light, var_color, vtx_spec, exponent);
  float I = intensity * dot(var_normal, -var_light);

  out_color = I * vec4(blinn, 1);

  if (white_filled == 1){
      out_color = vec4(255, 255, 255, 1);
  }

}
