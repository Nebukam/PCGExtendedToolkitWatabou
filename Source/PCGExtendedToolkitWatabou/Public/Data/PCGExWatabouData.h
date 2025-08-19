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

	UPROPERTY(EditAnywhere, Instanced, Category = Data)
	TObjectPtr<UPCGExWatabouFeaturesCollection> Features;

	UPROPERTY(EditAnywhere, Category = "Data|Details")
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
