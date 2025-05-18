#include "Editor.hpp"

#include <ISystem/ISystem.hpp>
#include <Foundation/Module_impl.hpp>


bool Editor::Initialize()
{
	Foundation::SystemInitializeParams params{ .IsEditor = true };

	// TODO: pass more about the features as arguments here, and read config if needed here
	const bool externalMainLoop = true;
	if (!InitializeEngine(params, externalMainLoop))
	{
		assert(false, "InitializeEngine failed");
		return false;
	}

	Foundation::ISystem* system = params.System;
	assert(system);

	return false;
}