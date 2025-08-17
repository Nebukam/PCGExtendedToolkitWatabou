// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/
#include "PCGExWatabouDataFactory.h"

#include "Editor.h"
#include "PCGExtendedToolkitWatabou.h"
#include "Data/PCGExWatabouData.h"
#include "Importers/PCGExWatabouImporter.h"


UPCGExWatabouDataFactory::UPCGExWatabouDataFactory()
{
	bEditorImport = true;
	bText = true;
	Formats.Add(TEXT("wataboujson;Watabou Data JSON"));
	SupportedClass = UPCGExWatabouData::StaticClass();
}

UObject* UPCGExWatabouDataFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FString FileContents;
	if (!FFileHelper::LoadFileToString(FileContents, *Filename))
	{
		bOutOperationCanceled = true;
		return nullptr;
	}

	// Parse JSON
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContents);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		bOutOperationCanceled = true;
		return nullptr;
	}

	FName GenName = NAME_None;
	FPCGExWatabouVersion GenVersion = FPCGExWatabouVersion(0);

	const TArray<TSharedPtr<FJsonValue>>* FeaturesArray;
	if (JsonObject->TryGetArrayField(TEXT("features"), FeaturesArray))
	{
		// If we have a feature array this narrows down the types of data to VG/MFCG/Hood
		// Find the feature object
		for (auto& FeatureValue : *FeaturesArray)
		{
			TSharedPtr<FJsonObject> FeatureObj = FeatureValue->AsObject();
			if (!FeatureObj) continue;

			FString TypeId;
			FString IdId;
			FString GeneratorId;
			FString GeneratorVersion;

			if (FeatureObj->TryGetStringField(TEXT("type"), TypeId) &&
				FeatureObj->TryGetStringField(TEXT("id"), IdId) &&
				FeatureObj->TryGetStringField(TEXT("generator"), GeneratorId) &&
				FeatureObj->TryGetStringField(TEXT("version"), GeneratorVersion))
			{
				if (TypeId == TEXT("Features") && IdId == TEXT("values"))
				{
					GenName = FName(GeneratorId);
					GenVersion = FPCGExWatabouVersion(GeneratorVersion);
					break;
				}
			}
		}
	}
	else
	{
		// Possibly OPD ?
	}

	FPCGExtendedToolkitWatabouModule* ModulePtr = FModuleManager::GetModulePtr<FPCGExtendedToolkitWatabouModule>("PCGExtendedToolkitWatabou");
	if (!ModulePtr) { return nullptr; }

	TSharedPtr<PCGExWatabouImporter::IImporter> ImporterInstance = ModulePtr->CreateImporter(GenName, GenVersion);

	if (!ModulePtr)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find valid importer for Generator '%s', version '%d'"), *GenName.ToString(), static_cast<int32>(GenVersion))
		return nullptr;
	}

	// Create the asset
	UPCGExWatabouData* NewAsset = NewObject<UPCGExWatabouData>(InParent, InClass, InName, Flags);

	// Assign AssetImportData for reimport
	if (!NewAsset->AssetImportData) { NewAsset->AssetImportData = NewObject<UAssetImportData>(NewAsset); }
	NewAsset->AssetImportData->Update(Filename);

	ImporterInstance->Build(JsonObject, NewAsset);

	return NewAsset;
}

UPCGExWatabouDataReimportFactory::UPCGExWatabouDataReimportFactory()
{
	SupportedClass = UPCGExWatabouData::StaticClass();
}

bool UPCGExWatabouDataReimportFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	if (UPCGExWatabouData* Asset = Cast<UPCGExWatabouData>(Obj))
	{
		if (Asset->AssetImportData)
		{
			OutFilenames.Add(Asset->AssetImportData->GetFirstFilename());
			return true;
		}
	}
	return false;
}

void UPCGExWatabouDataReimportFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	if (UPCGExWatabouData* Asset = Cast<UPCGExWatabouData>(Obj))
	{
		if (Asset->AssetImportData)
		{
			Asset->AssetImportData->Update(NewReimportPaths[0]);
		}
	}
}

EReimportResult::Type UPCGExWatabouDataReimportFactory::Reimport(UObject* Obj)
{
	if (!Obj) return EReimportResult::Failed;
	UPCGExWatabouData* Asset = Cast<UPCGExWatabouData>(Obj);
	if (!Asset || !Asset->AssetImportData) return EReimportResult::Failed;

	FString Filename = Asset->AssetImportData->GetFirstFilename();
	bool bCanceled = false;
	UObject* Reimported = FactoryCreateFile(
			Asset->GetClass(),
			Asset->GetOuter(),
			Asset->GetFName(),
			RF_Standalone | RF_Public,
			Filename,
			nullptr,
			GWarn,
			bCanceled
		);

	return Reimported ? EReimportResult::Succeeded : EReimportResult::Failed;
}
