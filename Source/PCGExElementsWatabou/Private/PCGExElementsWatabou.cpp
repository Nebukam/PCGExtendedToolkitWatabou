// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#include "PCGExElementsWatabou.h"
#if WITH_EDITOR
#include "ISettingsModule.h"
#endif

#include "PCGExGlobalSettings.h"
#include "PCGExWatabou.h"
#include "Importers/PCGExWatabouImporter_Hood.h"
#include "Importers/PCGExWatabouImporter_MFCG.h"
#include "Importers/PCGExWatabouImporter_OPD.h"
#include "Importers/PCGExWatabouImporter_VG.h"

#define LOCTEXT_NAMESPACE "FPCGExElementsWatabouModule"

void FPCGExElementsWatabouModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// Register importers
	
	OldBaseModules.Add(TEXT("PCGExElementsWatabou"));
	IPCGExModuleInterface::StartupModule();

#define PCGEX_REGISTER_IMPORTER(_ID, _VERSION, _CLASS) RegisterImporter(_ID, FPCGExWatabouVersion(TEXT(#_VERSION)), []() { return MakeShared<_CLASS>(); });

	// Village Generator
	PCGEX_REGISTER_IMPORTER(PCGExWatabou::GeneratorId_VG, 1.6.6, PCGExWatabouImporter::Importer_VG)

	// Medieval Fantasy City Generator
	PCGEX_REGISTER_IMPORTER(PCGExWatabou::GeneratorId_MFCG, 0.11.5, PCGExWatabouImporter::Importer_MFCG)

	// Hood Generator
	PCGEX_REGISTER_IMPORTER(PCGExWatabou::GeneratorId_Hood, 1.2.2, PCGExWatabouImporter::Importer_Hood)

	// One-page-dungeon Generator
	PCGEX_REGISTER_IMPORTER(PCGExWatabou::GeneratorId_OPD, 1.2.5, PCGExWatabouImporter::Importer_OPD)

#undef PCGEX_REGISTER_IMPORTER
}

void FPCGExElementsWatabouModule::ShutdownModule()
{
	IPCGExModuleInterface::ShutdownModule();
}

void FPCGExElementsWatabouModule::RegisterImporter(FName Name, int32 Version, FCreateFunc Func)
{
	TSharedPtr<TMap<int32, FCreateFunc>>* GeneratorsPtr = GeneratorRegistry.Find(Name);

	if (GeneratorsPtr)
	{
		(*GeneratorsPtr)->Add(Version, MoveTemp(Func));
		return;
	}

	const TSharedPtr<TMap<int32, FCreateFunc>> NewMap = MakeShared<TMap<int32, FCreateFunc>>();
	NewMap->Add(Version, MoveTemp(Func));

	GeneratorRegistry.Add(Name, NewMap);
}

TSharedPtr<PCGExWatabouImporter::IImporter> FPCGExElementsWatabouModule::CreateImporter(const FName Name, const int32 Version)
{
	TSharedPtr<TMap<int32, FCreateFunc>>* GeneratorsPtr = GeneratorRegistry.Find(Name);

	if (!GeneratorsPtr) { return nullptr; }

	const TMap<int32, FCreateFunc>& Generators = *GeneratorsPtr->Get();

	int32 BestMatch = -1;
	for (const TPair<int32, FCreateFunc>& Pair : Generators)
	{
		if (Pair.Key == Version) { return Pair.Value(); }
		if (Pair.Key < Version && Pair.Key > BestMatch) { BestMatch = Pair.Key; }
	}

	if (BestMatch == -1) { return nullptr; }

	return (*Generators.Find(BestMatch))();
}

#undef LOCTEXT_NAMESPACE

PCGEX_IMPLEMENT_MODULE(FPCGExElementsWatabouModule, PCGExElementsWatabou)
