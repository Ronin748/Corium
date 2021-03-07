#version 330 core

in vec3 fPos;
in vec3 fNorm;
in vec2 fTex;
in vec4 fCol;

layout(location = 0) out vec4 fragCol;

#define MAX_LIGHTS 8

uniform sampler2D diffuseTexture; // texture 0

uniform struct Light {
    vec3 pos;
    vec3 col;
    samplerCube depthMap;
} lights[MAX_LIGHTS];

uniform samplerCube depthMap[MAX_LIGHTS];

uniform float farPlane;

uniform int lightNum;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 fogCol;

uniform vec2 al; // ambient, lit

float lightAngle = 1.0f;

float shadowCheck(vec3 fPos, int light_i)
{
    vec3 lightDist = fPos - lights[light_i].pos;

    float closest = texture(lights[light_i].depthMap, lightDist).r;

    float c = 0.015f;

    float current = length(lightDist);
    float bias = 0.1f+0.2f*tan(acos(min(lightAngle, 1.0f)));

    closest *= farPlane;

    float shadow = current -  bias > closest ? 1.0: 0.0;

    return shadow;
}

void main()
{
    float amb = al.x;
    float lit = al.y; // a + l = 1.0f

    vec3 color = texture(diffuseTexture, fTex).rgb * fCol.rgb;
    vec3 normal = normalize(fNorm);
    vec3 lighting = vec3(0.0f);
    vec3 bloomLighting = vec3(0.0f);

    vec3 viewPos = -inverse(view)[3].xyz;

    // ambient colour
    vec3 ambient = amb * color;

    float shadow = 0.0f;

    for (int l = 0; l < lightNum; ++l) {
        if (length(lights[l].col) > 0.0f) {
            vec3 lightColor = lights[l].col;
            // DIFFUSE
            vec3 lightDir = normalize(lights[l].pos - fPos);
            float diff = max(dot(lightDir, normal), 0.0);
            vec3 diffuse = diff * lightColor;
            // SPEC
            vec3 viewDir = normalize(-viewPos - fPos);
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = 0.0;
            vec3 halfwayDir = normalize(lightDir + viewDir);
            spec = pow(max(dot(normal, halfwayDir), 0.0), 32);
            vec3 specular = spec * lightColor;

            lightAngle = abs(dot(lightDir, normal));
            // SHADOW CHECK
            shadow = shadowCheck(fPos, l);

            // distance fading, inverse square
            vec3 fadeDist = (fPos - lights[l].pos) / length(lightColor);
            float fade = length(fadeDist) * length(fadeDist) * 0.001f;

            lighting += (1.0 - min(
								   shadow + (1.0f - min(lightAngle, 1.0f)),
								   1.0f)) *
				(diffuse + specular) * color / max(fade, 1.0f);
        }
    }

    float depth = length(viewPos + fPos) / farPlane;

    vec3 sigma = ambient;
    sigma += lighting * lit;
    fragCol = vec4(sigma, fCol.a);

    //downsample color
    float s = 16.0f;
    fragCol.r = floor(fragCol.r * s) / s;
    fragCol.g = floor(fragCol.g * s) / s;
    fragCol.b = floor(fragCol.b * s) / s;
	
    float fog = max(min(
					   ((1.0f - pow(2.7f, -depth - 0.1f)) * 1.67f - 0.2f) * 1.2f,
					   1.0f), 0.0f);
    fragCol.rgb = fogCol * fog + fragCol.rgb * (1.0f - fog); // FOG

    if (texture(diffuseTexture, fTex).a == 0.0f)
        discard;
}
