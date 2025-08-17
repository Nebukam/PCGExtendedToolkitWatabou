// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "CoreMinimal.h"



class UPCGExWatabouData;

namespace PCGExWatabouImporter
{
	class PCGEXTENDEDTOOLKITWATABOU_API IImporter : public TSharedFromThis<IImporter>
	{
	public:
		IImporter() = default;
		virtual ~IImporter() = default;

		virtual void Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouData* InOutData);
	};

	class PCGEXTENDEDTOOLKITWATABOU_API IGeometryImporter : public IImporter
	{
	public:
		IGeometryImporter() = default;

		virtual void Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouData* InOutData) override;
	};

}
