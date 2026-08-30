using UnrealBuildTool;

public class FaceForgeACEToolset : ModuleRules
{
	public FaceForgeACEToolset(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"ToolsetRegistry",   // UToolsetDefinition and UAgentSkill are public base classes
				"FaceForgeACE",      // the capability this exposes, and the types in its signatures
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",   // RaiseScriptError comes from KismetSystemLibrary
			}
			);

		// As with every other toolset in this family: no dependency on ModelContextProtocol. Tools
		// register with ToolsetRegistry and MCP picks them up from there.
	}
}
