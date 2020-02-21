// 平行光
struct DirectionalLight
{
    vec3 direction;
    float intensity;
    vec4 color;
};

uniform DirectionalLight CGL_DIRECTIONAL_LIGHT;
