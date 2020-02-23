#include "StandardVertexAttr.inc"
#include "Standard.inc"

//out vec4 v2f_color;
//out vec2 outCoord;
out V2FData
{
    vec4 color;
    vec2 coord;
} v2f;

void main() 
{ 
    gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * CGL_MODEL_MATRIX * a_position;
    v2f.color = a_color;
    v2f.coord = a_coord;
}
