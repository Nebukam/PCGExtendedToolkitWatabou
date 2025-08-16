// Copyright (c) Nebukam


#include "Data/PCGExWatabouData.h"

UPCGExWatabouData::UPCGExWatabouData()
{
#if WITH_EDITOR
		// Instantiate AssetImportData to enable persistent reimport
		AssetImportData = CreateDefaultSubobject<UAssetImportData>(TEXT("AssetImportData"));
#endif		
}

void UPCGExWatabouData::AddFeature(const FString& FeatureId)
{
	FeatureIds.Add(FeatureId);
}
