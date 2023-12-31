#version 430

uniform sampler2D imageTexture;
uniform ivec2 viewportSize;
uniform float alpha;

out vec4 out_Color;

void main()
{
  vec2 texCoord = vec2(gl_FragCoord.xy/vec2(viewportSize.xy));
  vec3 result = texture(imageTexture, texCoord).rgb;
  out_Color = vec4(result.xyz, alpha);
}
