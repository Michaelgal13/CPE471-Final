#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform mat4 ligV;
uniform mat4 ligP;
uniform vec3 vertoff;

out vec3 vertex_pos;
out vec3 vertex_normal;
out vec2 vertex_tex;

out vec4 ligSpace;

void main()
{
	vertex_normal = vec4(M * vec4(vertNor,0.0)).xyz;

	vec4 tpos =  vec4(vertPos, 1.0);
	tpos.x += vertoff.x / 1000.0f;
	tpos.y += vertoff.z / 1000.0f;
	tpos =  M *tpos;
	vertex_pos = tpos.xyz;
	ligSpace = ligP * ligV * tpos;
	gl_Position = P * V * tpos;
	vertex_tex = vertTex;
}
