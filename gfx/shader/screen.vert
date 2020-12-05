#version 140

/*layout(location = 0) */in vec4 pos;
/*layout(location = 1) */in vec4 tex;

out vec4 vPos;
out vec2 vTex;

void main()
{
  gl_Position = vec4(pos.xyz, 1.0f);

  vTex = vec2(tex.s*tex.p, tex.t*tex.q);
  vPos = pos;
}
