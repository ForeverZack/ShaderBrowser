// in out inout

// 输入模型空间的顶点位置，输出世界空间中的顶点位置
vec4 ObjectToWorldPosition(vec4 v)
{
    return CGL_MODEL_MATRIX * v;
}

// 输入世界空间的顶点位置，返回世界空间中从该点到摄像机的观察方向
vec3 WorldSpaceViewDir(vec3 v)
{
    return normalize(CGL_MAINCAMERA_WORLDPOS - v);
}

// 输入模型空间的法线方向，输出世界空间中的法线方向
vec3 ObjectToWorldNormal(vec3 n)
{
    // 法线矩阵: IT_Model or TI_Model
    // IT_Model*normal = normal*I_Model
    return normalize(n * mat3(inverse(CGL_MODEL_MATRIX)));
}

// 将方向矢量从模型空间变换到世界空间中
vec3 ObjectToWorldDir(out vec3 dir)
{
    return normalize(mat3(CGL_MODEL_MATRIX) * dir);
}

// 将方向矢量从世界空间变换到模型空间中
vec3 WorldToObjectDir(out vec3 dir)
{
    return normalize(mat3(inverse(CGL_MODEL_MATRIX)) * dir);
}
