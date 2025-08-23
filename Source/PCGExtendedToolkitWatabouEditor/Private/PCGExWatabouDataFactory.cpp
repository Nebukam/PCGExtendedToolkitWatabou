// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#include "PCGExWatabouDataFactory.h"

#include "Editor.h"
#include "PCGExtendedToolkitWatabou.h"
#include "Data/PCGExWatabouData.h"
#include "Data/PCGExWatabouFeature.h"
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
#define PCGEX_WATABOU_FACTORY_CANCEL \
	bOutOperationCanceled = true; \
	return nullptr;

	UE_LOG(LogPCGExWatabou, Log, TEXT("Importing %s"), *Filename)

	FString FileContents;
	if (!FFileHelper::LoadFileToString(FileContents, *Filename))
	{
		UE_LOG(LogPCGExWatabou, Error, TEXT("Could not load as string %s"), *Filename)
		PCGEX_WATABOU_FACTORY_CANCEL
	}

	// Parse JSON
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContents);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogPCGExWatabou, Error, TEXT("Could not deserialize %s"), *Filename)
		PCGEX_WATABOU_FACTORY_CANCEL
	}

	FName GenName = NAME_None;
	FPCGExWatabouVersion GenVersion = FPCGExWatabouVersion(0);

	if (JsonObject->HasField(TEXT("features")))
	{
		// If we have a feature array this narrows down the types of data to VG/MFCG/Hood
		const TArray<TSharedPtr<FJsonValue>>* FeaturesArray;
		if (JsonObject->TryGetArrayField(TEXT("features"), FeaturesArray))
		{
			// Find the feature object
			// It should always come first, but since it's an array and people might be cobling data together 
			for (const TSharedPtr<FJsonValue>& FeatureValue : *FeaturesArray)
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
					if (TypeId == TEXT("Feature") && IdId == TEXT("values"))
					{
						GenName = FName(GeneratorId);
						GenVersion = FPCGExWatabouVersion(GeneratorVersion);
						break;
					}
				}
			}
		}
	}
	else
	{
		// Possibly OPD ?
	}

	if (GenName.IsNone())
	{
		UE_LOG(LogPCGExWatabou, Error, TEXT("Could not find an importer for file : %s"), *Filename)
		PCGEX_WATABOU_FACTORY_CANCEL
	}

	FPCGExtendedToolkitWatabouModule* ModulePtr = FModuleManager::GetModulePtr<FPCGExtendedToolkitWatabouModule>("PCGExtendedToolkitWatabou");
	if (!ModulePtr)
	{
		UE_LOG(LogPCGExWatabou, Error, TEXT("Can't find Watabou Module : %s"), *Filename)
		PCGEX_WATABOU_FACTORY_CANCEL
	}

	TSharedPtr<PCGExWatabouImporter::IImporter> ImporterInstance = ModulePtr->CreateImporter(GenName, GenVersion);

	if (!ModulePtr)
	{
		UE_LOG(LogPCGExWatabou, Error, TEXT("Could not find valid importer for Generator '%s' (%d) (%s)"), *GenName.ToString(), static_cast<int32>(GenVersion), *Filename)
		PCGEX_WATABOU_FACTORY_CANCEL
	}

	// Create the asset
	UPCGExWatabouData* NewAsset = NewObject<UPCGExWatabouData>(InParent, InClass, InName, Flags);
	NewAsset->Features->Id = FName(Filename);

	// Assign AssetImportData for reimport
	if (!NewAsset->AssetImportData) { NewAsset->AssetImportData = NewObject<UAssetImportData>(NewAsset); }
	NewAsset->AssetImportData->Update(Filename);

	// Reset and build
	NewAsset->Reset();

	if (!ImporterInstance->PreBuild(JsonObject, NewAsset)) { PCGEX_WATABOU_FACTORY_CANCEL }

	ImporterInstance->Build(JsonObject, NewAsset->Features, NewAsset);

	if (!ImporterInstance->PostBuild(NewAsset)) { PCGEX_WATABOU_FACTORY_CANCEL }

	UE_LOG(LogPCGExWatabou, Log, TEXT("Successfully imported %s"), *Filename)
	return NewAsset;

#undef PCGEX_WATABOU_FACTORY_CANCEL
}

UPCGExWatabouDataReimportFactory::UPCGExWatabouDataReimportFactory()
{
	SupportedClass = UPCGExWatabouData::StaticClass();
	bCreateNew = true;
	ImportPriority = DefaultImportPriority - 1; // must be lower than main factory
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
	if (!Obj)
	{
		return EReimportResult::Failed;
	}

	UPCGExWatabouData* Asset = Cast<UPCGExWatabouData>(Obj);

	if (!Asset || !Asset->AssetImportData)
	{
		return EReimportResult::Failed;
	}

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

int32 UPCGExWatabouDataReimportFactory::GetPriority() const
{
	return ImportPriority;
}
