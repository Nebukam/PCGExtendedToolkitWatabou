// Copyright (c) Nebukam

#pragma once

#include "CoreMinimal.h"
#include "PCGExWatabou.h"
#include "PCGExWatabouFeature.h"
#include "Engine/DataAsset.h"

#if WITH_EDITOR
#include "EditorFramework/AssetImportData.h"
#endif

#include "PCGExWatabouData.generated.h"

/**
 * 
 */
UCLASS()
class PCGEXTENDEDTOOLKITWATABOU_API UPCGExWatabouData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPCGExWatabouData();

	UPROPERTY(VisibleAnywhere, Category = Data)
	TMap<FName, double> Values;
	
	UPROPERTY(VisibleAnywhere, Category=Data, meta=(TitleProperty="{Id} | {Type}"))
	TMap<FPCGExFeatureIdentifier, int32> Identifiers;
	
	UPROPERTY(VisibleAnywhere, Instanced, Category = Data)
	TObjectPtr<UPCGExWatabouFeaturesCollection> Features;

	UPROPERTY(VisibleAnywhere, Category = "Data|Details")
	FBox Bounds = FBox(NoInit);
		
	void Reset();

#if WITH_EDITORONLY_DATA
	// Import data for this 
	UPROPERTY(VisibleAnywhere, Instanced, Category=ImportSettings)
	TObjectPtr<class UAssetImportData> AssetImportData;
	
	// UObject interface
	virtual void PostInitProperties() override;
	virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;
	// End of UObject interface
#endif
};
