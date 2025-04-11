#include <Hazel.h>
#include "Hazel/Core/EntryPoint.h"

#include "ExampleLayer.h"
#include "Sandbox2D.h"


/**
* 
*/
class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		// PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{

	}

};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}