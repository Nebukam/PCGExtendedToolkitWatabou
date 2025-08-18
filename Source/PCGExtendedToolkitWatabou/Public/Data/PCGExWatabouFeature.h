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

	UPROPERTY(EditAnywhere, Category=Settings, meta=(DisplayPriority=-1))
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

	UPROPERTY(EditAnywhere, Category=Data)
	FName Id = NAME_None;

	/** Isolated segments elements */
	UPROPERTY(EditAnywhere, Category=Data, meta=(TitleProperty="{Id}"))
	TArray<FPCGExWatabouFeatureLineString> LineStrings;

	/** Path-like elements */
	UPROPERTY(EditAnywhere, Category=Data, meta=(TitleProperty="{Id}"))
	TArray<FPCGExWatabouFeatureMultiLineString> MultiLineStrings;

	/** Areas */
	UPROPERTY(EditAnywhere, Category=Data, meta=(TitleProperty="{Id}"))
	TArray<FPCGExWatabouFeaturePolygon> Polygons;

	/** Singular points, rare occurence */
	UPROPERTY(EditAnywhere, Category=Data, meta=(TitleProperty="{Id}"))
	TArray<FPCGExWatabouFeaturePoint> Points;

	/** Single elements, a.k.a trees */
	UPROPERTY(EditAnywhere, Category=Data, meta=(TitleProperty="{Id}"))
	TArray<FPCGExWatabouFeatureMultiPoint> MultiPoints;

	/** Sub-collections of elements */
	UPROPERTY(EditAnywhere, Instanced, Category=Data, meta=(TitleProperty="{Id}"))
	TArray<TObjectPtr<UPCGExWatabouFeaturesCollection>> SubCollections;

	UPROPERTY(EditAnywhere, Category = Data)
	double RoadWidth = 0;

	UPROPERTY(EditAnywhere, Category = Data)
	double WallThickness = 0;

	void Reset();
	bool IsValidCollection() const;
};
