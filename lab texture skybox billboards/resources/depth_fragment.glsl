#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

//uniform sampler2D tex;
//out vec4 color;
  
//in vec2 TexCoords;

uniform sampler2D tex;

void main()
{             
    float depthValue = texture(tex, vertex_tex).r;
    color = vec4(vec3(depthValue), 1.0);
}  
