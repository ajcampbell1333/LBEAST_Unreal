// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIFaceController.generated.h"

/**
 * Facial animation data structure
 */
USTRUCT(BlueprintType)
struct FFacialAnimationData
{
	GENERATED_BODY()

	/** Blend shape weights for facial animation (normalized 0-1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Face")
	TMap<FName, float> BlendShapeWeights;

	/** Timestamp of this animation frame */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Face")
	float Timestamp = 0.0f;
};

/**
 * Configuration for AI Face system
 */
USTRUCT(BlueprintType)
struct FAIFaceConfig
{
	GENERATED_BODY()

	/** Target skeletal mesh for facial animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Face")
	TObjectPtr<USkeletalMeshComponent> TargetMesh = nullptr;

	/** Whether to use AI-generated expressions (vs. manual control) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Face")
	bool bUseAIGeneration = true;

	/** AI model endpoint URL (for future AI integration) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Face")
	FString AIEndpointURL;

	/** Update rate for facial animation (Hz) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Face")
	float UpdateRate = 30.0f;
};

/**
 * AI Face Controller Component
 * 
 * Manages AI-driven facial animation for immersive theater actors.
 * Attach this component to an actor to enable AI facial expression control.
 */
UCLASS(ClassGroup=(LBEAST), meta=(BlueprintSpawnableComponent))
class AIFACEMASK_API UAIFaceController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAIFaceController();

	/** Configuration for this AI face controller */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBEAST|AI Face")
	FAIFaceConfig Config;

	/**
	 * Initialize the AI face system with given configuration
	 * @param InConfig - Configuration settings
	 * @return true if initialization was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|AI Face")
	bool InitializeAIFace(const FAIFaceConfig& InConfig);

	/**
	 * Play facial animation data
	 * @param AnimationData - The facial animation data to play
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|AI Face")
	void PlayFacialAnimation(const FFacialAnimationData& AnimationData);

	/**
	 * Set a specific blend shape weight
	 * @param BlendShapeName - Name of the blend shape
	 * @param Weight - Weight value (0-1)
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|AI Face")
	void SetBlendShapeWeight(FName BlendShapeName, float Weight);

	/**
	 * Enable or disable AI generation
	 */
	UFUNCTION(BlueprintCallable, Category = "LBEAST|AI Face")
	void SetAIGenerationEnabled(bool bEnabled);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Current facial animation state */
	FFacialAnimationData CurrentAnimationData;

	/** Whether the system is initialized */
	bool bIsInitialized = false;

	/** Timer for AI update tick */
	float UpdateTimer = 0.0f;
};



