#version 330 core

in vec4 vPos;
in vec2 vTex;
layout(location = 0) out vec4 fragCol;

uniform float dither;

uniform sampler2D tex;
uniform vec2 ratio;

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

    limit = limits[index];

	return brightness < limit * 0.75f ? 0.95f : 1.05f;
}

vec3 dither8x8(vec2 position, vec3 color) {
	return color * dither8x8(position, luma(color));
}

vec4 dither8x8(vec2 position, vec4 color) {
	return vec4(color.rgb * dither8x8(position, luma(color)), 1.0);
}

void main()
{
	vec4 texCol = texture(tex, vec2(vTex.x, vTex.y));
	//texCol.rgb *= texCol.a;
	vec4 col = vec4(texCol.rgb, 1.0f);
	
//	dither = 0.0f;
	vec4 ditval = dither8x8(vec2(gl_FragCoord.x * ratio.x, gl_FragCoord.y * ratio.y), col * 1.5f);
    fragCol = ditval * sqrt(ditval) * dither + col * (1.0f - dither);
	
	fragCol /= length(fragCol.rgb) * 0.3f + 0.7f; // brighten and balance a bit
}
