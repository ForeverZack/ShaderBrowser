layout (location=0) in vec4 a_position;
layout (location=1) in vec4 a_color;
layout (location=2) in vec2 a_coord;

//out vec4 v2f_color;
//out vec2 outCoord;
out V2FData
{
    vec4 color;
    vec2 coord;
} v2f;

void main() 
{ 
    gl_Position = a_position;
    v2f.color = a_color;
    v2f.coord = a_coord;
//    v2f_color = a_color;
//    outCoord = a_coord;
}
