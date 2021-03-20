#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;
uniform vec2 texoff;

//uniform sampler2D tex;
//out vec4 color;
  
//in vec2 TexCoords;

uniform sampler2D tex;

void main()
{             
    vec2 tc = vec2(vertex_tex.x / 5. + (texoff.x * 0.2), vertex_tex.y / 5. + (texoff.y * 0.2));
    vec4 tcol = texture(tex, tc);
    color = tcol;
}  
