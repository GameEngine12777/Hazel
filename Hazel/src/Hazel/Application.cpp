#include "Application.h"

Hazel::Application::Application()
{
}

Hazel::Application::~Application()
{
}

void Hazel::Application::Run()
{
	while (true);
}

Hazel::Application* Hazel::CreateApplication()
{
	return new Application();
}
