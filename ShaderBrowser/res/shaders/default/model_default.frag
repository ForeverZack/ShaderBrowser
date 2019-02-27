
out vec4 fColor;

//in vec4 v2f_color;
//in vec2 inCoord;

in V2FData
{
    vec4 color;
    vec2 coord;
} v2f;

void main()
{
    fColor = CGL_ALBEDO_COLOR * texture(CGL_TEXTURE0, v2f.coord);
}
