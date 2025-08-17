// Copyright (c) Nebukam

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "PCGExWatabouFeature.generated.h"

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeature() = default;

	UPROPERTY(EditAnywhere, Category=Settings)
	FName Id = NAME_None;
};

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeaturePoint : public FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeaturePoint() = default;

	UPROPERTY(EditAnywhere, Category=Settings)
	FVector Location = FVector::ZeroVector;
};

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeatureMultiPoint : public FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeatureMultiPoint() = default;

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FPCGExWatabouFeaturePoint> Points;
};

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeaturePolygon : public FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeaturePolygon() = default;

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FVector> Polygon;
};

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeatureMultiPolygon : public FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeatureMultiPolygon() = default;

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FPCGExWatabouFeaturePolygon> Polygons;
};

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeatureLineString : public FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeatureLineString() = default;

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FVector> Positions;
};

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeatureMultiLineString : public FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeatureMultiLineString() = default;

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FPCGExWatabouFeatureLineString> Lines;
};

/**
 * 
 */
UCLASS()
class PCGEXTENDEDTOOLKITWATABOU_API UPCGExWatabouFeatureGeometryCollection : public UObject
{
	GENERATED_BODY()

public:
	UPCGExWatabouFeatureGeometryCollection() = default;

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FPCGExWatabouFeatureLineString> LineStrings;

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FPCGExWatabouFeatureMultiLineString> MultiLineStrings;

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FPCGExWatabouFeaturePolygon> Polygons;

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FPCGExWatabouFeatureMultiPolygon> MultiPolygons;

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FPCGExWatabouFeaturePoint> Points;

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FPCGExWatabouFeatureMultiPoint> MultiPoints;

	UPROPERTY(EditAnywhere, Instanced, Category=Settings)
	TArray<TObjectPtr<UPCGExWatabouFeatureGeometryCollection>> Collections;

	void Reset();
};
