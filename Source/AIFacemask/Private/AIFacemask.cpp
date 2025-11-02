// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "AIFacemask.h"

#define LOCTEXT_NAMESPACE "FAIFacemaskModule"

void FAIFacemaskModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("LBEAST AIFacemask Module: Startup"));
}

void FAIFacemaskModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("LBEAST AIFacemask Module: Shutdown"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAIFacemaskModule, AIFacemask)




