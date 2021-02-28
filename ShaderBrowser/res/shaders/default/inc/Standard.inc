const int MAX_DYNAMIC_BATCH_COUNT = 1000;

// shader中可以使用的内置的uniform变量名字(不一定会有值,看具体怎么使用)
uniform vec3 CGL_MAINCAMERA_WORLDPOS;   // 主相机位置(world space)
uniform sampler2D CGL_TEXTURE0; // 主纹理
uniform sampler2D CGL_TEXTURE1; // 附加纹理1
uniform sampler2D CGL_NORMALMAP; // 法线贴图
uniform mat4 CGL_MODEL_MATRIX;  // model矩阵
uniform mat4 CGL_VIEW_MATRIX;   // view矩阵
uniform mat4 CGL_PROJECTION_MATRIX; // projection矩阵
uniform vec4 CGL_ALBEDO_COLOR = vec4(1.0f, 1.0f, 1.0f, 1.0f);  // 材质颜色
uniform samplerBuffer CGL_BONES_MATRIX; // 骨骼矩阵纹理
uniform mat4 CGL_DYNAMIC_BATCH_MODEL_MATRIX[MAX_DYNAMIC_BATCH_COUNT];   // 自动合并模型model矩阵
uniform sampler2D CGL_LIGHT_TEXTURE0; // 光源衰减纹理(基于距离)
uniform sampler2D CGL_LIGHT_TEXTUREB0; // 光源衰减纹理(基于距离)
//layout (std140) uniform;  // 后面所有的uniform块都会使用这种话布局方式
layout (std140) uniform CameraInfo
{
    mat4 VIEW_MATRIX;
};

// pre-pass
// 顶点着色其中需要加上[PreparePass]来告诉引擎这个pass可以当做pre-pass使用
#ifdef OPEN_PRE_PASS
	// 为了使同一个份着色器代码能作为PrePass的着色器代码使用，这里直接用宏定义了PrePass的处理函数：直接返回
	// ，不再执行原始着色器代码复杂的计算（注意调用它们的位置，位置合理可以减少不必要的计算）
	#define PRE_PASS_VERT()	\
				return;
	#define PRE_PASS_FRAGMENT()	\
				return;
#else
	#define PRE_PASS_VERT() 
	#define PRE_PASS_FRAGMENT() 
#endif
