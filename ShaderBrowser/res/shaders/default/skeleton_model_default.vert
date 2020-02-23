#include "StandardVertexAttr.inc"
#include "Standard.inc"

out V2FData
{
    vec4 color;
    vec2 coord;
} v2f;

void main() 
{ 
	// 计算蒙皮矩阵
	mat4 skinning = mat4(0.0);
    for (int i=0; i<4; ++i)
    {
//        int boneId = a_boneIds[i];
//        int index = 1 + 4*boneId;
//        mat3x4 simpleBoneMat = CGL_BONES_MATRIX[boneId];
//        mat4 boneMatrix = mat4(
//            simpleBoneMat[0][0], simpleBoneMat[1][0], simpleBoneMat[2][0], 0,
//            simpleBoneMat[0][1], simpleBoneMat[1][1], simpleBoneMat[2][1], 0,
//            simpleBoneMat[0][2], simpleBoneMat[1][2], simpleBoneMat[2][2], 0,
//            simpleBoneMat[0][3], simpleBoneMat[1][3], simpleBoneMat[2][3], 1);
//        skinning += boneMatrix * a_boneWeights[i];
        
        int boneId = a_boneIds[i];
        vec4 col0 = texelFetch(CGL_BONES_MATRIX, boneId*4+0);   // boneMatrix第1列
        vec4 col1 = texelFetch(CGL_BONES_MATRIX, boneId*4+1);   // boneMatrix第2列
        vec4 col2 = texelFetch(CGL_BONES_MATRIX, boneId*4+2);   // boneMatrix第3列
        vec4 col3 = texelFetch(CGL_BONES_MATRIX, boneId*4+3);   // boneMatrix第4列
        mat4 boneMatrix = mat4(
            col0.x, col0.y, col0.z, col0.w,
            col1.x, col1.y, col1.z, col1.w,
            col2.x, col2.y, col2.z, col2.w,
            col3.x, col3.y, col3.z, col3.w); // 注意：opengl中的矩阵是列主序的，传入的数据会先填充满一列，在填充下一列。也就是说这里虽然传入数据格式是这样，但实际使用时，矩阵是(col0, col1, col2, col3)。
        skinning += boneMatrix * a_boneWeights[i];
	}

    gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * CGL_MODEL_MATRIX * skinning * vec4(a_position.xyz, 1.0);
	// gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * CGL_MODEL_MATRIX * a_position;
    // v2f.color = a_color;
	v2f.color = a_color;
    v2f.coord = a_coord;
}
