#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
   vec4 t1 = texture(texture1, TexCoord);
   vec2 t2_quater = vec2(TexCoord.x * 4.0, TexCoord.y * 4.0);
   vec4 t2 = texture(texture2, t2_quater);
   FragColor = mix(t1, t2, 0.2);
}
