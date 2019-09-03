#version 410

uniform float u_time;
uniform vec2 u_resolution;

uniform float u_moveSpeed;
uniform float u_colorSpeed;
uniform float u_colorWave;
uniform float u_intensity;

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


// Standard 2x2 hash algorithm.
vec2 hash22(vec2 p) { 
    // Faster, but probably doesn't disperse things as nicely as other ways.
    float n = sin(dot(p,vec2(1, 113))); 
    p = fract(vec2(8.*n, n)*262144.);
    return sin(p*6.2831853 + u_time*2.);
}

float Voronoi3Tap(vec2 p){
  vec2 s = floor(p + (p.x + p.y)*.3660254); // Skew the current point.
  p -= s - (s.x + s.y)*.2113249; // Use it to attain the vector to the base vertice (from p).

  // Determine which triangle we're in -- Much easier to visualize than the 3D version.
  // The following is equivalent to "float i = step(p.y, p.x)," but slightly faster, I hear.
  float i = p.x<p.y ? 0. : 1.;
  
  
  // Vectors to the other two triangle vertices.
  vec2 p1 = p - vec2(i, 1. - i) + .2113249, p2 = p - .5773502; 

  // Add some random gradient offsets to the three vectors above.
  p += hash22(s)*.125;
  p1 += hash22(s +  vec2(i, 1. - i))*.125;
  p2 += hash22(s + 1.)*.125;
  
  // Determine the minimum Euclidean distance. You could try other distance metrics, if you wanted.
  float d = min(min(dot(p, p), dot(p1, p1)), dot(p2, p2))/.425;
  
  // That's all there is to it.
  return sqrt(d); // Take the square root, if you want, but it's not mandatory.
}

void main()
{
  // Screen coordinates.
	vec2 uv = (gl_FragCoord.xy - u_resolution.xy * 0.5) / u_resolution.y;
	uv.y += 0.01 * u_moveSpeed * u_time;
	vec2 st = gl_FragCoord.xy / u_resolution.xy;
  
  // Prerotation to align the grid to the horizontal, but I'm leaving it on a slant for... stylistic purposes. :)
  //const float a = 3.14159/12.;
  //uv *= mat2(cos(a), sin(a), -sin(a), cos(a));
  
  // Take two 3-tap Voronoi samples near one another.
  float c = Voronoi3Tap(uv * 5.);
  float c2 = Voronoi3Tap(uv * 5. - 9./u_resolution.y);
  
  // Coloring the cell.
  // Use the Voronoi value, "c," above to produce a couple of different colors.
  // Mix those colors according to some kind of moving geometric patten.
  // Setting "pattern" to zero or one displays just one of the colors.
  float pattern = cos(uv.x*.75*3.14159 - .9)*cos(uv.y*1.5*3.14159 - .75)*.5 + .5;
  
  // Just to confuse things a little more, two different color schemes are faded in out.
  //
  // Color scheme one - Mixing a firey red with some bio green in a sinusoidal kind of pattern.
  vec3 col = mix(vec3(c*1.3, c*c, pow(c, 10.)), vec3(c*c*.8, c, c*c*.35), pattern );
  // Color scheme two - Farbrausch fr-025 neon, for that disco feel. :)
  vec3 col2 = mix(vec3(c*1.2, pow(c, 8.), c*c), vec3(c*1.3, c*c, pow(c, 10.)), pattern );
  // Alternating between the two color schemes.
  col = mix(col, col2, smoothstep(.4, .6, sin(u_time*.25)*.5 + .5)); // 
  
  // Hilighting.
  // Use a combination of the sample difference to add some really cheap, blueish highlighting.
  // It's a directional-derviative based lighting trick. Interesting, but no substitute for point-lit
  // bump mapping. Comment the following line out to see the regular, flat pattern.
  col += vec3(.5, .8, 1)*(c2*c2*c2 - c*c*c)*5.;
      
  // Speckles.
  // Adding subtle speckling to break things up and give it a less plastic feel.
  col += (length(hash22(uv + u_time))*.06 - .03)*vec3(1, .5, 0);
  

  // Rectangular Vignette.
  uv = gl_FragCoord.xy/u_resolution.xy;
  col *= smoothstep(0., .5, pow(16.*uv.x*uv.y*(1. - uv.x)*(1. - uv.y), .25))*vec3(1.1, 1.07, 1.01);
  
  
  // Approximate gamma correction.
	col = sqrt(max(col, 0.));
  
  // hue waves
  vec3 hsvCol = rgb2hsv(col.rgb);
	hsvCol.x += 0.5 * u_colorSpeed * u_time;
	hsvCol.x += u_colorWave * (sin(st.x-0.5) + cos(st.y-1.));

	fragColor = vec4(hsv2rgb(hsvCol), 1.);
}