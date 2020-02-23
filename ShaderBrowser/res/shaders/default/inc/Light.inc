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
void getDirectionalLightInfo(int idx, vec4& col, float& intensity, vec3& dir)
{
    DirectionalLight light = CGL_DIRECTIONAL_LIGHT[idx];
    col = light.color;
    intensity = light.intensity;
    dir = light.direction;
}
