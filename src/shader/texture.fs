#version 440

out vec4 frag_color;
in vec2 vertex_tex_coord;
 
uniform sampler2D tex;

void main(){
     float r = texture(tex, vertex_tex_coord).r;
     //gl_FragColor = vec4(r, r, r, 1.f);
     frag_color = vec4(r, r, r, 1.f);
}