// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/


#include "Importers/PCGExWatabouImporter.h"

#include "Data/PCGExWatabouData.h"





namespace PCGExWatabouImporter
{
	void IImporter::Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouData* InOutData)
	{
		InOutData->Reset();
	}

	void IGeometryImporter::Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouData* InOutData)
	{
		IImporter::Build(InJson, InOutData);
	}
}
