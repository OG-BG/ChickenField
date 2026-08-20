#version 110

attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
attribute vec3 tangent;

varying vec2 texCoord0;
varying vec3 worldPos0;
varying vec3 tangent0;
varying vec3 bitangent0;
varying vec3 normal0;

uniform mat4 model_Matrix;
uniform mat4 view_Matrix;
uniform mat4 proj_Matrix;

void main()
{
    vec4 worldPos = model_Matrix * vec4(position, 1.0);
    worldPos0 = worldPos.xyz;
    gl_Position = proj_Matrix * view_Matrix * worldPos;

    texCoord0 = texCoord;

    // Calcula TBN matrix mas passa os vetores separadamente
    vec3 n = normalize((model_Matrix * vec4(normal, 0.0)).xyz);
    vec3 t = normalize((model_Matrix * vec4(tangent, 0.0)).xyz);
    t = normalize(t - dot(t, n) * n);
    vec3 b = cross(t, n);

    // Passa os vetores da TBN separadamente
    tangent0 = t;
    bitangent0 = b;
    normal0 = n;
}