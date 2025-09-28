// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "CoreMinimal.h"

#include "PCGExWatabouGlobalSettings.generated.h"

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="PCGEx + Watabou", Description="Configure PCGEx + Watabou settings"))
class PCGEXTENDEDTOOLKITWATABOU_API UPCGExWatabouGlobalSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/**  */
	//UPROPERTY(EditAnywhere, config, Category = "Performance|Defaults")
	//bool bDefaultCacheNodeOutput = false;
};
