//layout(location = 0) in vec2 vertex_position;
//layout(location = 1) in vec3 vertex_colour;

//out vec3 colour;

//Vertex position attribute
in vec2 LVertexPos2D;

//Texture coordinate attribute
//in vec2 LTexCoord;
//out vec2 texCoord;



void main () {
  //colour = vertex_colour;
  //texCoord = LTexCoord;
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_Position = gl_ModelViewProjectionMatrix  * vec4(LVertexPos2D,0.0, 1.0);
}