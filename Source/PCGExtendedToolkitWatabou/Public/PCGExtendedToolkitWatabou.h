// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

namespace PCGExWatabouImporter
{
	class IImporter;
}

class PCGEXTENDEDTOOLKITWATABOU_API FPCGExtendedToolkitWatabouModule final : public IModuleInterface
{
	using FCreateFunc = TFunction<TSharedPtr<PCGExWatabouImporter::IImporter>()>;

public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RegisterImporter(FName Name, int32 Version, FCreateFunc Func);
	TSharedPtr<PCGExWatabouImporter::IImporter> CreateImporter(const FName Name, const int32 Version);

private:
	TMap<FName, TSharedPtr<TMap<int32, FCreateFunc>>> GeneratorRegistry;
};
