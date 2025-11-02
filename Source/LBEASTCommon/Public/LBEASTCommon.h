// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

// API macro for LBEASTCommon module
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG || UE_BUILD_TEST || UE_BUILD_SHIPPING
#define LBEASTCOMMON_API __declspec(dllexport)
#else
#define LBEASTCOMMON_API __declspec(dllimport)
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogLBEASTCommon, Log, All);

class FLBEASTCommonModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

