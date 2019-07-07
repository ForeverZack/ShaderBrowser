//layout (location=0) in uint inValue;
layout (location=0) in int inValue;
out float outValue;

const float values[]= float[](100.0f, 200.0f, 300.0f, 400.0f, 500.0f);
//float values[5]= float[5](100.0f, 200.0f, 300.0f, 400.0f, 500.0f);

void main() 
{
//    values[0] = 100.0;
//    values[1] = 200.0;
//    values[2] = 200.0;
//    values[3] = 200.0;
//    values[4] = 200.0;
    
    outValue = sqrt(values[inValue]);
//    outValue = inValue;
}
