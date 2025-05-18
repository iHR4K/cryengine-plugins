#pragma once
#include "../Foundation/IBasicModule.hpp"

namespace Foundation
{
	class IRenderer 
	{

	};

	struct IRendererModule : public Foundation::IBasicModule
	{
		GUID(IRendererModule, "IRendererModule"_generate_guid);
	};
}