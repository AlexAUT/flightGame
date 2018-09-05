varying vec2 uv;
varying vec3 normal;

uniform vec3 camera_position;
uniform vec3 camera_viewDir;

uniform bool enableDiffuseTex;
uniform vec3 diffuseColor;
uniform sampler2D baseColor_tex;

void main()
{
  vec4 color = vec4(diffuseColor, 1.0);
  if(enableDiffuseTex)
    color *= texture2D(baseColor_tex, uv);
  color.a = 1.0;

  gl_FragColor = color;
}
