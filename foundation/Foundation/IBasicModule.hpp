#pragma once

#include "IType.hpp"
#include "Class.h"

namespace Foundation
{
	struct SystemGlobalEnvironment;
	struct SystemInitializeParams;

	class IBasicModule : public IUnknownType
	{
	public:
		GUID(IBasicModule, "default"_generate_guid);

		virtual const char* GetName() = 0;

		virtual bool Initialize(Foundation::SystemGlobalEnvironment& environment, Foundation::SystemInitializeParams& initializeParams) = 0;
	};
}