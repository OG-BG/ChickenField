#version 110

varying vec3 fragBarycentric;

void main() 
{
    float lineWidth = 0.05; // Define a largura das linhas
    vec3 edge = smoothstep(0.0, lineWidth, fragBarycentric); // Suaviza as bordas
    float alpha = 1.0 - min(min(edge.x, edge.y), edge.z); // Define opacidade nas bordas

    // Cor do wireframe
    gl_FragColor = vec4(0.0, 1.0, 0.0, alpha); // Verde
}