// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/


#include "Importers/PCGExWatabouImporter_VG.h"





namespace PCGExWatabouImporter
{
	void Importer_VG::Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouFeaturesCollection* InCollection, UPCGExWatabouData* InData)
	{
		IGeometryImporter::Build(InJson, InCollection, InData);
	}
}

