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
	mat4 skinning = mat4(1.0);
	// if( useGPU )
	{
		//skinning =
		//	skinning * a_boneWeights[0] +
		//	skinning * a_boneWeights[1] +
		//	skinning * a_boneWeights[2] +
		//	skinning * a_boneWeights[3];
			
		skinning =
			CGL_BONES_MATRIX[a_boneIds[0]] * a_boneWeights[0] +
			CGL_BONES_MATRIX[a_boneIds[1]] * a_boneWeights[1] +
			CGL_BONES_MATRIX[a_boneIds[2]] * a_boneWeights[2] +
			CGL_BONES_MATRIX[a_boneIds[3]] * a_boneWeights[3];
	}

    gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * CGL_MODEL_MATRIX * skinning * vec4(a_position.xyz, 1.0);
	// gl_Position = CGL_PROJECTION_MATRIX * CGL_VIEW_MATRIX * CGL_MODEL_MATRIX * a_position;
    // v2f.color = a_color;
	v2f.color = a_color;
    v2f.coord = a_coord;
}
