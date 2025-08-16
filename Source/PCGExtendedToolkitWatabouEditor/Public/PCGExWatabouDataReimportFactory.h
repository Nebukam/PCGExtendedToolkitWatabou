// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/
#pragma once

#include "CoreMinimal.h"
#include "EditorReimportHandler.h"
#include "PCGExWatabouDataFactory.h"
#include "Factories/Factory.h"

#include "PCGExWatabouDataReimportFactory.generated.h"


/**
 * 
 */
UCLASS()
class UPCGExWatabouDataReimportFactory : public UPCGExWatabouDataFactory, public FReimportHandler
{
	GENERATED_BODY()

public:
	UPCGExWatabouDataReimportFactory();

	//~ Begin FReimportHandler Interface
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	virtual int32 GetPriority() const override;
	//~ End FReimportHandler Interface
};