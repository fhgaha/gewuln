#ifndef TEXTURE_H
#define TEXTURE_H


#include <string>

class Texture
{
public:
	unsigned int id;
	std::string type; //"texture_diffuse" or "texture_specular"
	std::string path; // store path of texture to compare with other textures
};

#endif