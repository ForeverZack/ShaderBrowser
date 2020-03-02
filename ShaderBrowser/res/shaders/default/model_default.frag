#include "Standard.inc"
#include "Light.inc"
#include "Math.inc"

#define FragInStruct_Standard
#include "CommonStruct.inc"

const vec4 specColor = vec4(1, 1, 1, 1);
const float gloss = 20;

void main()
{
    vec4 albedo = CGL_ALBEDO_COLOR * texture(CGL_TEXTURE0, v2f.coord);
    
    vec4 lightColor;
    float lightIntensity;
    vec3 lightDir;
    vec3 result = vec3(0, 0, 0);
    // 平行光 (BlinnPhongLight中包含了diffuse和specular)
    for (int i=0; i<CGL_DIRECTIONAL_LIGHT_NUM; ++i)
    {
        getDirectionalLightInfo(i, lightColor, lightIntensity, lightDir);
        result = result + vec3(BlinnPhongLight(albedo, v2f.normal, WorldSpaceViewDir(v2f.world_position), lightColor, lightDir, specColor, gloss))*lightIntensity;
    }
	
	// 环境光 (ambient)
	result += vec3(CGL_AMBIENT_COLOR * albedo);

    fColor = vec4(result.xyz, albedo.a);
}
