#version 330 core

in vec4 vPos;
in vec2 vTex;

layout(location = 0) out vec4 fragCol;

uniform float dithering;
uniform vec2 ratio;

uniform sampler2D screenTex;
uniform sampler2D ditherTex;

float luma(vec4 color) {
	return dot(color.rgb, vec3(0.299, 0.587*1.2f, 0.114));
}

float dither(vec2 p, float lum) {
	float x = mod(p.x, 8.0) + 0.5f;
	float y = mod(p.y, 8.0) + 0.5f;
	float limit = length(texture(ditherTex, vec2(x / 8.0f, y / 8.0f)));

	return lum < limit * 0.5f ? 0.95f : 1.05f;
}

vec4 ditherCol(vec2 p, vec4 col) {
	return vec4(col.rgb * dither(p, luma(col)), 1.0);
}

void main()
{
	vec4 texCol = texture(screenTex, vec2(vTex.x, vTex.y));
	vec4 col = vec4(texCol.rgb, 1.0f);
	
	vec4 ditval = ditherCol(vec2(gl_FragCoord.x * ratio.x, gl_FragCoord.y * ratio.y), col * 1.5f);
    fragCol = ditval * sqrt(ditval) * dithering + col * (1.0f - dithering);
	
	fragCol /= length(fragCol.rgb) * 0.3f + 0.7f; // brighten and balance a bit
}

