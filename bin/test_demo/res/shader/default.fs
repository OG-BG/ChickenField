#version 110

// Outputs colors in RGBA
varying vec3 fragColor;
varying vec2 fragTexCoord;
varying vec3 fragNormal;
varying vec3 fragWorldPos;

// Uniforms
uniform sampler2D tex0; // diffuse
uniform sampler2D tex1; // specular
uniform sampler2D tex2; // normal
uniform vec4 lightColor; // Color of the light
uniform vec3 lightPos;   // Position of the light
uniform vec3 camPos;     // Position of the camera

vec4 pointLight()
{
    vec3 lightVec = lightPos - fragWorldPos;
    float dist = length(lightVec);
    float a = 3.0;
    float b = 0.7;
    float inten = 1.0 / (a * dist * dist + b * dist + 1.0);

    float ambient = 0.20;

    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(lightVec);
    float diffuse = max(dot(normal, lightDirection), 0.0);

    float specularLight = 0.50;
    vec3 viewDirection = normalize(camPos - fragWorldPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0), 16.0);
    float specular = specAmount * specularLight;

    return (texture2D(tex0, fragTexCoord) * (diffuse * inten + ambient) + 
            texture2D(tex1, fragTexCoord).r * specular * inten) * lightColor;
}

vec4 direcLight()
{
    float ambient = 0.20;

    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(vec3(1.0, 1.0, 0.0));
    float diffuse = max(dot(normal, lightDirection), 0.0);

    float specularLight = 0.50;
    vec3 viewDirection = normalize(camPos - fragWorldPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0), 16.0);
    float specular = specAmount * specularLight;

    return (texture2D(tex0, fragTexCoord) * (diffuse + ambient) + 
            texture2D(tex1, fragTexCoord).r * specular) * lightColor;
}

vec4 spotLight()
{
    float outerCone = 0.90;
    float innerCone = 0.95;

    float ambient = 0.20;

    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(lightPos - fragWorldPos);
    float diffuse = max(dot(normal, lightDirection), 0.0);

    float specularLight = 0.50;
    vec3 viewDirection = normalize(camPos - fragWorldPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0), 16.0);
    float specular = specAmount * specularLight;

    float angle = dot(vec3(0.0, -1.0, 0.0), -lightDirection);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0, 1.0);

    return (texture2D(tex0, fragTexCoord) * (diffuse * inten + ambient) + 
            texture2D(tex1, fragTexCoord).r * specular * inten) * lightColor;
}

void main()
{
    gl_FragColor = direcLight();
}