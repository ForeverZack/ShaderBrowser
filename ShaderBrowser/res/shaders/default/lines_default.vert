layout (location=0) in vec4 a_position;
layout (location=1) in vec4 a_color;

#include "Standard.inc"
//out vec4 v2f_color;
out V2FData
{
    vec4 color;
} v2f;

void main() 
{ 
    gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * a_position;
    v2f.color = a_color;
}
