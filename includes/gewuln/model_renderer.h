#include "shader.h"
#include "model.h"

class ModelRenderer {
	public:
		ModelRenderer(Shader &shader);
		~ModelRenderer();
		
		void DrawModel(Model &model, glm::vec3 pos);
	private:
		Shader	shader;
		
		void initRenderData();
};