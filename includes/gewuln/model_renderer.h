#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <vector>
#include "shader.h"
#include "mesh.h"
#include "model.h"


class ModelRenderer
{
	
public:
	ModelRenderer(Shader &shader): shader(shader){}
	
	void Draw(Model& model)
	{
		for(unsigned int i = 0; i < model.meshes.size(); i++){
			model.meshes[i].Draw(shader);
		}
	}

private:
	
	Shader &shader;
};

#endif