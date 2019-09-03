#version 410

uniform sampler2DRect inTex;
uniform float intensity;

out vec4 fragColor;


void main(){
    vec2 st = gl_FragCoord.xy;
    vec2 pix_st = floor(st/intensity) * intensity;

    vec4 texColor = texture(inTex, pix_st);

    fragColor = vec4(texColor.rgb, 1.);
}