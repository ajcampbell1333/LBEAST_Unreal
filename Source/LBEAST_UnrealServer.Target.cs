// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

using UnrealBuildTool;
using System.Collections.Generic;

public class LBEAST_UnrealServerTarget : TargetRules
{
	public LBEAST_UnrealServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("LBEAST_Unreal");

		// Server-specific optimizations
		bUseLoggingInShipping = true;
		bCompileWithAccessibilitySupport = false;
		bCompileAgainstEngine = true;
		bCompileAgainstCoreUObject = true;
		
		// Disable unnecessary features for headless server
		bBuildDeveloperTools = false;
		bBuildWithEditorOnlyData = false;
	}
}

