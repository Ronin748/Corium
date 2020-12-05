#version 140

in vec4 vPos;
in vec2 vTex;

//layout(location = 0) out vec4 gl_FragColor;

uniform sampler2D tex;

void main()
{
/*float x = 410.0f*0.5f;
  gl_FragColor = vec4(texture(tex, vec2(floor(vTex.x*x)/x, vTex.y)).rgb, 1.0f) + vec4(texture(tex, vec2(floor(vTex.x*x)/x+0.5f/x, vTex.y)).rgb, 1.0f);
  gl_FragColor *= 0.5f;*/

  vec4 texCol = texture(tex, vec2(vTex.x, vTex.y));
  //texCol.rgb *= texCol.a;
  gl_FragColor = vec4(texCol.rgb, 1.0f);

}
