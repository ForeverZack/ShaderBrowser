// 计算顶点的蒙皮矩阵
// boneIds: 控制顶点的骨骼id数组(注意是int类型的)
// boneWeights: 控制顶点的骨骼权重数组
mat4 VertexSkinningMatrix(ivec4 boneIds, vec4 boneWeights)
{
    mat4 skinning = mat4(0.0);
    for (int i=0; i<4; ++i)
    {
        int boneId = boneIds[i];
        vec4 col0 = texelFetch(CGL_BONES_MATRIX, boneId*4+0);   // boneMatrix第1列
        vec4 col1 = texelFetch(CGL_BONES_MATRIX, boneId*4+1);   // boneMatrix第2列
        vec4 col2 = texelFetch(CGL_BONES_MATRIX, boneId*4+2);   // boneMatrix第3列
        vec4 col3 = texelFetch(CGL_BONES_MATRIX, boneId*4+3);   // boneMatrix第4列
        mat4 boneMatrix = mat4(
            col0.x, col0.y, col0.z, col0.w,
            col1.x, col1.y, col1.z, col1.w,
            col2.x, col2.y, col2.z, col2.w,
            col3.x, col3.y, col3.z, col3.w); // 注意：opengl中的矩阵是列主序的，传入的数据会先填充满一列，在填充下一列。也就是说这里虽然传入数据格式是这样，但实际使用时，矩阵是(col0, col1, col2, col3)。
        skinning += boneMatrix * boneWeights[i];
    }
    return skinning;
}
