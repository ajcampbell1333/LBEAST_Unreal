// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

using UnrealBuildTool;

public class LBEASTExperiences : ModuleRules
{
	public LBEASTExperiences(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"LBEASTCommon",  // Interfaces and shared types
				"LBEASTCore"      // Implementation classes (InputAdapter, ServerCommandProtocol, etc.)
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"AIFacemask",      // Implementation details - not exposed in public API
				"LargeHaptics",    // Implementation details - not exposed in public API
				"EmbeddedSystems"  // Implementation details - not exposed in public API
			}
		);
	}
}




