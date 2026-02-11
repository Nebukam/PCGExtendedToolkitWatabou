// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include <functional>

#include "CoreMinimal.h"

class FJsonObject;
class FJsonValue;
struct FPCGExFeatureDetails;
struct FPCGExWatabouFeature;
class UPCGExWatabouFeaturesCollection;
class UPCGExWatabouData;

namespace PCGExWatabouImporter
{
	const TSet<FName> IgnoredDetails = {
		FName("id"),
		FName("type"),
		FName("coordinates"),
		FName("features"),
		FName("geometries")
	};

	using FLoopOverCoordinates = std::function<void(const FVector2D&, const int32)>;

	PCGEXELEMENTSWATABOU_API
	int32 LoopOverCoordinates(const TArray<TSharedPtr<FJsonValue>>& InCoordinates, FLoopOverCoordinates&& InFunc);

	PCGEXELEMENTSWATABOU_API
	bool GetVector2D(const TSharedPtr<FJsonValue>& InObject, FVector2D& OutValue);

	class PCGEXELEMENTSWATABOU_API IImporter : public TSharedFromThis<IImporter>
	{
	public:
		IImporter() = default;
		virtual ~IImporter() = default;

		virtual bool PreBuild(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouData* InData);
		
		virtual void Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouFeaturesCollection* InCollection, UPCGExWatabouData* InData = nullptr);
		virtual void BuildDetails(const TSharedPtr<FJsonObject>& InFeatureObj, UPCGExWatabouFeaturesCollection* InCollection, FPCGExFeatureDetails* InDetails = nullptr);
		
		virtual bool PostBuild(UPCGExWatabouData* InData);
	};

	class PCGEXELEMENTSWATABOU_API IGeometryImporter : public IImporter
	{
	public:
		IGeometryImporter() = default;

		virtual void Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouFeaturesCollection* InCollection, UPCGExWatabouData* InData) override;

	protected:
		virtual int32 Build(FPCGExWatabouFeature& InElement, const TArray<TSharedPtr<FJsonValue>>& InCoordinates);
	};
}
