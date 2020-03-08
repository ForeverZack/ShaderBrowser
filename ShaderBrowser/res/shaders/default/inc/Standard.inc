const int MAX_DYNAMIC_BATCH_COUNT = 1000;

// shader中可以使用的内置的uniform变量名字(不一定会有值,看具体怎么使用)
uniform vec3 CGL_MAINCAMERA_WORLDPOS;   // 主相机位置(world space)
uniform sampler2D CGL_TEXTURE0; // 主纹理
uniform sampler2D CGL_TEXTURE1; // 附加纹理1
uniform mat4 CGL_MODEL_MATRIX;  // model矩阵
uniform mat4 CGL_VIEW_MATRIX;   // view矩阵
uniform mat4 CGL_PROJECTION_MATRIX; // projection矩阵
uniform vec4 CGL_ALBEDO_COLOR = vec4(1.0f, 1.0f, 1.0f, 1.0f);  // 材质颜色
uniform samplerBuffer CGL_BONES_MATRIX; // 骨骼矩阵纹理
uniform mat4 CGL_DYNAMIC_BATCH_MODEL_MATRIX[MAX_DYNAMIC_BATCH_COUNT];   // 自动合并模型model矩阵
uniform sampler2D CGL_LIGHT_TEXTURE0; // 光源衰减纹理
