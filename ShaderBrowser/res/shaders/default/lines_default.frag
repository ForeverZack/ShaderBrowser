out vec4 fColor;

in V2FData
{
    vec4 color;
} v2f;

void main()
{
    fColor = v2f.color;
//    fColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
