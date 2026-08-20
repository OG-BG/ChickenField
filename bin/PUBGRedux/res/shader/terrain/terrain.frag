#version 110

uniform sampler2D weightmap;
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

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

varying vec2 v_uv;
varying vec3 v_worldPos;
varying vec3 v_normal;
varying float v_height;

void main()
{
    // --- TEXTURAS DO TERRENO ---
    vec4 weights = texture2D(weightmap, v_uv);
    
    // Tiling natural (menor repetição)
    vec4 c0 = texture2D(tex0, v_uv * 20.0);
    vec4 c1 = texture2D(tex1, v_uv * 20.0);
    vec4 c2 = texture2D(tex2, v_uv * 20.0);
    vec4 c3 = texture2D(tex3, v_uv * 20.0);
    
    vec4 albedo = c0 * weights.r
                + c1 * weights.g
                + c2 * weights.b
                + c3 * weights.a;

    // --- NORMAL ---
    vec3 N = normalize(v_normal);
    vec3 V = normalize(C_eyePos - v_worldPos);
    vec3 L = normalize(-R_directionalLight.direction);
    
    // --- ILUMINAÇÃO CORRIGIDA ---
    
    // Luz difusa (sol)
    float NdotL = max(dot(N, L), 0.0);
    
    // Luz ambiente (céu) - cor azul suave
    vec3 ambientColor = vec3(0.25, 0.3, 0.35); // Azul claro natural
    
    // Luz direcional com cor do sol (mais quente)
    vec3 sunColor = vec3(1.0, 0.95, 0.9); // Amarelado suave
    
    // Intensidades
    float ambientIntensity = 0.4;
    float sunIntensity = 1.2;
    
    // --- SOMBRA NÃO FICA BRANCA ---
    // Sombra pura (quando NdotL = 0) usa só ambiente
    vec3 shadowColor = albedo.rgb * ambientColor * ambientIntensity;
    
    // Luz direta
    vec3 lightColor = albedo.rgb * sunColor * sunIntensity * NdotL;
    
    // Mistura sombra + luz
    vec3 finalColor = shadowColor + lightColor;
    
    // --- EFEITO DE ALTURA (natural) ---
    // Vales mais escuros (acumulam sombra)
    float valleyDarkness = clamp(1.0 - v_height * 0.03, 0.7, 1.0);
    finalColor *= valleyDarkness;
    
    // Topos mais iluminados pelo sol
    float peakLight = clamp(v_height * 0.02, 0.0, 0.2);
    finalColor += peakLight * sunColor;
    
    // --- MICRO-DETALHE (baseado na inclinação) ---
    // Encostas mais íngremes podem ter mais sombra
    float slope = 1.0 - abs(N.y);
    float slopeShadow = mix(1.0, 0.85, slope * 0.5);
    finalColor *= slopeShadow;
    
    // --- NEBLINA SUAVE (fog natural) ---
    float dist = length(C_eyePos - v_worldPos);
    float fogStart = 300.0;
    float fogEnd = 800.0;
    float fogFactor = clamp((dist - fogStart) / (fogEnd - fogStart), 0.0, 0.4); // Máx 40%
    
    vec3 fogColor = vec3(0.65, 0.7, 0.75); // Cor da neblina (azul acinzentado)
    finalColor = mix(finalColor, fogColor, fogFactor);
    
    // --- CORREÇÃO DE GAMMA ---
    finalColor = pow(finalColor, vec3(1.0/2.2));
    
    // Saturação natural (levemente aumentada)
    float gray = dot(finalColor, vec3(0.299, 0.587, 0.114));
    finalColor = mix(vec3(gray), finalColor, 1.1);
    
    gl_FragColor = vec4(finalColor, albedo.a);
}


/*
#version 110

uniform sampler2D weightmap;
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

varying vec2 v_uv;
varying float v_height;

void main()
{
    vec4 weights = texture2D(weightmap, v_uv);

    vec4 c0 = texture2D(tex0, v_uv * 16.0);
    vec4 c1 = texture2D(tex1, v_uv * 16.0);
    vec4 c2 = texture2D(tex2, v_uv * 16.0);
    vec4 c3 = texture2D(tex3, v_uv * 16.0);

    vec4 color = c0 * weights.r
               + c1 * weights.g
               + c2 * weights.b
               + c3 * weights.a;

    vec3 lightDir = normalize(vec3(0.4, 5.8, -0.2));

    float shade = clamp(dot(vec3(0,1,0), lightDir),0.4,1.0);

    gl_FragColor = color * shade;
}
*/

