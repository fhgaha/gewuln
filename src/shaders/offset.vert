#version 330 core
layout (location = 0) in vec3 aPos; // attribute position
layout (location = 1) in vec3 aColor;  // attribute color
out vec3 ourColor;
uniform float offset;

void main()
{
   gl_Position = vec4(aPos.x + offset, aPos.y, aPos.z, 1.0);
   // gl_Position = vec4(aPos, 1.0);;
   ourColor = aColor;
}
