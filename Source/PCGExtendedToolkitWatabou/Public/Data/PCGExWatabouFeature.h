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
	explicit FPCGExWatabouFeature(const FName InId);

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
	explicit FPCGExWatabouFeaturePoint(const FName InId);


	UPROPERTY(EditAnywhere, Category=Settings)
	FVector2D Location = FVector2D::ZeroVector;
};

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeatureMultiPoint : public FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeatureMultiPoint() = default;
	explicit FPCGExWatabouFeatureMultiPoint(const FName InId, const int32 InReserve = 0);

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FVector2D> Elements;
};

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeaturePolygon : public FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeaturePolygon() = default;
	explicit FPCGExWatabouFeaturePolygon(const FName InId, const int32 InReserve = 0);

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FVector2D> Elements;
};

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeatureMultiPolygon : public FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeatureMultiPolygon() = default;
	explicit FPCGExWatabouFeatureMultiPolygon(const FName InId, const int32 InReserve = 0);

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FPCGExWatabouFeaturePolygon> Elements;
};

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeatureLineString : public FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeatureLineString() = default;
	explicit FPCGExWatabouFeatureLineString(const FName InId, const int32 InReserve = 0);

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FVector2D> Elements;
};

/**
 * 
 */
USTRUCT()
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeatureMultiLineString : public FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeatureMultiLineString() = default;
	explicit FPCGExWatabouFeatureMultiLineString(const FName InId, const int32 InReserve = 0);

	UPROPERTY(EditAnywhere, Category=Settings)
	TArray<FPCGExWatabouFeatureLineString> Elements;
};

/**
 * 
 */
UCLASS()
class PCGEXTENDEDTOOLKITWATABOU_API UPCGExWatabouFeaturesCollection : public UObject
{
	GENERATED_BODY()

public:
	UPCGExWatabouFeaturesCollection() = default;

	UPROPERTY(EditAnywhere, Category=Settings)
	FName Id = NAME_None;

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
	TArray<TObjectPtr<UPCGExWatabouFeaturesCollection>> Collections;

	void Reset();
	bool IsValidCollection() const;
};
