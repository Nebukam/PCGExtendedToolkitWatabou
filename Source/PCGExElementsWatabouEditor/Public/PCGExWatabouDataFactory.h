// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/
#pragma once

#include "CoreMinimal.h"
#include "EditorReimportHandler.h"
#include "Factories/Factory.h"
#include "PCGExWatabouDataFactory.generated.h"


/**
 * 
 */
UCLASS()
class PCGEXELEMENTSWATABOUEDITOR_API UPCGExWatabouDataFactory : public UFactory
{
	GENERATED_BODY()

public:
	UPCGExWatabouDataFactory();

	virtual UObject* FactoryCreateFile(
		UClass* InClass,
		UObject* InParent,
		FName InName,
		EObjectFlags Flags,
		const FString& Filename,
		const TCHAR* Parms,
		FFeedbackContext* Warn,
		bool& bOutOperationCanceled
		) override;
};

/**
 * 
 */
UCLASS()
class UPCGExWatabouDataReimportFactory : public UPCGExWatabouDataFactory, public FReimportHandler
{
	GENERATED_BODY()

public:
	UPCGExWatabouDataReimportFactory();

	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	virtual int32 GetPriority() const override;
};
