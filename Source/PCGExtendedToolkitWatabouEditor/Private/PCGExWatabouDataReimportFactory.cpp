// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/
#include "PCGExWatabouDataFactory.h"

#include "Editor.h"
#include "PCGExWatabouData.h"


UPCGExWatabouDataFactory::UPCGExWatabouDataFactory()
{
	SupportedClass = UPCGExWatabouData::StaticClass();
	bCreateNew = false;
	bEditAfterNew = true;
	SupportedClass = UDataAsset::StaticClass(); // Replace with your custom asset class
	Formats.Add(TEXT("wataboujson;Watabou JSON Export"));
}

UObject* UPCGExWatabouDataFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FString FileContent;
	if (!FFileHelper::LoadFileToString(FileContent, *Filename))
	{
		return nullptr;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return nullptr;
	}

	// Create your custom asset
	UDataAsset* NewAsset = NewObject<UDataAsset>(InParent, InClass, InName, Flags);

	// Parse JSON and fill asset data here
	// Example: NewAsset->RoadWidth = JsonObject->GetIntegerField("roadWidth");

	return NewAsset;
}

bool UPCGExWatabouDataFactory::FactoryCanImport(const FString& Filename)
{
	return Super::FactoryCanImport(Filename);
}
