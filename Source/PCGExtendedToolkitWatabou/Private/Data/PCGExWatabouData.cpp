// Copyright (c) Nebukam


#include "Data/PCGExWatabouData.h"

#include "Data/PCGExWatabouFeature.h"

UPCGExWatabouData::UPCGExWatabouData()
{
#if WITH_EDITOR
	// Instantiate AssetImportData to enable persistent reimport
	AssetImportData = CreateDefaultSubobject<UAssetImportData>(TEXT("AssetImportData"));
#endif

	Features = CreateDefaultSubobject<UPCGExWatabouFeatureGeometryCollection>(TEXT("Features"));
}

void UPCGExWatabouData::Reset()
{
	if (Features) { Features->Reset(); }
}
