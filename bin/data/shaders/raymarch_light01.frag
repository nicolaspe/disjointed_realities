#version 410

//#extension GL_OES_standard_derivatives : enable 

#ifdef GL_ES
precision mediump float;
#endif

// Light Accumulation
// Source - Raymarching.com
// Author - Gary "Shane" Warne
// eMail - mail@Raymarching.com, mail@Labyrinth.com
// Last update: 25th Jun, 2015


uniform float time;
uniform vec2 resolution;
uniform sampler2D texChannel0;

out vec4 fragColor;

// 2D rotation.
mat2 rot(float th){
    float cs = cos(th), si = sin(th);
    return mat2(cs, -si, si, cs);    
}

// Standard hash algorithm that you'll see all over the place. Range: [-1, 1].
vec3 hash33(vec3 p) { 

    // Basically, this is a tweaked amalgamation I put together, based on a couple of other random algorithms 
    // I've seen around... so use it with caution, because I make a tonne of mistakes. :)
    float n = sin(dot(p, vec3(7.0, 157., 113.)));
    return fract(vec3(64.0*n, 8.0*n, n)*32768.0)*2.-1.;
}

// Box
float box( vec3 p, vec3 b ){ vec3 d = max(abs(p) - b, 0.); return  sqrt(dot(d, d)); }

// Rounded cube.
float roundedCube( vec3 p, vec3 b , float edge){ vec3 d = max(abs(p) - b, 0.); return  sqrt(dot(d, d)) - edge; }

float map(vec3 p) {

    vec3 sv = sin(p*32.);
     
    p = fract(p) - 0.5;   

    // Bumpy spheres. Cliche, but great for raymarching demonstrations. :)
    return length(p) - 0.2 + sv.x*sv.y*sv.z*0.02;
    
    // Some other things to try:
    
    // Rounded cube field.
    //return roundedCube(p, vec3(0.15), 0.035);
    
    // Box field.
    //return box(p, vec3(0.18));    
    
    /*
    // Convoluted way to produce a ball-jointed, tapered lattice.
     
    p = fract(p) - 0.5;
    
    vec3 sv = cos(p*6.2831853);

    p *= p;

    p += p.yzx;

    float m = sqrt(min(min(p.x, p.y), p.z))-0.05;

    return min(m, dot(p, vec3(0.5))-0.015) - sv.x*sv.y*sv.z*0.02;
    */
    

}

// With a normal raymarching function, you inch the ray forward until you either hit something, or reach a maximum distance.
// The end result is then used to calculate the pixel color. This is similar. However, pixel colors are calculated "every"
// time the ray is moved forward through the space, not just at the end. The final color returned is the accumulation of
// all color values. Conceptually, it's pretty simple, but requires more calculation. 
vec3 raymarch(vec3 ro, vec3 rd) {
	
	float t = 0.0;
	
	vec3 col = vec3(0.);
    
    // You could use more iterations, but I prefer to keep the number down. Obviously, it keeps calculations down,
    // but it also gives a blurrier, more abstact result, which I kind of like.
    for (int i = 0; i < 56; i++) {
        
        
        vec3 pos = ro + rd*t;
        float d = map(pos);
        
        if (d<0.005 || t>20.) break;
        
        // Normally, you'd just jump a fraction of the maximum distance, "d." However, by forcing a minimum step, more
        // color values can be gathered along the way, which looks a little smoother.
        float td = min(0.25, d);
        t += td * 0.5;
        
        // How you calculate color is entirely up to you. In this case, I've used the distance from the starting point, "d,"
        // to calculate some falloff, then the current ray position to index into a texture. Each have been combined to
        // provide the current pixel color, which is then added to the final color.
        
        // Light attenuation, based on the distance from "ro."
        float atten = max(1.-0.2/max(d, 0.001), 0.)*0.5;
        //float atten = min(0.2/max(d, 0.001), 1.)*0.15;
        
        // Adds a bit of jitter, which helps disguise texture artifacts due to cheaping out. I also kind of like the way 
        // it looks. Not necessary here, though.      
        //pos += hash33(pos)*0.03;
        
        // Use the current position to retrieve a texture value. If you wanted, you could do something more sophisticated, 
        // like calculate the surface normal (analytically, if possible), then do a tri-planar texture lookup, etc. However, 
        // the following does an OK job, so it'll do. In the end, who cares, so long as the final result looks pretty. :)
        vec3 texCol = texture2D(texChannel0, (pos.xy + pos.zy + pos.zx)/32.).xyz;
       
        // Combine the texel and attenuation, then add it to our final color.
        col += texCol*atten;
        
        // Not absolutely necessary, but a bit of ray warping can make things look more interesting.
        mat2 rt = rot(0.03*td);
        rd.xy *= rt;
        //rd.yz *= rt;
        rd.zx *= rt;
        
    }
    
    // Return the final color.
    return col;
}

void main( void ){
    
    // Screen coordinates.
	vec2 uv = (gl_FragCoord.xy - resolution.xy*0.5) / resolution.y;
    
    // Normalized directional ray. The line directly below gives a mild 
    // fish lens effect, and the one below it is the non-warped version. 
    // Use whichever one you prefer.
    vec3 rd = normalize(vec3(uv, sqrt(1.-dot(uv, uv)*0.5)*0.5));
    //vec3 rd = normalize(vec3(uv, 0.5));
    
    // Rotate the directional ray to give the observer a look around.
    // I think of it as a cheap, lazy-man's camera. :)
    rd.xy *= rot(time*0.5);
    rd.xz *= rot(time*0.25); // Extra variance.
    
    // Our startoff postion, which moves forward in the Z-direction.
    vec3 ro = vec3(0.0, 0.0, time*2.0);
    
    // Calculate the color.
    //
    // You can do way more than just return a color, but we're keeping
    // things simple.
    vec3 sCol = raymarch(ro, rd);
    
    // Put it on the screen. Too easy. :)
	fragColor = vec4(clamp(sCol, 0., 1.), 1.0);
}
