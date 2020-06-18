//in vec3 colour;
uniform sampler2D texture1;
//Texture coordinate
//in vec2 texCoord;

out vec4 frag_colour;


uniform float baseline_alpha;



//Final color
//out vec4 gl_FragColor;


void main () {

  vec4 color =  texture2D(texture1, gl_TexCoord[0].st);// * vec4 (1.0, 1.0, 1.0, baseline_alpha);


  	vec4 f4 = color * vec4(0.299, 0.587, 0.114, 1.0);
	float f = (round(((f4.r + f4.g + f4.b)/85)*255.0)*85)/255.0;
	frag_colour.r = color.a;
	frag_colour.g = 0;
	frag_colour.b = 0;
	frag_colour = color;

  //gl_FragColor =vec4 (1.0,1.0,0.0, 1.0);// texture( LTextureUnit, texCoord ) * 

	float frequency = 200.0;
	float offset = 0.0;
	vec2 texCoord = gl_TexCoord[0].st;
    float global_pos = (texCoord.y + offset) * frequency;
    float wave_pos = cos((fract( global_pos ) - 0.5)*3.14);
    vec4 pel = texture2D( texture1, texCoord );

    frag_colour = color;//mix(vec4(0,0,0,0), pel, wave_pos);

}


/*


const float dotSpace = 4.0;
const float dotSize = 2.0;

const float sinPer = 3.141592 / dotSpace;
const float frac = dotSize / dotSpace;

void main( )
{
   vec2 fragCoord = gl_TexCoord[0].xy;
	vec4 color =  texture2D(texture1, gl_TexCoord[0].st);// * vec4 (1.0, 1.0, 1.0, baseline_alpha);

    float varyX = (abs(sin(sinPer * fragCoord.x)) - frac);
    float varyY = (abs(sin(sinPer * fragCoord.y)) - frac);
    float pointX = floor(fragCoord.x / dotSpace) * dotSpace + (0.5 * dotSpace);
    float pointY = floor(fragCoord.y / dotSpace) * dotSpace + (0.5 * dotSpace);
	frag_colour = (texture(texture1, vec2(pointX, pointY) / vec2(256,256)) * varyX * varyY) * (2.0/frac);
}*/