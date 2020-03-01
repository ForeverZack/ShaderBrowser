#include "Standard.inc"
#include "Light.inc"

#define FragInStruct_Standard
#include "CommonStruct.inc"

const vec4 specColor = vec4(1, 1, 1, 1);
const float gloss = 8;

void main()
{
    vec4 albedo = CGL_ALBEDO_COLOR * texture(CGL_TEXTURE0, v2f.coord);
    
    vec4 lightColor;
    float lightIntensity;
    vec3 lightDir;
//    vec3 result = vec3(0, 0, 0);
//    // 平行光
//    for (int i=0; i<CGL_DIRECTIONAL_LIGHT_NUM; ++i)
//    {
//        getDirectionalLightInfo(i, lightColor, lightIntensity, lightDir);
//        result = result + vec3(BlinnPhongLight(albedo, v2f.normal, WorldSpaceViewDir(v2f.world_position), lightColor, lightDir, specColor, gloss))*lightIntensity;
//    }
//
//    fColor = vec4(result, albedo.a);
    
    getDirectionalLightInfo(0, lightColor, lightIntensity, lightDir);
    albedo = HalfLambertLight(albedo, v2f.normal, lightColor, lightDir) * lightIntensity;

    fColor = albedo;
    
}
