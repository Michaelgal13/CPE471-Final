#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

uniform sampler2D tex;
uniform sampler2D tex2;
uniform sampler2D depthMap;
uniform vec3 ligpos;
uniform float texoff;



in vec4 ligSpace;
uniform float dn;

float bias;

float ShadowCalculation(vec4 ligSpace)
{
    // perform perspective divide
    vec3 projCoords = ligSpace.xyz / ligSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow


    
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  
   // float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
   if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

void main()
{
vec3 lp = vec3(0,50,0);

vec3 normalcolor = texture(tex, vertex_tex*500. + vec2(texoff*0.4, -texoff*1.8)).rgb;
vec3 texnormal = normalize(normalcolor - vec3(0.5, 0.5, 0.5));
texnormal = vec3(texnormal.x, texnormal.y, texnormal.z);

vec3 normalcolor2 = texture(tex2, vertex_tex*500. + vec2(-texoff *1.2, texoff*1.7)).rgb;
vec3 texnormal2 = normalize(normalcolor2 - vec3(0.5, 0.5, 0.5));
texnormal2 = vec3(texnormal2.x, texnormal2.y, texnormal2.z);

vec3 ld = normalize(ligpos - vertex_pos);
vec3 n = normalize(texnormal2 + texnormal);
float diff = dot(n, ld);
vec3 cd = normalize(campos - vertex_pos);
vec3 h = normalize(cd - ld);

float spec = dot(n, h);
spec = pow(spec,5);
spec = clamp(spec, 0, 1);
bias = 0.001;  
float shadow = ShadowCalculation(ligSpace);
float ambient = 0.1;
vec3 color3 = vec3(0, 0.2, 1);

vec3 lighting = ((ambient*0.5) + (1.0 - shadow) * (diff*0.5 + spec+ ambient * 0.5)) * color3;    
    
color = vec4(lighting, 1.0);
color.a = 1;



}
