// Copyright (c) Nebukam

#pragma once

#include "CoreMinimal.h"
#include "PCGExWatabou.h"
#include "Engine/DataAsset.h"

#if WITH_EDITOR
#include "EditorFramework/AssetImportData.h"
#endif

#include "PCGExWatabouData.generated.h"

class UPCGExWatabouFeaturesCollection;
/**
 * 
 */
UCLASS()
class PCGEXTENDEDTOOLKITWATABOU_API UPCGExWatabouData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPCGExWatabouData();

#if WITH_EDITORONLY_DATA
	// Standard property to hold the source file info
	UPROPERTY(VisibleAnywhere, Instanced, Category = "Import")
	UAssetImportData* AssetImportData;
#endif

	UPROPERTY(Instanced)
	TObjectPtr<UPCGExWatabouFeaturesCollection> Features;

	UPROPERTY(VisibleAnywhere)
	FBox Bounds = FBox(NoInit);

	void Reset();
};
