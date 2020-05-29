#include "StandardVertexAttr.inc"
#include "Standard.inc"

void main() 
{
    gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * CGL_MODEL_MATRIX * a_position;
}
