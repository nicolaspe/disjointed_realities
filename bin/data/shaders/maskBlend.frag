#version 410

uniform sampler2DRect inTex;
uniform sampler2DRect maskTex;

out vec4 fragColor;


void main(){
    vec2 st = gl_FragCoord.xy;

    vec4 texColor = texture(inTex, st);
    vec4 texMask  = texture(maskTex, st);

    fragColor = vec4(texColor.rgb, texMask.x);
}