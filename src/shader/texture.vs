#version 440

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex_coord;

out vec2 vertex_tex_coord;

void main(){
     gl_Position = vec4(pos, 0.f, 1.f);
     vertex_tex_coord = vec2(tex_coord.x, tex_coord.y);
}