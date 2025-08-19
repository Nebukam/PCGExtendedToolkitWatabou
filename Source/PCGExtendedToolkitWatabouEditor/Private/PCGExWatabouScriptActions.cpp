// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#include "PCGExWatabouScriptActions.h"

#include "ToolMenuSection.h"
#include "Data/PCGExWatabouData.h"
#include "EditorFramework/AssetImportData.h"
#include "Misc/MessageDialog.h"
#include "Widgets/Views/SListView.h"

FText FPCGExWatabouScriptActions::GetName() const
{
	return INVTEXT("Watabou Arcana");
}

FString FPCGExWatabouScriptActions::GetObjectDisplayName(UObject* Object) const
{
	return Object->GetName();
}

UClass* FPCGExWatabouScriptActions::GetSupportedClass() const
{
	return UPCGExWatabouData::StaticClass();
}

FColor FPCGExWatabouScriptActions::GetTypeColor() const
{
	return FColor::Purple;
}

uint32 FPCGExWatabouScriptActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

void FPCGExWatabouScriptActions::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets,
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

bool FPCGExWatabouScriptActions::HasActions(const TArray<UObject*>& InObjects) const
{
	return false;
}

