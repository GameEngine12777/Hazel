#include <Hazel.h>


class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}

};

Hazel::Application* Hazel::CreateApplication()
{
	printf("Create Sandbox!!\n");
	return new Sandbox();
}