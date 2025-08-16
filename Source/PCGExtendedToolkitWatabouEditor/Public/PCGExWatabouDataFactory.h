// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/
#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "PCGExWatabouDataFactory.generated.h"


/**
 * 
 */
UCLASS()
class PCGEXTENDEDTOOLKITWATABOUEDITOR_API UPCGExWatabouDataFactory : public UFactory
{
	GENERATED_BODY()

public:
	UPCGExWatabouDataFactory();
protected:
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

	// Supported formats
	virtual bool FactoryCanImport(const FString& Filename) override;

};
