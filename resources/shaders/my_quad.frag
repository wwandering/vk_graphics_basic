#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;

layout (binding = 0) uniform sampler2D colorTex;

layout (location = 0 ) in VS_OUT
{
  vec2 texCoord;
} surf;

void sort(inout vec4 colors[9]) {
  bool flag = false;
  for (int ch = 0; ch < 3; ++ch) {
    for (int i = 0; i < 8; ++i) {
      flag = false;
      for (int j = 0; j < 8 - i; ++j) {
        if (colors[j+1][ch] < colors[j][ch]) {
          vec4 s = colors[j];
          colors[j] = colors[j+1];
          colors[j+1] = s;
          flag = true;
        }
      }
      if (!flag) {
        break;
      }
    }
  }
}

void main()
{
  vec4 fin_colors[9] = vec4[9](
    textureLodOffset(colorTex, surf.texCoord, 0, ivec2(0, 0)),
    textureLodOffset(colorTex, surf.texCoord, 0, ivec2(-1, 0)),
    textureLodOffset(colorTex, surf.texCoord, 0, ivec2(0, -1)),
    textureLodOffset(colorTex, surf.texCoord, 0, ivec2(1, 0)),
    textureLodOffset(colorTex, surf.texCoord, 0, ivec2(0, 1)),
    textureLodOffset(colorTex, surf.texCoord, 0, ivec2(1, 1)),
    textureLodOffset(colorTex, surf.texCoord, 0, ivec2(1, -1)),
    textureLodOffset(colorTex, surf.texCoord, 0, ivec2(-1, 1)),
    textureLodOffset(colorTex, surf.texCoord, 0, ivec2(-1, -1))
  );
  sort(fin_colors);
  color = fin_colors[4];
}
