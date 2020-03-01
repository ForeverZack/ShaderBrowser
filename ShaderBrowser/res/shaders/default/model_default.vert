#include "StandardVertexAttr.inc"
#include "Standard.inc"
#include "Math.inc"

// 要使用Standard.inc中定义的结构体，必须要在include之前define，这点比较蛋疼
#define VertOutStruct_Standard
#include "CommonStruct.inc"

void main() 
{
    vec4 worldPos = ObjectToWorldPosition(a_position);
    gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * worldPos;
    v2f.color = a_color;
    v2f.coord = a_coord;
    v2f.normal = ObjectToWorldNormal(a_normal);
    v2f.world_position = vec3(worldPos);
}
