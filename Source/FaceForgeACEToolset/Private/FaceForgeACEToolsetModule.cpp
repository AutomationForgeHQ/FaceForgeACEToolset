#include "FaceForgeACEToolsetModule.h"

#include "ACEToolset.h"
#include "ToolsetRegistry/UToolsetRegistry.h"

DEFINE_LOG_CATEGORY(LogACEToolset);

#define LOCTEXT_NAMESPACE "FFaceForgeACEToolsetModule"

void FFaceForgeACEToolsetModule::StartupModule()
{
	if (!UToolsetRegistry::IsAvailable())
	{
		// Expected outside the editor, and whenever the experimental plugins are off. Not an error.
		UE_LOG(LogACEToolset, Log,
			TEXT("Toolset registry unavailable - Audio2Face tools not registered."));
		return;
	}

	if (UToolsetRegistry::IsToolsetClassRegistered(UACEToolset::StaticClass()))
	{
		bRegistered = true;
		return;
	}

	UToolsetRegistry::RegisterToolsetClass(UACEToolset::StaticClass());
	bRegistered = UToolsetRegistry::IsToolsetClassRegistered(UACEToolset::StaticClass());

	// Worth logging either way. The failure mode this replaces was silent: the module loaded, the
	// class existed, and the tools simply were not there.
	UE_LOG(LogACEToolset, Log, TEXT("Audio2Face toolset %s."),
		bRegistered ? TEXT("registered") : TEXT("failed to register"));

	// The two skills need no registration at all - native UAgentSkill subclasses are discovered.
}

void FFaceForgeACEToolsetModule::ShutdownModule()
{
	if (bRegistered && UToolsetRegistry::IsAvailable())
	{
		UToolsetRegistry::UnregisterToolsetClass(UACEToolset::StaticClass());
		bRegistered = false;
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFaceForgeACEToolsetModule, FaceForgeACEToolset)
