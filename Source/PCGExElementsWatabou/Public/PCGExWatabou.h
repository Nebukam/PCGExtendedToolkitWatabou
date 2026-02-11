// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "PCGExWatabou.generated.h"

USTRUCT()
struct PCGEXELEMENTSWATABOU_API FPCGExWatabouVersion
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Version = 0;
	
	FPCGExWatabouVersion() = default;
	explicit FPCGExWatabouVersion(const FString& InSemVer);
	explicit FPCGExWatabouVersion(const int32& InVersion);

	bool operator<(const FPCGExWatabouVersion& Other) const { return Version < Other.Version; }
	bool operator>(const FPCGExWatabouVersion& Other) const { return Version > Other.Version; }
	bool operator==(const FPCGExWatabouVersion& Other) const { return Version == Other.Version; }

	operator int32() const { return Version; }
};

namespace PCGExWatabou
{
	const FName GeneratorId_VG = FName("vg");
	const FName GeneratorId_MFCG = FName("mfcg");
	const FName GeneratorId_Hood = FName("Hood");
	const FName GeneratorId_OPD = FName("OPD");

	const FString FeatureTypeFeature = TEXT("Feature");
	const FString FeatureTypePolygon = TEXT("Polygon");
	const FString FeatureTypePoint = TEXT("Point");
	const FString FeatureTypeLineString = TEXT("LineString");
	const FString FeatureTypeMultiPolygon = TEXT("MultiPolygon");
	const FString FeatureTypeMultiPoint = TEXT("MultiPoint");
	const FString FeatureTypeMultiLineString = TEXT("MultiLineString");
	const FString FeatureTypeGeometryCollection = TEXT("GeometryCollection");
	
}
