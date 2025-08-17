// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/


#include "Importers/PCGExWatabouImporter_Hood.h"




namespace PCGExWatabouImporter
{
	void Importer_Hood::Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouData* InOutData)
	{
		IGeometryImporter::Build(InJson, InOutData);
	}
}

