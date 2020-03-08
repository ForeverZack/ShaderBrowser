#include "Math.inc"

// 环境光
uniform vec4 CGL_AMBIENT_COLOR;

// 平行光
#ifndef CLOSE_DIRECTIONAL_LIGHT
// 平行光最大支持数量
const int MAX_DIRECTIONAL_LIGHT_COUNT = 2;
// 平行光数据结构
struct DirectionalLight
{
    vec4 color;
    float intensity;
    vec3 direction; // world-space (normalized)
	mat4 lightMatrix;	// world-space -> light-space
};
// 平行光数量
uniform int CGL_DIRECTIONAL_LIGHT_NUM;
// 平行光数据
uniform DirectionalLight CGL_DIRECTIONAL_LIGHTS[MAX_DIRECTIONAL_LIGHT_COUNT];
// 获取平行光数据
void getDirectionalLightInfo(int idx, out vec4 col, out float intensity, out vec3 dir)
{
    DirectionalLight light = CGL_DIRECTIONAL_LIGHTS[idx];
    col = light.color;
    intensity = light.intensity;
    dir = -light.direction;	// 注意：平行光的光源方向和平行光方向是相反的
}
void getDirectionalLightInfo(int idx, out vec4 col, out float intensity, out vec3 dir, out mat4 mat)
{
    DirectionalLight light = CGL_DIRECTIONAL_LIGHTS[idx];
    col = light.color;
    intensity = light.intensity;
    dir = -light.direction;    // 注意：平行光的光源方向和平行光方向是相反的
    mat = light.lightMatrix;
}
#endif

// 点光源
#ifndef CLOSE_POINT_LIGHT
// 点光源最大支持数量
const int MAX_POINT_LIGHT_COUNT = 4;
// 点光源数据结构
struct PointLight
{
    vec4 color;
    float intensity;
    vec3 position;  // world-space
    mat4 lightMatrix;   // world-space -> light-space
};
// 点光源数量
uniform int CGL_POINT_LIGHT_NUM;
// 点光源数据
uniform PointLight CGL_POINT_LIGHTS[MAX_POINT_LIGHT_COUNT];
//// 光源衰减纹理  standard.inc中
//uniform sampler2D CGL_LIGHT_TEXTURE0;
// 获取点光源数据
void getPointLightInfo(int idx, out vec4 col, out float intensity, out vec3 pos)
{
    PointLight light = CGL_POINT_LIGHTS[idx];
    col = light.color;
    intensity = light.intensity;
    pos = light.position;
}
void getPointLightInfo(int idx, out vec4 col, out float intensity, out vec3 pos, out mat4 mat)
{
    PointLight light = CGL_POINT_LIGHTS[idx];
    col = light.color;
    intensity = light.intensity;
    pos = light.position;
    mat = light.lightMatrix;
}
// 计算点光源衰减度
float PointLightAtten(vec3 worldPos, in mat4 lightMatrix)
{
    // 把点坐标转换到点光源的坐标空间中，lightMatrix由引擎代码计算后传递到shader中，这里包含了对点光源范围的计算，具体可参考Unity引擎源码。经过lightMatrix变换后，在点光源中心处lightCoord为(0, 0, 0)，在点光源的范围边缘处lightCoord模为1
    vec3 lightCoord = vec3(lightMatrix * vec4(worldPos, 1));
    // 使用点到光源中心距离的平方dot(lightCoord, lightCoord)构成二维采样坐标，对衰减纹理CGL_LIGHT_TEXTURE0采样。对它的采样都是使用光源空间到点到光源中心的距离平方来作为采样坐标
    float coord = dot(lightCoord, lightCoord);
    float atten = texture(CGL_LIGHT_TEXTURE0, vec2(coord, coord)).a;
    
    return atten;
}
#endif

// Lambert光照模型
// albedo: 片段自身颜色
// normal: 世界空间下的片段的法线
// lightDir: 光源方向
// lightColor: 光源颜色
// lightIntensity: 光照强度
vec4 LambertLight(vec4 albedo, vec3 normal, vec4 lightColor, vec3 lightDir)
{
    float diff = max(0, dot (normal, lightDir));

    vec4 c;
    c.rgb = albedo.rgb * lightColor.rgb * diff;
    c.a = albedo.a;
    return c;
}
vec4 LambertLight(vec4 albedo, vec3 normal, vec4 lightColor, vec3 lightDir, float lightIntensity)
{
    float diff = max(0, dot (normal, lightDir));

    vec4 c;
    c.rgb = albedo.rgb * lightColor.rgb * diff;
    c.a = albedo.a;
    return c;
}

// 半Lambert光照模型
// albedo: 片段自身颜色
// normal: 世界空间下的片段的法线
// lightDir: 光源方向
// lightColor: 光源颜色
// lightIntensity: 光照强度
vec4 HalfLambertLight(vec4 albedo, vec3 normal, vec4 lightColor, vec3 lightDir)
{
    float diff = max(0, dot (normal, lightDir))*0.5 + 0.5;

    vec4 c;
    c.rgb = albedo.rgb * lightColor.rgb * diff;
    c.a = albedo.a;
    return c;
}
vec4 HalfLambertLight(vec4 albedo, vec3 normal, vec4 lightColor, vec3 lightDir, float lightIntensity)
{
    float diff = max(0, dot (normal, lightDir))*0.5 + 0.5;

    vec4 c;
    c.rgb = albedo.rgb * lightColor.rgb * diff * lightIntensity;
    c.a = albedo.a;
    return c;
}

// BlinnPhong光照模型 (包含Lambert)
// albedo: 片段自身颜色
// normal: 世界空间下的片段的法线
// viewDir: 片段到摄像机的向量
// lightDir: 光源方向
// lightColor: 光源颜色
// lightIntensity: 光照强度
// specColor: 高光反射颜色
// gloss: shininess
vec4 BlinnPhongLight(vec4 albedo, vec3 normal, vec3 viewDir, vec4 lightColor, vec3 lightDir, vec4 specColor, float gloss)
{	
    // diff
    vec4 diffuse = LambertLight(albedo, normal, lightColor, lightDir);

    // specular
    vec3 h = normalize(lightDir + viewDir);
    vec4 specular = lightColor * specColor * pow(max(0, dot(normal, h)), gloss);

    // combine
    vec4 c = diffuse + specular;
    c.a = albedo.a;
    
    return c;
}

// BlinnPhong光照模型 (包含半Lambert)
// albedo: 片段自身颜色
// normal: 世界空间下的片段的法线
// viewDir: 片段到摄像机的向量
// lightDir: 光源方向
// lightColor: 光源颜色
// lightIntensity: 光照强度
// specColor: 高光反射颜色
// gloss: shininess
vec4 BlinnPhongHalfLambertLight(vec4 albedo, vec3 normal, vec3 viewDir, vec4 lightColor, vec3 lightDir, vec4 specColor, float gloss)
{
    // diff
    vec4 diffuse = HalfLambertLight(albedo, normal, lightColor, lightDir);

    // specular
    vec3 h = normalize(lightDir + viewDir);
    vec4 specular = lightColor * specColor * pow(max(0, dot(normal, h)), gloss);
    
    // combine
    vec4 c = diffuse + specular;
    c.a = albedo.a;
    
    return c;
}
