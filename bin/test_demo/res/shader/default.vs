#version 110

// Positions/Coordinates
//attribute vec3 aPos; 
//attribute vec3 aColor;
//attribute vec2 aTex;
//attribute vec3 aNormal;

attribute vec3 vertPosition;
attribute vec3 vertColor;
attribute vec2 vertTexCoords; // Texture Coords Attrib
attribute vec3 vertNormal;

// Outputs for Fragment Shader
varying vec3 fragColor;
varying vec2 fragTexCoord;
varying vec3 fragNormal;
varying vec3 fragWorldPos;

// Uniforms
//uniform mat4 camMatrix;
//uniform mat4 model;
//uniform mat4 projection;

uniform mat4 model_Matrix;
uniform mat4 view_Matrix; // Camera View Matrix
uniform mat4 proj_Matrix; // Camera Projection Matrix


void main()
{
    fragWorldPos = vec3(model_Matrix * vec4(vertPosition, 1.0));
    gl_Position = proj_Matrix * view_Matrix * vec4(fragWorldPos, 1.0);

    fragColor = vertColor;
    fragTexCoord = vertTexCoords;
    fragNormal = vertNormal;
}

