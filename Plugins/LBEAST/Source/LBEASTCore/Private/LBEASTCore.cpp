// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "LBEASTCore.h"

#define LOCTEXT_NAMESPACE "FLBEASTCoreModule"

void FLBEASTCoreModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogTemp, Log, TEXT("LBEAST Core Module: Startup"));
}

void FLBEASTCoreModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Log, TEXT("LBEAST Core Module: Shutdown"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLBEASTCoreModule, LBEASTCore)



