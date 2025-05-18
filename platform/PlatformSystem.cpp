#include "PlatformSystem.hpp"

#include <Foundation/Module_impl.hpp>


extern "C"
{
	DLL_EXPORT Foundation::ISystem* CreateSystemInterface(Foundation::SystemInitializeParams& startupParams)
	{
		std::unique_ptr<PlatformSystem> pSystem = nullptr;
		{
			pSystem = std::make_unique<PlatformSystem>();
			// TODO: who should hold this reference after this call
			startupParams.System = pSystem.get();
		}

		ModuleInitISystem(pSystem.get(), "PlatformSystem");

		if (!pSystem->Initialize(startupParams))
		{
			// error 
			gEnv->System = nullptr;
			return nullptr;
		}

		return pSystem.release();
	}
}