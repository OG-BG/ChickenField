#version 110

varying vec2 fragTexCoord;

uniform sampler2D textureSampler;

void main() 
{
    gl_FragColor = texture2D(textureSampler, fragTexCoord);
}