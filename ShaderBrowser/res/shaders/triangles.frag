uniform float cusTest[4];
uniform mat4 matTest;

#include "Standard.inc"

out vec4 fColor;

//in vec4 v2f_color;
//in vec2 inCoord;

in V2FData
{
    vec4 color;
    vec2 coord;
} v2f;

void main()
{
//    fColor = vec4(matTest[0][0], matTest[0][1], matTest[0][2], matTest[3][3]);
//    fColor = vec4(cusTest[0], cusTest[1], cusTest[2], cusTest[3]);
//    fColor = v2f_color;
//    fColor = v2f.color;
    fColor = mix(texture(CGL_TEXTURE0, v2f.coord), texture(CGL_TEXTURE1, v2f.coord), 0.5);
//    fColor = vec4(v2f.coord.x, v2f.coord.y, 0, 1);
}
