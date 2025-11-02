// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "LargeHaptics.h"

#define LOCTEXT_NAMESPACE "FLargeHapticsModule"

void FLargeHapticsModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("LBEAST LargeHaptics Module: Startup"));
}

void FLargeHapticsModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("LBEAST LargeHaptics Module: Shutdown"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLargeHapticsModule, LargeHaptics)





