#version 110


uniform mat4 model_Matrix;
uniform mat4 view_Matrix; // Camera View Matrix
uniform mat4 proj_Matrix; // Camera Projection Matrix

// Positions/Coordinates
attribute vec3 vertPosition;  // = aPos
attribute vec3 vertColor;
attribute vec2 vertTexCoords; // Texture Coords Attrib
attribute vec3 vertNormal;

// Outputs for Fragment Shader
varying vec3 fragColor;
varying vec2 fragTexCoord;
varying vec3 fragNormal;
varying vec3 fragWorldPos;

void main()
{
    fragWorldPos = vec3(model_Matrix * vec4(vertPosition, 1.0));
    gl_Position = proj_Matrix * view_Matrix * vec4(fragWorldPos, 1.0);

    fragColor = vertColor;
    fragTexCoord = vertTexCoords;
    fragNormal = vertNormal;
}

/*
void main()
{
    vec4 worldPos = model_Matrix * vec4(vertPosition, 1.0);
    fragWorldPos = worldPos.xyz;
    fragTexCoords = vertTexCoords;

    gl_Position = proj_Matrix * view_Matrix * worldPos;

    fragColor = vertColor;
    fragNormal = vertNormal;
}*/


