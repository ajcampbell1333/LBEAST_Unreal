// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#include "ExperienceTemplates/AIFacemaskExperience.h"
#include "AIFaceController.h"
#include "EmbeddedDeviceController.h"

AAIFacemaskExperience::AAIFacemaskExperience()
{
	// Create components
	FaceController = CreateDefaultSubobject<UAIFaceController>(TEXT("FaceController"));
	CostumeController = CreateDefaultSubobject<UEmbeddedDeviceController>(TEXT("CostumeController"));

	// Configure for multiplayer by default
	bMultiplayerEnabled = true;

	// Enable passthrough for live actors
	HMDConfig.bEnablePassthrough = true;
}

bool AAIFacemaskExperience::InitializeExperienceImpl()
{
	if (!Super::InitializeExperienceImpl())
	{
		return false;
	}

	// Initialize AI Face Controller
	if (FaceController && LiveActorMesh)
	{
		FAIFaceConfig FaceConfig;
		FaceConfig.TargetMesh = LiveActorMesh;
		FaceConfig.bUseAIGeneration = true;
		FaceConfig.UpdateRate = 30.0f;

		if (!FaceController->InitializeAIFace(FaceConfig))
		{
			UE_LOG(LogTemp, Error, TEXT("AIFacemaskExperience: Failed to initialize face controller"));
			return false;
		}
	}

	// Initialize Costume Controller (embedded systems)
	if (CostumeController)
	{
		FEmbeddedDeviceConfig DeviceConfig;
		DeviceConfig.DeviceType = ELBEASTMicrocontrollerType::ESP32;
		DeviceConfig.Protocol = ELBEASTCommProtocol::WiFi;
		DeviceConfig.DeviceAddress = TEXT("192.168.1.50");
		DeviceConfig.Port = 8888;
		DeviceConfig.InputChannelCount = 8;  // Buttons on costume
		DeviceConfig.OutputChannelCount = 8; // Haptic vibrators

		if (!CostumeController->InitializeDevice(DeviceConfig))
		{
			UE_LOG(LogTemp, Warning, TEXT("AIFacemaskExperience: Costume controller initialization failed, continuing without embedded systems"));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("AIFacemaskExperience: Initialized with %d live actors and %d players"), NumberOfLiveActors, NumberOfPlayers);
	return true;
}

void AAIFacemaskExperience::ShutdownExperienceImpl()
{
	if (CostumeController && CostumeController->IsDeviceConnected())
	{
		CostumeController->DisconnectDevice();
	}

	Super::ShutdownExperienceImpl();
}

void AAIFacemaskExperience::SetFacialExpression(const TMap<FName, float>& BlendShapes)
{
	if (!FaceController)
	{
		return;
	}

	for (const auto& Shape : BlendShapes)
	{
		FaceController->SetBlendShapeWeight(Shape.Key, Shape.Value);
	}
}

void AAIFacemaskExperience::TriggerCostumeHaptic(int32 Channel, float Intensity, float Duration)
{
	if (CostumeController && CostumeController->IsDeviceConnected())
	{
		CostumeController->TriggerHapticPulse(Channel, Intensity, Duration);
	}
}


