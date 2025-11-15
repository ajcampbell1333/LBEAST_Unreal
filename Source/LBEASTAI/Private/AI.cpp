// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "AI.h"

#define LOCTEXT_NAMESPACE "FLBEASTAIModule"

void FLBEASTAIModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("LBEAST AI Module: Startup"));
}

void FLBEASTAIModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("LBEAST AI Module: Shutdown"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLBEASTAIModule, LBEASTAI)




