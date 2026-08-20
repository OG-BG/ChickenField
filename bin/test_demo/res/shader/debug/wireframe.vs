#version 110

attribute vec3 position; // Posição dos vértices
attribute vec3 barycentric; // Coordenadas baricêntricas por vértice (preenchidas no VBO)

varying vec3 fragBarycentric;

void main() 
{
    fragBarycentric = barycentric;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);
}