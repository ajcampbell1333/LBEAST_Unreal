// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"

#ifndef LBEASTAI_API
// API macro for LBEASTAI module
// Use Unreal's DLLEXPORT/DLLIMPORT macros (defined by build system)
#if defined(DLLEXPORT) && !defined(DLLIMPORT)
#define LBEASTAI_API DLLEXPORT
#elif defined(DLLIMPORT)
#define LBEASTAI_API DLLIMPORT
#else
// Fallback to __declspec if Unreal macros not available
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG || UE_BUILD_TEST || UE_BUILD_SHIPPING
#define LBEASTAI_API __declspec(dllexport)
#else
#define LBEASTAI_API __declspec(dllimport)
#endif
#endif
#endif // LBEASTAI_API

