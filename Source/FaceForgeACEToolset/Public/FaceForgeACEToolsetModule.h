#pragma once

#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"

FACEFORGEACETOOLSET_API DECLARE_LOG_CATEGORY_EXTERN(LogACEToolset, Log, All);

/**
 * Registers the Audio2Face toolset with the toolset registry.
 *
 * Its own plugin rather than part of FaceForgeACE, and that is the rule rather than a preference:
 * ToolsetRegistry is Experimental, so folding the adapter into the capability would mean the
 * capability refuses to load anywhere the experimental plugins are switched off.
 */
class FFaceForgeACEToolsetModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	bool bRegistered = false;
};
