#ifndef TEXTURE_H
#define TEXTURE_H


#include <string>

class Texture
{
public:
	unsigned int id;
	//"texture_diffuse" or "texture_specular"
	std::string type;
};

#endif