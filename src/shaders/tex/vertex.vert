#version 330 core
layout (location = 0) in vec3 aPos; // attribute position
layout (location = 1) in vec3 aColor;  // attribute color
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
   gl_Position = vec4(aPos, 1.0);
   ourColor = aColor;
   TexCoord = aTexCoord;
}
