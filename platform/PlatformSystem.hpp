#pragma once
#include <memory>
#include <string>
#include <cassert>
#include <unordered_map>

#include <ISystem/ISystem.hpp>
#include <IRenderer/IRenderer.hpp>
#include <Foundation/IBasicModule.hpp>
#include <Foundation/IFactoryRegistry.hpp>
#include <Foundation/IType.hpp>

#include "FactoryRegistryImpl.hpp"

#include <windows.h>

#define DLL_MODULE_INIT_ISYSTEM "ModuleInitISystem"
#define RENDERER_DLL_NAME "renderer"
#define DLL_MODULE void*

class PlatformSystem : public Foundation::ISystem
{
public:
	bool Initialize(Foundation::SystemInitializeParams& startupParams) override
	{
		m_Env.System = this;
		m_Env.IsEditor = startupParams.IsEditor;

		startupParams.System = this;

		if (!InitRenderer(startupParams))
		{
			assert(false);
		}

		return true;
	}

	IFactoryRegistry* GetFactoryRegistry() const override
	{
		return &FactoryRegistryImpl::Access();
	}

	SystemGlobalEnvironment* GetGlobalEnvironment() override
	{
		return &m_Env;
	}

	bool InitRenderer(Foundation::SystemInitializeParams& startupParams)
	{
		bool isSuccessful = InitializeEngineModule(startupParams, RENDERER_DLL_NAME, Type::iidof<Foundation::IRendererModule>(), true);
		return isSuccessful;
	}

private:
	Foundation::SystemGlobalEnvironment m_Env{ };
	std::string m_PhysicsLibraryDLLName{ "" };
	std::unordered_map<std::string, DLL_MODULE> m_DllModules;

	template<class T>
	bool CreateInstanceWithInterfaceType(InterfaceID const& id, std::shared_ptr<T>& module)
	{
		assert(m_Env.System);

		IFactoryRegistry* factoryRegistry = m_Env.System->GetFactoryRegistry();
		if (factoryRegistry)
		{
			size_t count = 1;
			IFactory* factory = 0;
			factoryRegistry->IterateFactories(id, &factory, count);
			if (count == 1 && factory)
			{
				auto classInstance = factory->CreateClassInstance();
				std::shared_ptr<T> pointer = TypeCast::interface_cast<T>(classInstance);
				if (pointer)
				{
					module = pointer;
				}
			}
		}

		return module.get() != nullptr;
	}

	bool InitializeEngineModule(Foundation::SystemInitializeParams& startupParams, const char* dllName,
		InterfaceID const& interfaceID, bool emptyModuleIsError)
	{
		bool libraryHasBeenLoaded{ false };

		std::shared_ptr<Foundation::IBasicModule> module;
		bool isSuccessful = CreateInstanceWithInterfaceType(interfaceID, module);
		if (!isSuccessful)
		{
			// need to load the dll
			auto dllHasBeenLoaded = LoadDDL(dllName);
			if (!dllHasBeenLoaded)
			{
				return false;
			}

			libraryHasBeenLoaded = true;
			isSuccessful = CreateInstanceWithInterfaceType(interfaceID, module);
		}

		if (!isSuccessful && module == nullptr)
		{
			return false;
		}

		auto moduleHasBeenInitialized = module->Initialize(m_Env, startupParams);
		if (!moduleHasBeenInitialized)
		{
			// TODO: cleanup - unload and etc
			return false;
		}

		return true;
	}

	bool LoadDDL(const char* dllName)
	{
		std::string fullDllName = dllName + std::string(".dll");
		auto module = ::LoadLibraryA(fullDllName.c_str());
		if (module == nullptr)
		{
			// TODO: I have only important once now
			assert(false);
		}

		m_DllModules.emplace(fullDllName, module);

		typedef void* (*PtrFunc_ModuleInitISystem)(ISystem* system, const char* moduleName);
		PtrFunc_ModuleInitISystem pfnModuleInitISystem = (PtrFunc_ModuleInitISystem)GetProcAddress(module, DLL_MODULE_INIT_ISYSTEM);
		if (pfnModuleInitISystem)
		{
			pfnModuleInitISystem(this, dllName);
		}

		return true;
	}
};