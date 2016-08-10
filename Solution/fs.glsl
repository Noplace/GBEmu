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
	frag_colour.r = 0;
	frag_colour.g = f;
	frag_colour.b = 0;
	//frag_colour = color;
  //gl_FragColor =vec4 (1.0,1.0,0.0, 1.0);// texture( LTextureUnit, texCoord ) * 
}