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

	if (!bIsInitialized)
	{
		return;
	}

	UpdateTimer += DeltaTime;
	
	// Update at configured rate (receiving from NVIDIA ACE)
	if (UpdateTimer >= (1.0f / Config.UpdateRate))
	{
		UpdateTimer = 0.0f;
		
		// NOOP: TODO - Receive facial animation data from NVIDIA ACE endpoint
		// NVIDIA ACE will stream facial textures and blend shapes based on:
		// - Audio track (speech recognition)
		// - NLU (natural language understanding)
		// - Emotion detection
		// - State machine context
		// This component receives and applies the output - no manual control needed
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

void UAIFaceController::ReceiveFacialAnimationData(const FFacialAnimationData& AnimationData)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIFaceController: Cannot receive animation data - not initialized"));
		return;
	}

	CurrentAnimationData = AnimationData;

	// Apply blend shapes from NVIDIA ACE to target mesh
	if (Config.TargetMesh && AnimationData.BlendShapeWeights.Num() > 0)
	{
		ApplyBlendShapesToMesh(AnimationData.BlendShapeWeights);
	}

	// Apply facial texture from NVIDIA ACE to target mesh
	if (Config.TargetMesh && AnimationData.FacialTexture)
	{
		ApplyFacialTextureToMesh(AnimationData.FacialTexture);
	}
}

void UAIFaceController::ApplyBlendShapesToMesh(const TMap<FName, float>& BlendShapeWeights)
{
	if (!Config.TargetMesh)
	{
		return;
	}

	// NOOP: TODO - Apply blend shape weights to skeletal mesh morph targets
	// Map NVIDIA ACE blend shape names to Unreal morph target names
	// Apply weights using USkeletalMeshComponent::SetMorphTarget()
	// Example:
	// for (const auto& BlendShape : BlendShapeWeights)
	// {
	//     Config.TargetMesh->SetMorphTarget(BlendShape.Key, BlendShape.Value);
	// }
}

void UAIFaceController::ApplyFacialTextureToMesh(UTexture2D* FacialTexture)
{
	if (!Config.TargetMesh || !FacialTexture)
	{
		return;
	}

	// NOOP: TODO - Apply facial texture to mesh material
	// Update material parameter for facial texture
	// Example:
	// if (UMaterialInstanceDynamic* Material = Config.TargetMesh->CreateDynamicMaterialInstance(0))
	// {
	//     Material->SetTextureParameterValue(FName("FacialTexture"), FacialTexture);
	// }
}




