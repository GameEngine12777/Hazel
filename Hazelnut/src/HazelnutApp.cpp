#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Hazel {

	class Hazelnut : public Application
	{
	public:
		Hazelnut(const Hazel::ApplicationSpecification& specification)
			: Application(specification)
		{
			PushLayer(new EditorLayer());
		}

		~Hazelnut()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Hazelnut";
		// spec.WorkingDirectory = "../Hazelnut";
		spec.CommandLineArgs = args;

		return new Hazelnut(spec);
	}

}
