#version 410

uniform float u_time;
uniform vec2 u_resolution;

out vec4 fragColor;

void main(){
    vec2 st = gl_FragCoord.xy / u_resolution.xy;

    float g = (sin(u_time) + 1.) / 2.;

    fragColor = vec4(st.x, g, st.y, 1.);
}