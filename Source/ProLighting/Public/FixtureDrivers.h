// Copyright (c) 2025 AJ Campbell. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "ProLighting.h"
#include "ProLightingTypes.h"
#include "UniverseBuffer.h"

class PROLIGHTING_API IFixtureDriver
{
public:
	virtual ~IFixtureDriver() = default;
	virtual void ApplyIntensity(const FLBEASTDMXFixture& Fixture, float Intensity, FUniverseBuffer& Buffer) = 0;
	virtual void ApplyColor(const FLBEASTDMXFixture& Fixture, float Red, float Green, float Blue, float White, FUniverseBuffer& Buffer) = 0;
};

class PROLIGHTING_API FFixtureDriverDimmable : public IFixtureDriver
{
public:
	virtual void ApplyIntensity(const FLBEASTDMXFixture& Fixture, float Intensity, FUniverseBuffer& Buffer) override
	{
		uint8 V = (uint8)(FMath::Clamp(Intensity, 0.0f, 1.0f) * 255.0f);
		Buffer.SetChannel(Fixture.Universe, Fixture.DMXChannel, V);
	}
	virtual void ApplyColor(const FLBEASTDMXFixture&, float, float, float, float, FUniverseBuffer&) override {}
};

class PROLIGHTING_API FFixtureDriverRGB : public IFixtureDriver
{
public:
	virtual void ApplyIntensity(const FLBEASTDMXFixture& Fixture, float Intensity, FUniverseBuffer& Buffer) override
	{
		uint8 V = (uint8)(FMath::Clamp(Intensity, 0.0f, 1.0f) * 255.0f);
		Buffer.SetChannel(Fixture.Universe, Fixture.DMXChannel, V);
	}
	virtual void ApplyColor(const FLBEASTDMXFixture& Fixture, float Red, float Green, float Blue, float, FUniverseBuffer& Buffer) override
	{
		int32 Base = Fixture.DMXChannel;
		Buffer.SetChannel(Fixture.Universe, Base + 0, (uint8)(FMath::Clamp(Red, 0.0f, 1.0f) * 255.0f));
		Buffer.SetChannel(Fixture.Universe, Base + 1, (uint8)(FMath::Clamp(Green, 0.0f, 1.0f) * 255.0f));
		Buffer.SetChannel(Fixture.Universe, Base + 2, (uint8)(FMath::Clamp(Blue, 0.0f, 1.0f) * 255.0f));
	}
};

class PROLIGHTING_API FFixtureDriverRGBW : public IFixtureDriver
{
public:
	virtual void ApplyIntensity(const FLBEASTDMXFixture& Fixture, float Intensity, FUniverseBuffer& Buffer) override
	{
		uint8 V = (uint8)(FMath::Clamp(Intensity, 0.0f, 1.0f) * 255.0f);
		Buffer.SetChannel(Fixture.Universe, Fixture.DMXChannel, V);
	}
	virtual void ApplyColor(const FLBEASTDMXFixture& Fixture, float Red, float Green, float Blue, float White, FUniverseBuffer& Buffer) override
	{
		int32 Base = Fixture.DMXChannel;
		Buffer.SetChannel(Fixture.Universe, Base + 0, (uint8)(FMath::Clamp(Red, 0.0f, 1.0f) * 255.0f));
		Buffer.SetChannel(Fixture.Universe, Base + 1, (uint8)(FMath::Clamp(Green, 0.0f, 1.0f) * 255.0f));
		Buffer.SetChannel(Fixture.Universe, Base + 2, (uint8)(FMath::Clamp(Blue, 0.0f, 1.0f) * 255.0f));
		Buffer.SetChannel(Fixture.Universe, Base + 3, (uint8)(FMath::Clamp(White, 0.0f, 1.0f) * 255.0f));
	}
};

class PROLIGHTING_API FFixtureDriverMovingHead : public IFixtureDriver
{
public:
	virtual void ApplyIntensity(const FLBEASTDMXFixture& Fixture, float Intensity, FUniverseBuffer& Buffer) override
	{
		uint8 V = (uint8)(FMath::Clamp(Intensity, 0.0f, 1.0f) * 255.0f);
		int32 Base = Fixture.DMXChannel;
		int32 Offset = (Fixture.ChannelCount >= 3) ? 2 : 0;
		Buffer.SetChannel(Fixture.Universe, Base + Offset, V);
	}
	virtual void ApplyColor(const FLBEASTDMXFixture& Fixture, float Red, float Green, float Blue, float, FUniverseBuffer& Buffer) override
	{
		int32 Base = Fixture.DMXChannel;
		// Assume RGB at offsets 3,4,5 (1-based -> 0-based math)
		Buffer.SetChannel(Fixture.Universe, Base + 3, (uint8)(FMath::Clamp(Red, 0.0f, 1.0f) * 255.0f));
		Buffer.SetChannel(Fixture.Universe, Base + 4, (uint8)(FMath::Clamp(Green, 0.0f, 1.0f) * 255.0f));
		Buffer.SetChannel(Fixture.Universe, Base + 5, (uint8)(FMath::Clamp(Blue, 0.0f, 1.0f) * 255.0f));
	}
};

class PROLIGHTING_API FFixtureDriverCustom : public IFixtureDriver
{
public:
	virtual void ApplyIntensity(const FLBEASTDMXFixture& Fixture, float Intensity, FUniverseBuffer& Buffer) override
	{
		uint8 V = (uint8)(FMath::Clamp(Intensity, 0.0f, 1.0f) * 255.0f);
		Buffer.SetChannel(Fixture.Universe, Fixture.DMXChannel, V);
	}
	virtual void ApplyColor(const FLBEASTDMXFixture& Fixture, float Red, float Green, float Blue, float, FUniverseBuffer& Buffer) override
	{
		if (Fixture.CustomChannelMapping.Num() >= 3)
		{
			int32 Base = Fixture.DMXChannel;
			int32 ro = Fixture.CustomChannelMapping[0] - 1;
			int32 go = Fixture.CustomChannelMapping[1] - 1;
			int32 bo = Fixture.CustomChannelMapping[2] - 1;
			if (ro >= 0) Buffer.SetChannel(Fixture.Universe, Base + ro, (uint8)(FMath::Clamp(Red, 0.0f, 1.0f) * 255.0f));
			if (go >= 0) Buffer.SetChannel(Fixture.Universe, Base + go, (uint8)(FMath::Clamp(Green, 0.0f, 1.0f) * 255.0f));
			if (bo >= 0) Buffer.SetChannel(Fixture.Universe, Base + bo, (uint8)(FMath::Clamp(Blue, 0.0f, 1.0f) * 255.0f));
		}
	}
};

class PROLIGHTING_API FFixtureDriverFactory
{
public:
	static TUniquePtr<IFixtureDriver> Create(ELBEASTDMXFixtureType Type)
	{
		switch (Type)
		{
		case ELBEASTDMXFixtureType::Dimmable: return MakeUnique<FFixtureDriverDimmable>();
		case ELBEASTDMXFixtureType::RGB:      return MakeUnique<FFixtureDriverRGB>();
		case ELBEASTDMXFixtureType::RGBW:     return MakeUnique<FFixtureDriverRGBW>();
		case ELBEASTDMXFixtureType::MovingHead:return MakeUnique<FFixtureDriverMovingHead>();
		case ELBEASTDMXFixtureType::Custom:   return MakeUnique<FFixtureDriverCustom>();
		default: return MakeUnique<FFixtureDriverDimmable>();
		}
	}
};




