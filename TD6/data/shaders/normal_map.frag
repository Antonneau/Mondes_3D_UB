#version 330 core

in vec3 v_color;
in vec3 v_normal;
in vec3 v_view;
in vec2 v_texcoord;
in vec3 v_tangent;
in vec3 v_bitangent;
in mat3 m_normal;

uniform vec3 lightDir;
uniform sampler2D tex2D;
uniform sampler2D tex2D_2;

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

  vec3 normal_map = normalize(m_normal * (texture(tex2D, v_texcoord).xyz*2-1));
  vec3 v_t = normalize(v_tangent);
  vec3 v_b = normalize(v_bitangent);
  vec3 v_n = normalize(v_normal);
  vec3 c1 = vec3(v_t.x, v_b.x, v_n.x);
  vec3 c2 = vec3(v_t.y, v_b.y, v_n.y);
  vec3 c3 = vec3(v_t.z, v_b.z, v_n.z);
  mat3 tbnv_matrix = mat3(c1, c2, c3);

  out_color = vec4(ambient * v_color + blinn(normal_map, tbnv_matrix*normalize(v_view), tbnv_matrix*lightDir, v_color, spec_color, shininess),1.0);
  //out_color = vec4(v_t, 1.);

}
