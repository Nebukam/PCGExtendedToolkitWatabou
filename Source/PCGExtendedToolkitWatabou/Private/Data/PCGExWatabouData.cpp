// Copyright (c) Nebukam


#include "Data/PCGExWatabouData.h"

#include "Data/PCGExWatabouFeature.h"

UPCGExWatabouData::UPCGExWatabouData()
{
#if WITH_EDITOR
	// Instantiate AssetImportData to enable persistent reimport
	AssetImportData = CreateDefaultSubobject<UAssetImportData>(TEXT("AssetImportData"));
#endif

	Features = CreateDefaultSubobject<UPCGExWatabouFeaturesCollection>(TEXT("Features"));
}

void UPCGExWatabouData::Reset()
{
	if (Features) { Features->Reset(); }
}


#if WITH_EDITORONLY_DATA

void UPCGExWatabouData::PostInitProperties()
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	}
	Super::PostInitProperties();
}

void UPCGExWatabouData::GetAssetRegistryTags(FAssetRegistryTagsContext Context) const
{
	if (AssetImportData)
	{
		Context.AddTag( FAssetRegistryTag(SourceFileTagName(), AssetImportData->GetSourceData().ToJson(), FAssetRegistryTag::TT_Hidden) );
	}
	
	Super::GetAssetRegistryTags(Context);
}

#endif
