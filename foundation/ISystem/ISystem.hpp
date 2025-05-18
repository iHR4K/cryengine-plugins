#pragma once
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)

class IFactoryRegistry;

namespace Foundation
{
	class ISystem;
	class IRenderer;

	struct SystemGlobalEnvironment
	{
		ISystem* System{ nullptr };
		IRenderer* Renderer{ nullptr };

		bool IsEditor : 1;
	};

	struct SystemDesc
	{
	};

	struct SystemInitializeParams;

	class ISystem
	{
	public:
		virtual ~ISystem() = default;

		virtual bool Initialize(Foundation::SystemInitializeParams& startupParams) = 0;

		virtual IFactoryRegistry* GetFactoryRegistry() const = 0;

		virtual SystemGlobalEnvironment* GetGlobalEnvironment() = 0;

	};

	struct SystemInitializeParams
	{
		Foundation::ISystem* System{ nullptr };
		bool IsEditor : 1;
	};
}

typedef Foundation::ISystem* (*PFNCREATESYSTEMINTERFACE)(Foundation::SystemInitializeParams&);

extern "C"
{
	DLL_EXPORT Foundation::ISystem* CreateSystemInterface(Foundation::SystemInitializeParams&);
}