#version 330 core

uniform float white_filled;
in vec3 var_color;

out vec4 out_color;

void main(void) {
    out_color = vec4(var_color, 1);
    if (white_filled == 1){
        out_color = vec4(255, 255, 255, 1);
    }
}
