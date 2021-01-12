// 输入Gamma空间的值，返回线性空间的值 
float GammaToLinearSpaceExact(float value)
{
    if (value <= 0.04045F)
        return value / 12.92F;
    else if (value < 1.0F)
        return pow((value + 0.055F)/1.055F, 2.4F);
    else
        return pow(value, 2.2F);
}

// 输入Gamma空间的颜色值，返回线性空间的颜色值 
vec3 GammaToLinearSpace(vec3 sRGB)
{
    return sRGB * (sRGB * (sRGB * 0.305306011f + 0.682171111f) + 0.012522878f);

    // Precise version, useful for debugging.
    //return vec3(GammaToLinearSpaceExact(sRGB.r), GammaToLinearSpaceExact(sRGB.g), GammaToLinearSpaceExact(sRGB.b));
}

// 输入线性空间的值，返回Gamma空间的值
float LinearToGammaSpaceExact(float value)
{
    if (value <= 0.0f)
        return 0.0f;
    else if (value <= 0.0031308f)
        return 12.92f * value;
    else if (value < 1.0f)
        return 1.055f * pow(value, 0.4166667f) - 0.055f;
    else
        return pow(value, 0.45454545f);
}

// 输入线性空间的颜色值，返回Gamma空间的颜色值
vec3 LinearToGammaSpace(vec3 linearRGB)
{
    linearRGB = max(linearRGB, vec3(0, 0, 0));
    return max(1.055f * pow(linearRGB, vec3(0.416666667f)) - 0.055f, 0.f);

    // Exact version, useful for debugging.
    // return vec3(LinearToGammaSpaceExact(linearRGB.r), LinearToGammaSpaceExact(linearRGB.g), LinearToGammaSpaceExact(linearRGB.b));
}