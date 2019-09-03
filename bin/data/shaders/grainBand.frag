#version 410

#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2DRect inTex;
uniform sampler2DRect maskTex;
uniform vec2 u_resolution;
uniform bool invert;
uniform float gridSize;

out vec4 fragColor;

// #define gridSize 50;

float luma(vec3 col){
    float light = 0.2126*col.r + 0.7152*col.g + 0.0722*col.b;
    return light;
}


float circle(in vec2 _st, in float _rad) {
    vec2 dist = _st - vec2(0.5);
    return 1. - smoothstep(_rad-0.001, _rad+0.001, dot(dist, dist)*5.);
}


void main() {
    vec2 st = gl_FragCoord.xy;
    // st.x = u_resolution.x-st.x;

    // grid
    float grid = gridSize;
    float aspect = u_resolution.x / u_resolution.y;
    vec2 uv = st * vec2(aspect, 1.) * grid / u_resolution;
    uv = fract(uv);

    vec4 texColor = texture(inTex, st);
    vec4 texMask  = texture(maskTex, st);

    float value = luma(texColor.rgb);
    float rad = 0.8;
    
    float al = invert ? texMask.x : 1 - texMask.x;
    float circs = circle(uv, rad);

    fragColor = vec4(texColor.rgb, al*circs);
}