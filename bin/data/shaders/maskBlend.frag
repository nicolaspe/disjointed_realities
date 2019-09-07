#version 410

uniform sampler2DRect inTex;
uniform sampler2DRect maskTex;

out vec4 fragColor;


void main(){
    vec2 st = gl_FragCoord.xy;

    vec4 texColor = texture(inTex, st);
    vec4 texMask  = texture(maskTex, st);
	float a = texMask.x > .5 ? 1. : 0.;

    //fragColor = vec4(texColor.rgb, a);
	fragColor = texColor;
}