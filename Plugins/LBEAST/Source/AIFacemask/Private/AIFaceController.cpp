// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "AIFaceController.h"
#include "Components/SkeletalMeshComponent.h"

UAIFaceController::UAIFaceController()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIFaceController::BeginPlay()
{
	Super::BeginPlay();
	
	// Auto-initialize if config is set
	if (Config.TargetMesh)
	{
		InitializeAIFace(Config);
	}
}

void UAIFaceController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsInitialized || !Config.bUseAIGeneration)
	{
		return;
	}

	UpdateTimer += DeltaTime;
	
	// Update at configured rate
	if (UpdateTimer >= (1.0f / Config.UpdateRate))
	{
		UpdateTimer = 0.0f;
		
		// NOOP: TODO - Query AI service for new facial expression data
		// For now, this is a placeholder for future AI integration
	}
}

bool UAIFaceController::InitializeAIFace(const FAIFaceConfig& InConfig)
{
	Config = InConfig;

	if (!Config.TargetMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("AIFaceController: Cannot initialize - no target mesh specified"));
		return false;
	}

	bIsInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("AIFaceController: Initialized successfully"));
	return true;
}

void UAIFaceController::PlayFacialAnimation(const FFacialAnimationData& AnimationData)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFaceController: Cannot play animation - not initialized"));
		return;
	}

	CurrentAnimationData = AnimationData;

	// Apply blend shape weights to target mesh
	if (Config.TargetMesh)
	{
		for (const auto& BlendShape : AnimationData.BlendShapeWeights)
		{
			// NOOP: TODO - Apply morph target weights to skeletal mesh
			// This requires mapping blend shape names to morph target names
		}
	}
}

void UAIFaceController::SetBlendShapeWeight(FName BlendShapeName, float Weight)
{
	if (!bIsInitialized)
	{
		return;
	}

	CurrentAnimationData.BlendShapeWeights.Add(BlendShapeName, FMath::Clamp(Weight, 0.0f, 1.0f));
	
	// Apply to mesh immediately
	if (Config.TargetMesh)
	{
		// NOOP: TODO - Apply morph target weight
	}
}

void UAIFaceController::SetAIGenerationEnabled(bool bEnabled)
{
	Config.bUseAIGeneration = bEnabled;
}




