#version 110

attribute vec3 aPos;

uniform mat4 model;
uniform mat4 camMatrix;
uniform mat4 projection; // Adicionar a matriz de projeção

void main()
{
    gl_Position = projection * camMatrix * model * vec4(aPos, 1.0);
}
