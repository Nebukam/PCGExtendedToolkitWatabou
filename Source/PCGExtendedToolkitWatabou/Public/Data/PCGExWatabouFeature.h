// Copyright (c) Nebukam

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "PCGExWatabouFeature.generated.h"

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeature
{
	GENERATED_BODY()

public:
	FPCGExWatabouFeature() = default;

	UPROPERTY()
	FName FeatureId = NAME_None;
};
