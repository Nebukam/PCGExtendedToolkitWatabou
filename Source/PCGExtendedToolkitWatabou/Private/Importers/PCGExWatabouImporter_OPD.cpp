// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/


#include "Importers/PCGExWatabouImporter_OPD.h"




namespace PCGExWatabouImporter
{
	void Importer_OPD::Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouData* InOutData)
	{
		IImporter::Build(InJson, InOutData);
	}
}

