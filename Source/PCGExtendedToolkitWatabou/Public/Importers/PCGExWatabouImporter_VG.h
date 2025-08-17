﻿// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "CoreMinimal.h"
#include "PCGExWatabouImporter.h"



namespace PCGExWatabouImporter
{
	class PCGEXTENDEDTOOLKITWATABOU_API Importer_VG : public IGeometryImporter
	{
	public:
		Importer_VG() = default;

		virtual void Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouData* InOutData) override;
	};
}
