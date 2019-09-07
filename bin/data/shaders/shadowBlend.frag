#version 410

uniform sampler2DRect inTex;
uniform sampler2DRect unlitTex;
uniform sampler2DRect shadwTex;

out vec4 fragColor;


float luma(vec3 c){
    float light = 0.2126*c.r + 0.7152*c.g + 0.0722*c.b;
    return light;
}
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


void main(){
    vec2 st = gl_FragCoord.xy;

    vec4 texColor = texture(inTex, st);
    vec4 texUnlit = texture(unlitTex, st);
	vec4 texShadw = texture(shadwTex, st);

	vec3 texHsv = rgb2hsv(texColor.rgb);
	float unlitLuma = luma(texUnlit.rgb);
	float shadwLuma = luma(texShadw.rgb);

	texHsv.z = texHsv.z - abs(unlitLuma - shadwLuma);

    fragColor = vec4(hsv2rgb(texHsv), texUnlit.a);
}