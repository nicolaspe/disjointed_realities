#version 410

uniform sampler2DRect inTex;
uniform float u_hue;
uniform float u_bri;
uniform float u_con;
uniform float u_sat;
uniform float u_opc;

out vec4 fragColor;

vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = c.g < c.b ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);
    vec4 q = c.r < p.x ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


void main() {
    vec2 st = gl_FragCoord.xy;

    // extract original color
    vec4 texColor = texture(inTex, st);

    // contrast
    vec3 col_1 = (texColor.rgb + u_bri) * u_con;
    vec3 col_2 = mix(texColor.rgb, col_1, u_opc);

    // hsv operations 
    vec3 hsvColor = rgb2hsv(col_2);
    hsvColor.x += u_hue; // hue shift
    hsvColor.y += u_sat; // sat shift
    
    // revert to rgb and return
    vec3 rgbColor = hsv2rgb(hsvColor);
    fragColor = vec4(rgbColor, texColor.a);
}