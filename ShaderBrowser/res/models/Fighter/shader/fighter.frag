#include "Standard.inc"
#include "Light.inc"
#include "GammaCorrection.inc"

#define FragInStruct_StandardTBN
#include "CommonStruct.inc"

const vec4 specColor = vec4(1, 1, 1, 1);	// 传入albedo更正常
uniform float gloss = 8;

void main()
{
    vec4 albedo = CGL_ALBEDO_COLOR * texture(CGL_TEXTURE0, v2f.coord);
    
	/*
						 - T(世界空间X坐标轴在切线空间的表示) -											|									|									|
		M(切线->世界) = [- B(世界空间Y坐标轴在切线空间的表示) -] = I_M(世界->切线) = T_M(世界->切线) = [T(切线空间T坐标轴在世界空间的表示)	B(切线空间B坐标轴在世界空间的表示)	N(切线空间N坐标轴在世界空间的表示)	]
						 - N(世界空间N坐标轴在切线空间的表示) -											|									|									|
	*/
	// 方式1: 组好矩阵(注意glsl的矩阵是列主序的，要按列传入，先传第1列，然后第2列。。。)
	// tangent-space to world-space
	mat3 tangentToWorldMat = mat3(
		v2f.tangent.x, v2f.tangent.y, v2f.tangent.z,
		v2f.binormal.x, v2f.binormal.y, v2f.binormal.z,
		v2f.normal.x, v2f.normal.y, v2f.normal.z);
	vec3 tangent_normal = texture(CGL_NORMALMAP, v2f.coord).rgb;	// tangent-space
	vec3 normal = tangentToWorldMat * UnpackNormal(tangent_normal);
	// // 方式2：直接拿矩阵每行和法向量相乘
	// vec3 normal = UnpackNormal(tangent_normal);
	// normal = vec3(v2f.tangent.x*normal.x+v2f.binormal.x*normal.y+v2f.normal.x*normal.z,
				  // v2f.tangent.y*normal.x+v2f.binormal.y*normal.y+v2f.normal.y*normal.z,
				  // v2f.tangent.z*normal.x+v2f.binormal.z*normal.y+v2f.normal.z*normal.z);
	
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
        result = result + vec3(BlinnPhongLight(albedo, normal, WorldSpaceViewDir(v2f.world_position), lightColor, lightDir, albedo, gloss))*lightIntensity;
	}
    // 点光源
    for (int i=0; i<CGL_POINT_LIGHT_NUM; ++i)
    {
        getPointLightInfo(i, lightColor, lightIntensity, lightPosition, lightMatrix);
        atten = PointLightAtten(v2f.world_position, lightMatrix);
        lightDir = CalculateDirection(v2f.world_position, lightPosition);
        result = result + vec3(BlinnPhongLight(albedo, normal, WorldSpaceViewDir(v2f.world_position), lightColor, lightDir, albedo, gloss))*lightIntensity*atten;
    }
    // 聚光灯
    for (int i=0; i<CGL_SPOT_LIGHT_NUM; ++i)
    {
        getSpotLightInfo(i, lightColor, lightIntensity, lightPosition, lightMatrix);
        atten = SpotLightAtten(v2f.world_position, lightMatrix);
        lightDir = CalculateDirection(v2f.world_position, lightPosition);
        result = result + vec3(BlinnPhongLight(albedo, normal, WorldSpaceViewDir(v2f.world_position), lightColor, lightDir, albedo, gloss))*lightIntensity*atten;
    }
    
	// 环境光 (ambient)
	result += vec3(CGL_AMBIENT_COLOR * albedo);

    fColor = vec4(LinearToGammaSpace(result.rgb), albedo.a);
}
