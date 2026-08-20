#version 110

attribute vec3 position; // 
attribute vec3 normal; // normal map
attribute vec2 texCoords; // UV
attribute vec3 color;

varying vec2 fragTexCoord;

uniform mat4 model_Matrix; // Matriz do modelo
uniform mat4 view_Matrix;  // Matriz da câmera (view)
uniform mat4 proj_Matrix;  // Matriz de projeção

void main() 
{
    fragTexCoord = texCoords;
    //gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0); // pre gl2
    gl_Position = proj_Matrix * view_Matrix * model_Matrix * vec4(position, 1.0);

}