#include "model_renderer.h"

ModelRenderer::ModelRenderer(Shader & shader)
{
	this->shader = shader;
	initRenderData();
}

ModelRenderer::~ModelRenderer()
{
}

void ModelRenderer::DrawModel(Model & model, glm::vec3 pos)
{
	model.Draw(shader);
}

void ModelRenderer::initRenderData()
{
	
}
