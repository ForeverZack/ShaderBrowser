#include "Math.inc"

// 环境光
uniform vec4 CGL_AMBIENT_COLOR;

// 平行光最大支持数量
const int MAX_DIRECTIONAL_LIGHT_COUNT = 2;
// 平行光
struct DirectionalLight
{
    vec4 color;
    float intensity;
    vec3 direction; // world-space (normalized)
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
    dir = light.direction;
}

// Lambert光照模型
// albedo: 片段自身颜色
// normal: 世界空间下的片段的法线
// lightDir: 光源方向
// lightColor: 光源颜色
vec4 LambertLight(vec4 albedo, vec3 normal, vec4 lightColor, float lightIntensity, vec3 lightDir)
{
    float diff = max(0, dot (normal, lightDir));

    vec4 c;
    c.rgb = albedo.rgb * lightColor.rgb * diff * lightIntensity;
    c.a = albedo.a;
    return c;
}

// 半Lambert光照模型
// albedo: 片段自身颜色
// normal: 世界空间下的片段的法线
// lightDir: 光源方向
// lightColor: 光源颜色
vec4 HalfLambertLight(vec4 albedo, vec3 normal, vec4 lightColor, float lightIntensity, vec3 lightDir)
{
    float diff = max(0, dot (normal, lightDir))*0.5 + 0.5;

    vec4 c;
    c.rgb = albedo.rgb * lightColor.rgb * diff * lightIntensity;
    c.a = albedo.a;
    return c;
}
