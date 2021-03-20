
#version 330 core
out vec4 FragColor;

/*in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;*/

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

in vec4 ligSpace;

uniform sampler2D tex1;
uniform sampler2D depthMap;

uniform vec3 ligpos;
uniform vec3 campos;
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
    vec2 tc = vec2(vertex_tex);
    vec3 color = texture(tex1, tc*16).rgb;
    vec3 normal = normalize(vertex_normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(ligpos - vertex_pos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(campos - vertex_pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    bias = 0.005;  
    float shadow = ShadowCalculation(ligSpace);                      
    vec3 lighting = (ambient * 0.5 + (1.0 - shadow) * (diffuse + specular + ambient * 0.5)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}

/*#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;
uniform vec3 ligpos;

uniform sampler2D tex;
void main()
{
vec4 tcol = texture(tex, vertex_tex*10.);
color = tcol;
}

*/