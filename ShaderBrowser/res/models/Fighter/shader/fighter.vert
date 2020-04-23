#include "StandardVertexAttr.inc"
#include "Standard.inc"
#include "SkeletonAnimation.inc"
#include "Math.inc"

#define VertOutStruct_StandardTBN
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
	
	mat3 inverse_model = mat3(inverse(CGL_MODEL_MATRIX));
	
    v2f.normal = normalize(mat3(skinning) * a_normal * inverse_model);
	v2f.tangent = normalize(mat3(skinning) * a_tangent * inverse_model);
	v2f.binormal = normalize(cross(v2f.normal, v2f.tangent));
    v2f.world_position = vec3(worldPos);
}
