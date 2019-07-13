layout (location=0) in int inValue;
out float outValue;
out float outValue2;

uniform samplerBuffer testInput;

void setValue(out float ooo);
void setValue(out float ooo, float aaa);

void main()
{
//    setValue(outValue);
    
    float value = texelFetch(testInput, inValue).x;
    setValue(outValue, value);
    
    outValue2 = texelFetch(testInput, inValue).y;
}
void setValue(out float ooo)
{
    float aaa;
    if (true)
    {
        float value = texelFetch(testInput, inValue).x;
        aaa = ooo = value;
        return;
    }

    ooo = 1;
}
void setValue(out float ooo, float aaa)
{
    ooo = aaa;
}

//void main()
//{
//    float value = texelFetch(testInput, inValue).x;
//    outValue = value;
//}
