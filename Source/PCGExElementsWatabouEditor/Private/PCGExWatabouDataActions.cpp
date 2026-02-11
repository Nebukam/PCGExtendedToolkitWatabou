// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#include "PCGExWatabouDataActions.h"

#include "ToolMenuSection.h"
#include "Data/PCGExWatabouData.h"
#include "EditorFramework/AssetImportData.h"
#include "Misc/MessageDialog.h"
#include "Widgets/Views/SListView.h"

FText FPCGExWatabouDataActions::GetName() const
{
	return INVTEXT("Watabou Arcana");
}

FString FPCGExWatabouDataActions::GetObjectDisplayName(UObject* Object) const
{
	return Object->GetName();
}

UClass* FPCGExWatabouDataActions::GetSupportedClass() const
{
	return UPCGExWatabouData::StaticClass();
}

FColor FPCGExWatabouDataActions::GetTypeColor() const
{
	return FColor::Purple;
}

uint32 FPCGExWatabouDataActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

void FPCGExWatabouDataActions::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets,
	TArray<FString>& OutSourceFilePaths) const
{
	for (auto& Asset : TypeAssets)
	{
		const auto DataAsset = CastChecked<UPCGExWatabouData>(Asset);
		if (DataAsset->AssetImportData)
		{
			DataAsset->AssetImportData->ExtractFilenames(OutSourceFilePaths);
		}
	}
}

bool FPCGExWatabouDataActions::HasActions(const TArray<UObject*>& InObjects) const
{
	return false;
}

