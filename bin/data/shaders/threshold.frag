#version 410

uniform sampler2DRect inTex;
uniform float limit;
uniform vec3 targetColor;

out vec4 fragColor;

float luma(vec3 col){
    float light = 0.2126*col.r + 0.7152*col.g + 0.0722*col.b;
    return light;
}

void main(){
    vec2 st = gl_FragCoord.xy;

    vec4 texColor = texture(inTex, st);
    float value = luma(texColor.rgb);
    float th = value > limit ? 1. : 0.;

    fragColor = vec4(vec3(th), 1.);
}