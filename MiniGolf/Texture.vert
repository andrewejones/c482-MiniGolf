#version 400 core

layout(location = 0) in vec4 vposition;
layout(location = 1) in vec4 vcolor;
layout(location = 2) in vec3 vnormal;
layout(location = 3) in vec2 in_tex_coord;

uniform mat4 ProjModelView;
uniform mat4 ModelView;
uniform mat3 NormalMatrix;

out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 tex_coord;

void main()
{
    color = vcolor;
    normal = normalize(NormalMatrix * vnormal);
    position = ModelView * vposition;
    gl_Position = ProjModelView * vposition;
    tex_coord = in_tex_coord;
}
