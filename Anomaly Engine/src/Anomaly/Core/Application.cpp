#include "aepch.h"
#include "Application.h"
#include "Log.h"

#include "../Rendering/Renderer.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "glad/glad.h"

namespace Anomaly
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	
	Application* Application::s_Instance = nullptr;

	//This creates a new window
	Application::Application()
	{
		AE_CORE_ASSERT(!s_Instance, "Application already exists!")
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		m_Window->SetVSync(true);

		glEnable(GL_DEPTH_TEST);
		//Culling faces
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);  
		glFrontFace(GL_CW);
		
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
		
	}

	Application::~Application() = default;

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* Overlay)
	{
		m_LayerStack.PushOverlay(Overlay);
		Overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}
	void Application::Run()
	{				
		double lastTime = glfwGetTime();
		int Frames = 0;
		
		while (m_Running)
		{
			float time = static_cast<float>(glfwGetTime());
			TimeStep DeltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;
			DeltaTime.SetglfwTime(time);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(DeltaTime);

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();

			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
