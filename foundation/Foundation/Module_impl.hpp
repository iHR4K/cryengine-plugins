#pragma once
#include <windows.h>

#include "../ISystem/ISystem.hpp"
#include "Library.hpp"
#include "IFactoryRegistry.hpp"

#include <cassert>

struct SRegFactoryNode* g_pHeadToRegFactories = nullptr;
struct Foundation::SystemGlobalEnvironment* gEnv = nullptr;


extern "C"
{
	DLL_EXPORT void ModuleInitISystem(Foundation::ISystem* pSystem, const char* moduleName)
	{
		if (gEnv) // Already registered.
			return;

		if (pSystem)
			gEnv = pSystem->GetGlobalEnvironment();

		if (pSystem)
		{
			IFactoryRegistryImpl* pFactoryImpl = static_cast<IFactoryRegistryImpl*>(pSystem->GetFactoryRegistry());
			pFactoryImpl->RegisterFactories(g_pHeadToRegFactories);

		}
	}
}

#define SystemLibraryName "platform.dll"

bool InitializeEngine(Foundation::SystemInitializeParams& startupParams, bool externalMainLoop)
{
	Library library{ SystemLibraryName };

	PFNCREATESYSTEMINTERFACE createSystemInterface = library.GetProcedureAddress<PFNCREATESYSTEMINTERFACE>("CreateSystemInterface");
	assert(createSystemInterface);
	if (createSystemInterface == nullptr)
	{
		return false;
	}

	// NOTE: be careful here - you should pass the parameter which will extend lifetime of the system library in the Editor's case
	if (createSystemInterface(startupParams) != nullptr)
	{
		if (externalMainLoop)
		{
			library.Release();
		}
	}
	else
	{
		return false;
	}

	return true;
}