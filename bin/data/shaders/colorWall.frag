#version 410

uniform float u_time;
uniform vec2 u_resolution;

uniform float u_moveSpeed;
uniform float u_colorSpeed;
uniform float u_colorWave;
uniform float u_intensity;

out vec4 fragColor;

#define grid_size 10.0
#define glow_size 2.0
#define glow_br   0.2
#define time_step 10.0


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


float random(vec2 seed)
{
	float value = fract(sin(dot(seed ,vec2(12.9898,78.233))) * 43758.5453);
	return value;
}

float step_to(float value, float steps)
{
	float  closest_int = floor(value / steps);
	return closest_int * steps;
}

vec4 dot_grid(vec2 pos, bool with_grid)
{
	float value = floor(mod(pos.x,grid_size)) * floor(mod(pos.y,grid_size));		
	value		= clamp(value, 0.0, 1.0);
	
	float c_time	= u_time / time_step;
	
	vec2 step_pos	= vec2(step_to(pos.x , grid_size), step_to(pos.y, grid_size));
	vec2 norm_pos	= step_pos.xy / u_resolution.xy;
	
	norm_pos	= vec2(norm_pos.x + random(norm_pos), norm_pos.y + random(norm_pos ));
	
	float r = fract(sin(norm_pos.x ));
	float g = fract(sin(norm_pos.y + abs(c_time) ));
	float b = abs(r-g);
	
	if(with_grid == false)
	{
		value = 1.0;	
	}
	
	return vec4(r,g,b,1.0) * value;
}

vec4 glow(vec2 pos)
{
	vec4 color 	=  clamp(dot_grid(pos, true) * glow_br, 0.0, 1.0);
	color		+= clamp(dot_grid(vec2(pos.x - u_intensity,pos.y),false) * glow_br, 0.0, 1.0);
	color		+= clamp(dot_grid(vec2(pos.x + u_intensity,pos.y),false) * glow_br, 0.0, 1.0);
	
	color		+= clamp(dot_grid(vec2(pos.x,pos.y - u_intensity),false ) * glow_br, 0.0, 1.0);
	color		+= clamp(dot_grid(vec2(pos.x,pos.y + u_intensity),false ) * glow_br, 0.0, 1.0);
	
	return color;
}

void main( void ) 
{
	vec2 uv = gl_FragCoord.xy;
	uv.y += 0.1 * u_moveSpeed*u_moveSpeed * u_time*10.;
	vec2 st = gl_FragCoord.xy / u_resolution.xy;
	
    vec4 col = glow(uv);

	// hue waves
	vec3 hsvCol = rgb2hsv(col.rgb);
	hsvCol.x += 0.5 * u_colorSpeed * u_time;
	hsvCol.x += u_colorWave * (sin(st.x-0.5) + cos(st.y-1.));

	fragColor = vec4(hsv2rgb(hsvCol), col.a);
}