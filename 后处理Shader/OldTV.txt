float density = 1.3;
float opacityScanline = .3;
float opacityNoise = .2;
float flickering = 0.03;

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float blend(const in float x, const in float y) {
	return (x < 0.5) ? (2.0 * x * y) : (1.0 - 2.0 * (1.0 - x) * (1.0 - y));
}

vec3 blend(const in vec3 x, const in vec3 y, const in float opacity) {
	vec3 z = vec3(blend(x.r, y.r), blend(x.g, y.g), blend(x.b, y.b));
	return z * opacity + x * (1.0 - opacity);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;
    vec3 col = texture(	iChannel0,uv).rgb;
    
    float count = iResolution.y * density;
    vec2 sl = vec2(sin(uv.y * count), cos(uv.y * count));
	vec3 scanlines = vec3(sl.x, sl.y, sl.x);

    col += col * scanlines * opacityScanline;
    col += col * vec3(random(uv*iTime)) * opacityNoise;
    col += col * sin(110.0*iTime) * flickering;


    fragColor = vec4(col,1.0);
}