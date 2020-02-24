#version 330 core

uniform float white_filled;

in vec3 var_color;
in vec3 var_view;
in vec3 var_normal;
in vec3 var_light;

out vec4 out_color;

vec3 blinn(vec3 norm, vec3 view, vec3 light, vec3 diffuse, vec3 spec, float exp)
{
  float theta = max(dot(norm, light), 0);
  float beta = max(dot(norm, normalize(view + light)), 0);
  beta = pow(beta, exp);

  vec3 ps = spec*beta;
  vec3 pd = diffuse*theta;
  return ps + pd;
}

void main(void) 
{
  vec3 vtx_spec = vec3(1.0, 1.0, 1.0);
  float exponent = 25.0;
  float intensity = 1.0;

  vec3 blinn = blinn(var_normal, var_view, -var_light, var_color, vtx_spec, exponent);
  float I = intensity * dot(var_normal, -var_light);

  out_color = I * vec4(blinn, 1);

  if (white_filled == 1){
      out_color = vec4(255, 255, 255, 1);
  }

}
