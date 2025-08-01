#version 460

in vec2 TexCoords;
in vec3 Normal;
in vec4 someVec4;

uniform sampler2D texture_diffuse1;

uniform bool drawing_wireframe;
uniform vec3 wireframe_color;


out vec4 FragColor;


void main()
{
   // FragColor = vec4(Normal, 1.0f);  // show normals
   
   if (drawing_wireframe) {
      FragColor = vec4(wireframe_color, 1.0);      
   } else {
      FragColor = texture(texture_diffuse1, TexCoords);
   }
}
