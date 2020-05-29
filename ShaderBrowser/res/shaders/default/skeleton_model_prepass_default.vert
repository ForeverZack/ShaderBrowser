#include "StandardVertexAttr.inc"
#include "Standard.inc"

void main() 
{ 
	// 计算蒙皮矩阵
    mat4 skinning = VertexSkinningMatrix(a_boneIds, a_boneWeights);
    gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * CGL_MODEL_MATRIX * (skinning * vec4(a_position.xyz, 1.0));

}
