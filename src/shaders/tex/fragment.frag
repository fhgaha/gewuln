#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
   // vec4 a_vec = texture(texture_diffuse1, TexCoords);
   // if (a_vec.a < 0.5) {
   //    discard;
   // }
   
   // FragColor = a_vec;
   
   ///////////////////
   FragColor = texture(texture_diffuse1, TexCoords);
   
}
