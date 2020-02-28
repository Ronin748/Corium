#version 150

precision lowp int;
precision lowp float;

in vec3 fPos;
in vec3 fNorm;
in vec2 fTex;
in vec4 fCol;

//layout(location = 0) out vec4 gl_FragColor;

#define MAX_LIGHTS CORIUM_LIGHTNUM

uniform sampler2D diffuseTexture; // texture 0

uniform struct Light {
    vec3 pos;
    vec3 col;
  //  samplerCube depthMap;
} lights[MAX_LIGHTS];

uniform samplerCube depthMap[MAX_LIGHTS];

uniform float farPlane;

uniform int max_lights;

uniform vec2 ws; // window size

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 fogCol;

uniform vec2 al;

uniform float dither;

float lightAngle = 1.0f;

float shadowCheck(vec3 fPos, int light_i)
{
    vec3 fragToLight = fPos - lights[light_i].pos;

    float closestDepth = 0.0f;

	CORIUM_TEXTURE_DEPTHMAP

    float c = 0.015f;

    float currentDepth = length(fragToLight);
    float bias = 0.1f+0.2f*tan(acos(min(lightAngle, 1.0f)));

    closestDepth *= farPlane;

    float shadow = currentDepth -  bias > closestDepth ? 1.0: 0.0;

    return shadow;
}
uniform float limits[64] = float[64](
                 0.015625f,
                 0.515625f,
                 0.140625f,
                 0.640625f,
                 0.046875f,
                 0.546875f,
                 0.171875f,
                 0.671875f,

                 0.765625f,
                 0.265625f,
                 0.890625f,
                  0.390625f,
                 0.796875f,
                  0.296875f,
                  0.921875f,
                  0.421875f,

                  0.203125f,
                  0.703125f,
                  0.078125f,
                  0.578125f,
                  0.234375f,
                  0.734375f,
                  0.109375f,
                  0.609375f,

                  0.953125f,
                  0.453125f,
                 0.828125f,
                  0.328125f,
                  0.984375f,
                  0.484375f,
                  0.859375f,
                  0.359375f,

                  0.0625f,
                  0.5625f,
                  0.1875f,
                  0.6875f,
                  0.03125f,
                  0.53125f,
                  0.15625f,
                  0.65625f,

                  0.8125f,
                  0.3125f,
                  0.9375f,
                  0.4375f,
                 0.78125f,
                  0.28125f,
                  0.90625f,
                  0.40625f,

                  0.25f,
                  0.75f,
                 0.125f,
                  0.625f,
                  0.21875f,
                  0.71875f,
                  0.09375f,
                  0.59375f,

                  1.0f,
                  0.5f,
                  0.875f,
                  0.375f,
                  0.96875f,
                  0.46875f,
                  0.84375f,
                  0.34375f
                  );

float luma(vec3 color) {
  return dot(color, vec3(0.299, 0.587*1.2f, 0.114));
}

float luma(vec4 color) {
  return dot(color.rgb, vec3(0.299, 0.587*1.2f, 0.114));
}

float dither8x8(vec2 position, float brightness) {
  int x = int(mod(position.x, 8.0));
  int y = int(mod(position.y, 8.0));
  int index = x + y * 8;
  float limit = 0.0f;

  //if (x < 8) {
    limit = limits[index];
    //limit = /*int(x + y)%3+*/int(x*0.5f + y)%2;

  return brightness < limit*0.75f ? 0.95f : 1.05f;
}

vec3 dither8x8(vec2 position, vec3 color) {
  return color * dither8x8(position, luma(color));
}

vec4 dither8x8(vec2 position, vec4 color) {
  return vec4(color.rgb * dither8x8(position, luma(color)), 1.0);
}

void main()
{
    float amb = al.x;
    float lght = al.y; // a + l = 1.0f

    vec3 color = texture(diffuseTexture, fTex).rgb * fCol.rgb;
    //color = fCol.rgb * vec3(0.5f, 0.8f, 1.0f);
    vec3 normal = normalize(fNorm);
    vec3 lighting = vec3(0.0f);
    vec3 bloomLighting = vec3(0.0f);

    vec3 viewPos = -inverse(view)[3].xyz;

    // ambient colour
    vec3 ambient = amb * color;

    float shadow = 0.0f;

    for (int l = 0; l < MAX_LIGHTS; ++l) {
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
            spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
            vec3 specular = spec * lightColor; // shinyness = multiplier
            //specular = vec3(0.0f);

            lightAngle = abs(dot(lightDir, normal));
            // SHADOW CHECK
            shadow = shadowCheck(fPos, l);

            // distance fading, inverse square law
            vec3 fadeDist = (fPos - lights[l].pos)/length(lightColor);
            float fade = length(fadeDist)*length(fadeDist)*0.001f;

            lighting += (1.0 - min(shadow+(1.0f-min(lightAngle,1.0f)), 1.0f)) *((((diffuse + specular)) * color))/max(fade, 1.0f);
        }
    }

    float depth = length(viewPos+fPos)/(farPlane);

    vec3 sigma = ambient;
    sigma += lighting*lght;
    gl_FragColor = vec4(sigma,fCol.a);

    //downsample color (16-bit, apparently? rgb 12-bit)
    float s = 16.0f;
    gl_FragColor.r = floor(gl_FragColor.r*s)/s;
    gl_FragColor.g = floor(gl_FragColor.g*s)/s;
    gl_FragColor.b = floor(gl_FragColor.b*s)/s;

    vec4 ditval = dither8x8(vec2(gl_FragCoord.x, gl_FragCoord.y), gl_FragColor);
    gl_FragColor *= ditval*dither*ditval*dither*4.0f + (1.0f - dither);
    gl_FragColor *= 0.5f;
    gl_FragColor.rgb += ambient*0.5f*dither; // adjust amb colors after dither

    gl_FragColor /= length(gl_FragColor.rgb)*0.3f + 0.7f; // brighten and balance a bit

    float fg = max(min((((1.0f - pow(2.7f, -depth- 0.1f)))*1.67f-0.2f)*1.2f, 1.0f), 0.0f);
    gl_FragColor.rgb = fogCol*fg + gl_FragColor.rgb*(1.0f - fg); // FOG

    if (texture(diffuseTexture, fTex).a == 0.0f)
        discard;
}
