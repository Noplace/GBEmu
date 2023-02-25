in vec2 LVertexPos2D;
uniform sampler2D texture1;

out vec4 frag_colour;
uniform float baseline_alpha;
uniform float baseline_alpha2;

[vertex]
void main () {
  //colour = vertex_colour;
  //texCoord = LTexCoord;
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_Position = gl_ModelViewProjectionMatrix  * vec4(LVertexPos2D,0.0, 1.0);
}





[fragment]
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

