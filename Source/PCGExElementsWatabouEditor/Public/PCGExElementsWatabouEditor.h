// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Styling/SlateStyle.h"

class FPCGExWatabouDataActions;
class UPCGExWatabouDataReimportFactory;

class FPCGExElementsWatabouEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:
	TSharedPtr<FPCGExWatabouDataActions> DataActions;
	TSharedPtr<FSlateStyleSet> Style;
};
