// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

using UnrealBuildTool;

public class LBEASTCore : ModuleRules
{
	public LBEASTCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"HeadMountedDisplay",
				"XRBase",
				"EnhancedInput",
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
				"Sockets",
				"Networking"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"EmbeddedSystems",      // For UEmbeddedDeviceController (used in .cpp only)
				"LBEASTExperiences"     // For ALBEASTExperienceBase (used in .cpp only)
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);

		// OpenXR Support
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PrivateDependencyModuleNames.AddRange(new string[] { "OpenXRHMD", "OpenXRInput" });
		}
	}
}


