#version 410

uniform sampler2DRect inTex;
uniform sampler2DRect maskTex;
uniform vec2 u_resolution;
uniform bool invert;
uniform float gridSize;

out vec4 fragColor;


void main(){
    vec2 st = gl_FragCoord.xy;

    vec4 texColor = texture(inTex, st);
    vec4 texMask  = texture(maskTex, st);
    float al = invert ? texMask.x : 1 - texMask.x;

    fragColor = vec4(texColor.rgb, al);
}