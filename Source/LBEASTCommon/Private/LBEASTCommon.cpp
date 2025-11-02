// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "LBEASTCommon.h"

DEFINE_LOG_CATEGORY(LogLBEASTCommon);

#define LOCTEXT_NAMESPACE "FLBEASTCommonModule"

void FLBEASTCommonModule::StartupModule()
{
	UE_LOG(LogLBEASTCommon, Log, TEXT("LBEAST Common Module: Startup"));
}

void FLBEASTCommonModule::ShutdownModule()
{
	UE_LOG(LogLBEASTCommon, Log, TEXT("LBEAST Common Module: Shutdown"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLBEASTCommonModule, LBEASTCommon)

