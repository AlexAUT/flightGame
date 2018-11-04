const int CASCADE_COUNT = 4;
varying vec2 uv;
varying vec3 normal;
varying vec3 position;
varying vec4 pos_shadowmap[CASCADE_COUNT];

struct DirectionalLight
{
  vec3 dir;
  vec3 color;
  float energy;
};

uniform vec3 camera_position;
uniform vec3 camera_viewDir;

uniform bool enableDiffuseTex;
uniform bool enableDecalTex;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec4 specularColor;

uniform sampler2D baseColor_tex;
uniform sampler2D decalColor_tex;
uniform sampler2D shadowMap_1;

const int MAX_DIR_LIGHT_COUNT = 1;
uniform DirectionalLight dirLights[MAX_DIR_LIGHT_COUNT];

vec4 getDiffuseColor()
{
  vec4 color;
  if(enableDecalTex)
  {
    color = 0.5 * texture2D(decalColor_tex, uv);
    color += 0.5 * texture2D(baseColor_tex, uv);
  }
  else if(enableDiffuseTex)
    color = texture2D(baseColor_tex, uv);
  else
    color = vec4(1.0, 1.0, 1.0, 1.0);
  return color * vec4(diffuseColor, 1.0);
}

vec4 calculateDirLight(DirectionalLight light, vec4 diffColor, float visibility)
{
  float dotDiffuse = max(dot(normalize(light.dir), normalize(normal)), 0.0);   
  vec4 dColor = diffColor * vec4(light.color, 1.0) * dotDiffuse;
  float ambient = 0.25;
  vec4 aColor = vec4(ambient, ambient, ambient, 1.0) * diffColor;

  vec3 viewDir = normalize(camera_position - position);
  vec3 reflectDir = reflect(-light.dir, normalize(normal));
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularColor.a);
  vec4 sColor = 0.25 * vec4(specularColor.rgb * spec * light.color, 1.0); 

  return light.energy * (aColor + (dColor + sColor) * visibility);
}

void main()
{
  vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 mapColor = vec4(0.0, 0.0, 0.0, 1.0);

  for(int i = 0; i < MAX_DIR_LIGHT_COUNT; i++)
  {
    float visibility = 1.0;
    float bias = max(0.005 * (1.0 - dot(normal, dirLights[i].dir)), 0.0005);  
    for(int i = 0; i < CASCADE_COUNT; i++)
    {
      if(pos_shadowmap[i].x >= 0.0 && pos_shadowmap[i].x <= 1.0 && pos_shadowmap[i].y >= 0.0 && pos_shadowmap[i].y <= 1.0)
      {
        vec2 scaledTex = pos_shadowmap[i].xy * 0.5;
        scaledTex.x += mod(float(i), 2.0) * 0.5;
        scaledTex.y += (float(i) / 2.0) * 0.5;
        if(texture2D(shadowMap_1, scaledTex).r < pos_shadowmap[i].z - bias)
        {
          visibility = 0.5;
          if(i == 1)
            mapColor = vec4(1.0, 0.0, 0.0, 1.0);
          if(i == 2)
            mapColor = vec4(0.0, 1.0, 0.0, 1.0);
          if(i == 3)
            mapColor = vec4(0.0, 0.0, 1.0, 1.0);
          if(i == 4)
            mapColor = vec4(1.0, 0.0, 1.0, 1.0);
          break;
        }
      }
    }
    
    color += calculateDirLight(dirLights[i], getDiffuseColor(), visibility); 
  }
    
  color.a = 1.0;
  gl_FragColor = color;
  //gl_FragColor = mapColor * getDiffuseColor();
  //gl_FragColor = getDiffuseColor(); 
  //gl_FragColor = vec4(normal, 1.0);
}
