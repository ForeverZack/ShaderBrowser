#include "StandardVertexAttr.inc"
#include "Standard.inc"
#include "SkeletonAnimation.inc"
#include "Math.inc"

#define VertOutStruct_Standard
#include "CommonStruct.inc"

void main() 
{ 
	// 计算蒙皮矩阵
    mat4 skinning = VertexSkinningMatrix(a_boneIds, a_boneWeights);

    vec4 worldPos = ObjectToWorldPosition(skinning * vec4(a_position.xyz, 1.0));
    gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * worldPos;
	// gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * CGL_MODEL_MATRIX * a_position;
    // v2f.color = a_color;
	v2f.color = a_color;
    v2f.coord = a_coord;
    v2f.normal = ObjectToWorldNormal(mat3(skinning) * a_normal);
    v2f.world_position = vec3(worldPos);
}
