// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "LBEASTExperiences.h"

#define LOCTEXT_NAMESPACE "FLBEASTExperiencesModule"

void FLBEASTExperiencesModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("LBEAST Experiences Module: Startup"));
}

void FLBEASTExperiencesModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("LBEAST Experiences Module: Shutdown"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLBEASTExperiencesModule, LBEASTExperiences)


