#version 110

attribute vec3 position;
attribute vec2 texcoord;
attribute vec3 normal;  // Vamos precisar de normais básicas

varying vec2 v_uv;
varying vec3 v_worldPos;
varying vec3 v_normal;

void main()
{
    v_uv = texcoord;
    v_worldPos = position;
    v_normal = normal;

    gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);
}