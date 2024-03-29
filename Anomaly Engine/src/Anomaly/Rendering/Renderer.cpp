#include "aepch.h"
#include "Renderer.h"

namespace Anomaly
{

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
	
	void Renderer::BeginScene(PerspecCamera& camera)
	{
		//m_SceneData->ViewProjMatrix = camera.GetViewProjMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submission(const std::shared_ptr<VertexArray>& vertexarray, const std::shared_ptr<Shader>& shader)
	{
		RenderRequest::DrawCall(vertexarray, shader);
	}

	void Renderer::Submission(Model& model, const std::shared_ptr<Shader>& shader)
	{	
		model.Draw(shader);
	}
}
