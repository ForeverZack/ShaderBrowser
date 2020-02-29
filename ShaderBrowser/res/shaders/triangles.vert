#include "StandardVertexAttr.inc"
#include "Standard.inc"
#include "Standard.inc"

#define VertOutStruct_Standard
#include "CommonStruct.inc"

void main() 
{ 
    gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * CGL_MODEL_MATRIX * a_position;
    v2f.color = a_color;
    v2f.coord = a_coord;
//    v2f_color = a_color;
//    outCoord = a_coord;
}
