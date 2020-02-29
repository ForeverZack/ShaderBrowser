// 注意！！！：要使用Standard.inc中定义的结构体，必须要在include之前define，这点比较蛋疼

// 片段着色器默认仅输出颜色（注意：这里是#ifndef）
#ifndef FragOutStruct_Custom
    out vec4 fColor;
#endif

// 顶点着色器输出结构体
// 标准结构
#ifdef VertOutStruct_Standard
    out V2FData
    {
        vec4 color;
        vec2 coord;
        vec3 normal;
    } v2f;
#endif

// 片段着色器输入结构题
#ifdef FragInStruct_Standard
    in V2FData
    {
        vec4 color;
        vec2 coord;
        vec3 normal;
    } v2f;
#endif
