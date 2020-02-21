// 骨骼最大数量
const int MAX_BONES = 100;
const int MAX_DYNAMIC_BATCH_COUNT = 1000;

// shader中可以使用的内置的uniform变量名字(不一定会有值,看具体怎么使用)
uniform sampler2D CGL_TEXTURE0;
uniform sampler2D CGL_TEXTURE1;
uniform mat4 CGL_MODEL_MATRIX;
uniform mat4 CGL_VIEW_MATRIX;
uniform mat4 CGL_PROJECTION_MATRIX;
uniform vec4 CGL_ALBEDO_COLOR = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform DirectionalLight CGL_DIRECTIONAL_LIGHT;
uniform samplerBuffer CGL_BONES_MATRIX;
uniform mat4 CGL_DYNAMIC_BATCH_MODEL_MATRIX[MAX_DYNAMIC_BATCH_COUNT];
