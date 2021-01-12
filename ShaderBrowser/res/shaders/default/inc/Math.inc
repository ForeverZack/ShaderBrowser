// in out inout
/*
	in: 用在函数的参数中，表示这个参数是输入的，在函数中改变这个值，并不会影响对调用的函数产生副作用。（相当于C语言的传值），这个是函数参数默认的修饰符
	out: 用在函数的参数中，表示该参数是输出参数，值是会改变的。
	inout: 用在函数的参数，表示这个参数即是输入参数也是输出参数。
*/ 

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
vec3 ObjectToWorldDir(vec3 dir)
{
    return normalize(mat3(CGL_MODEL_MATRIX) * dir);
}

// 将方向矢量从世界空间变换到模型空间中
vec3 WorldToObjectDir(vec3 dir)
{
    return normalize(mat3(inverse(CGL_MODEL_MATRIX)) * dir);
}

// 输入起点和终点，输出从起点到终点的方向 (注意使用相同的空间)
vec3 CalculateDirection(vec3 from, vec3 to)
{
    return normalize(to - from);
}

// 解法线贴图(法线空间)
vec3 UnpackNormal(vec3 n)
{
	// [0,1] -> [-1,1]
	return normalize(n * 2 - 1); 
}