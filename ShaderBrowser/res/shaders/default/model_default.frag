out vec4 fColor;

#include "Standard.inc"
#include "Light.inc"

//in vec4 v2f_color;
//in vec2 inCoord;

in V2FData
{
    vec4 color;
    vec2 coord;
    vec3 normal;    // world-space
} v2f;

void main()
{
    vec4 albedo = CGL_ALBEDO_COLOR * texture(CGL_TEXTURE0, v2f.coord);
    
    vec4 lightColor;
    float lightIntensity;
    vec3 lightDir;
//    for (int i=0; i<CGL_DIRECTIONAL_LIGHT_NUM; ++i)
//    {
//        getDirectionalLightInfo(i, lightColor, lightIntensity, lightDir);
//        albedo = HalfLambertLight(albedo, v2f.normal, lightColor, lightIntensity, lightDir);
//    }
    // test
    getDirectionalLightInfo(0, lightColor, lightIntensity, lightDir);
    albedo = HalfLambertLight(albedo, v2f.normal, lightColor, lightIntensity, lightDir);

    fColor = albedo;
}
