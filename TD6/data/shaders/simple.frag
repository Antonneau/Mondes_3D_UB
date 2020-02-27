#version 330 core

in vec3 v_color;
in vec3 v_normal;
in vec3 v_view;
in vec2 v_texcoord;

uniform vec3 lightDir;
uniform sampler2D tex2D;

out vec4 out_color;

vec3 blinn(vec3 n, vec3 v, vec3 l, vec3 dCol, vec3 sCol, float s)
{
  vec3 res = vec3(0,0,0);
  float dc = max(0,dot(n,l));
  if(dc>0) {
    res = dCol * dc;
    float sc = max(0,dot(n,normalize(v+l)));
    if(sc>0)
      res += sCol * pow(sc,s) * dc;
  }
  return res;
}

void main(void) {
  float ambient = 0.2;
  float shininess = 50;
  vec3 spec_color = vec3(1,1,1);
  float theta = max(dot(normalize(v_normal), lightDir), 0);
  vec4 tex2D_color = texture(tex2D, v_texcoord);
  out_color =  tex2D_color * theta;
  
  //out_color = vec4(ambient * v_color + blinn(normalize(v_normal),normalize(v_view), lightDir, v_color, spec_color, shininess),1.0);
}
