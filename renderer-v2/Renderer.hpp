#pragma once
#include <ISystem/ISystem.hpp>
#include <IRenderer/IRenderer.hpp>
#include <Foundation/IBasicModule.hpp>


class Renderer : public Foundation::IRenderer 
{
public:
	Renderer();
};

class RendererModule : public Foundation::IRendererModule
{
public:
	CRYINTERFACE_BEGIN()
		CRYINTERFACE_ADD(Foundation::IBasicModule)
		CRYINTERFACE_ADD(IRendererModule)
		CRYINTERFACE_END()

		CRYGENERATE_SINGLETONCLASS_GUID(RendererModule, "RendererModule", "rendering-module"_generate_guid)
		
		virtual const char* GetName() override { return "RendererModule"; };

	bool Initialize(Foundation::SystemGlobalEnvironment& environment, Foundation::SystemInitializeParams& initializeParams) override
	{
		auto renderer = new Renderer();
		environment.Renderer = renderer;
		return true;
	}
};

CRYREGISTER_SINGLETON_CLASS(RendererModule)