#include "Standard.inc"
#include "Light.inc"
#include "Math.inc"

#define FragInStruct_Standard
#include "CommonStruct.inc"

const vec4 specColor = vec4(1, 1, 1, 1);
const float gloss = 20;

void main()
{
    vec4 albedo = CGL_ALBEDO_COLOR;
//    vec4 albedo = texture(CGL_TEXTURE0, v2f.coord);
    
    vec4 lightColor;
    float lightIntensity;
    vec3 lightDir;
    mat4 lightMatrix;
    vec3 lightPosition;
    float atten;
    vec3 result = vec3(0, 0, 0);
    // 平行光 (BlinnPhongLight中包含了diffuse和specular)
    for (int i=0; i<CGL_DIRECTIONAL_LIGHT_NUM; ++i)
    {
        getDirectionalLightInfo(i, lightColor, lightIntensity, lightDir);
        result = result + vec3(BlinnPhongLight(albedo, v2f.normal, WorldSpaceViewDir(v2f.world_position), lightColor, lightDir, specColor, gloss))*lightIntensity;
    }
    // 点光源
    for (int i=0; i<CGL_POINT_LIGHT_NUM; ++i)
    {
        getPointLightInfo(i, lightColor, lightIntensity, lightPosition, lightMatrix);
        atten = PointLightAtten(v2f.world_position, lightMatrix);
        lightDir = CalculateDirection(v2f.world_position, lightPosition);
        result = result + vec3(BlinnPhongLight(albedo, v2f.normal, WorldSpaceViewDir(v2f.world_position), lightColor, lightDir, specColor, gloss))*lightIntensity*atten;
    }
    // 聚光灯
    for (int i=0; i<CGL_SPOT_LIGHT_NUM; ++i)
    {
        getSpotLightInfo(i, lightColor, lightIntensity, lightPosition, lightMatrix);
        atten = SpotLightAtten(v2f.world_position, lightMatrix);
        lightDir = CalculateDirection(v2f.world_position, lightPosition);
        result = result + vec3(BlinnPhongLight(albedo, v2f.normal, WorldSpaceViewDir(v2f.world_position), lightColor, lightDir, specColor, gloss))*lightIntensity*atten;
    }
	
	// 环境光 (ambient)
	result += vec3(CGL_AMBIENT_COLOR * albedo);

    fColor = vec4(result.xyz, albedo.a);
}
