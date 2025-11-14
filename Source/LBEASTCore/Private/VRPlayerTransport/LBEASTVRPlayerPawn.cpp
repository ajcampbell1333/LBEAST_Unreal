// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "VRPlayerTransport/LBEASTVRPlayerPawn.h"
#include "VRPlayerTransport/VRPlayerReplicationComponent.h"
#include "LBEASTHandGestureRecognizer.h"

ALBEASTVRPlayerPawn::ALBEASTVRPlayerPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create VR replication component
	VRReplicationComponent = CreateDefaultSubobject<ULBEASTVRPlayerReplicationComponent>(TEXT("VRReplicationComponent"));
	
	bAutoCreateHandGestureRecognizer = false;
}

void ALBEASTVRPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	// Optionally create hand gesture recognizer if requested
	if (bAutoCreateHandGestureRecognizer)
	{
		if (!GetHandGestureRecognizer())
		{
			ULBEASTHandGestureRecognizer* GestureRecognizer = NewObject<ULBEASTHandGestureRecognizer>(this, TEXT("HandGestureRecognizer"));
			if (GestureRecognizer)
			{
				GestureRecognizer->RegisterComponent();
				
				// Auto-initialize if we have a player controller
				if (APlayerController* PC = GetController<APlayerController>())
				{
					GestureRecognizer->InitializeRecognizer(PC);
				}
			}
		}
	}
}

ULBEASTHandGestureRecognizer* ALBEASTVRPlayerPawn::GetHandGestureRecognizer() const
{
	return FindComponentByClass<ULBEASTHandGestureRecognizer>();
}

