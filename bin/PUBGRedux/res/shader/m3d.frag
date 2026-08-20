#version 110

varying vec2 texCoord0;

void main() {
    // Cria um padrão de xadrez 16x16
    vec2 check = fract(texCoord0 * 16.0);
    float mask = step(0.5, check.x) == step(0.5, check.y) ? 1.0 : 0.5;
    gl_FragColor = vec4(vec3(mask), 1.0);
}

/*
varying vec2 texCoord0;
varying vec3 worldPos0;
varying vec3 tangent0;
varying vec3 bitangent0;
varying vec3 normal0;

uniform sampler2D diffuse;
uniform sampler2D normalMap;

uniform vec3 C_eyePos;

struct BaseLight 
{
    vec3 color;
    float intensity;
};

struct DirectionalLight 
{
    BaseLight base;
    vec3 direction;
};

uniform DirectionalLight R_directionalLight;
uniform int useNormalMap;

// Rim light suave e quente
float rimLight(vec3 n, vec3 v, float power)
{
    float r = 1.0 - max(dot(n, v), 0.0);
    return pow(r, power);
}

// Outline mínimo baseado no N·V
float outlineFactor(vec3 n, vec3 v)
{
    float nv = dot(n, v);
    return smoothstep(0.25, 0.05, nv);
}

void main()
{
    vec2 uv = texCoord0;
    vec4 baseColor = texture2D(diffuse, uv);

    // --- NORMAL ---
    vec3 N;
    if(useNormalMap == 1)
    {
        mat3 TBN = mat3(tangent0, bitangent0, normal0);
        vec3 nm = texture2D(normalMap, uv).xyz;
        nm = normalize(nm * 2.0 - 1.0);
        N = normalize(TBN * nm);
    }
    else 
    {
        N = normalize(normal0);
    }

    vec3 V = normalize(C_eyePos - worldPos0);
    vec3 L = normalize(-R_directionalLight.direction);

    float ndl = clamp(dot(N, L), 0.0, 1.0);

    // --- *AQUI MANTÉM EXATAMENTE SUA FAIXA DE LUZ* ---
    float lightEdge = smoothstep(0.45, 0.55, ndl);

    float texLuma = dot(baseColor.rgb, vec3(0.299, 0.587, 0.114));

    // --- SUAS CORES ORIGINAIS ---
    vec3 shadowColor = baseColor.rgb * vec3(1.0, 0.63, 0.50);
    vec3 lightColor  = mix(vec3(1.0, 0.95, 0.9), vec3(1.0), 0.5);

    vec3 litColor = mix(shadowColor, lightColor, lightEdge);

    float brightnessFactor = smoothstep(0.25, 0.15, texLuma);
    float lightFactor = smoothstep(0.55, 0.95, ndl);

    float materialResponse = mix(0.35, 1.0, pow(texLuma, 1.2));

    float adaptiveBloom = lightFactor * pow(brightnessFactor, 1.8) * materialResponse;

    litColor = mix(litColor, vec3(1.0), adaptiveBloom * 0.9);

    litColor = mix(litColor, baseColor.rgb, (1.0 - brightnessFactor) * 0.45);

    litColor *= materialResponse;

    // --- RIM LIGHT SUAVE ---
    float rim = rimLight(N, V, 3.8) * 0.20;
    litColor += rim * vec3(1.0, 0.8, 0.7);

    // --- OUTLINE SUTIL ---
    float outline = outlineFactor(N, V);
    litColor = mix(litColor, vec3(0.03, 0.03, 0.045), outline * 0.15);

    // --- CONTRASTE / GAMMA ---
    litColor = (litColor - 0.5) * 1.05 + 0.5;
    litColor = pow(litColor, vec3(0.95));

    gl_FragColor = vec4(clamp(litColor, 0.0, 1.0), baseColor.a);
}*/

