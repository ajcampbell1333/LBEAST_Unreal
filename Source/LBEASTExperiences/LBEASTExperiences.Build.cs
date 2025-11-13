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
				"LBEASTCore",    // Implementation classes (InputAdapter, ServerCommandProtocol, etc.)
				"Sockets",        // For GoKart ECU UDP communication
				"Networking",     // For GoKart ECU UDP communication
				"EnhancedInput",  // For GoKart input handling
				"InputCore",      // For GoKart input handling
				"Json",           // For GoKart data serialization
				"JsonUtilities"   // For GoKart data serialization
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




