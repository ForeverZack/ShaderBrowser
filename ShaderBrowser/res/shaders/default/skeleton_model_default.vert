layout (location=0) in vec4 a_position;
layout (location=1) in vec4 a_color;
layout (location=2) in vec2 a_coord;
layout (location=3) in vec3 a_normal;
layout (location=4) in vec3 a_tangent;
layout (location=5) in uvec4 a_boneIds;
layout (location=6) in vec4 a_boneWeights;

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
        mat3x4 simpleBoneMat = CGL_BONES_MATRIX[a_boneIds[i]];
        mat4 boneMatrix = mat4(
            simpleBoneMat[0][0], simpleBoneMat[1][0], simpleBoneMat[2][0], 0,
            simpleBoneMat[0][1], simpleBoneMat[1][1], simpleBoneMat[2][1], 0,
            simpleBoneMat[0][2], simpleBoneMat[1][2], simpleBoneMat[2][2], 0,
            simpleBoneMat[0][3], simpleBoneMat[1][3], simpleBoneMat[2][3], 1);
        skinning += boneMatrix * a_boneWeights[i];
	}

    gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * CGL_MODEL_MATRIX * skinning * vec4(a_position.xyz, 1.0);
	// gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * CGL_MODEL_MATRIX * a_position;
    // v2f.color = a_color;
	v2f.color = a_color;
    v2f.coord = a_coord;
}
