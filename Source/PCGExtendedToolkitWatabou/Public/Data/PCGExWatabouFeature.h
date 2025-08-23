// Copyright (c) Nebukam

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "PCGExWatabouFeature.generated.h"

UENUM(BlueprintType)
enum class EPCGExWatabouFeatureType : uint8
{
	Unknown     = 0,
	Point       = 1,
	MultiPoints = 2,
	LineString  = 3,
	Polygon     = 5,
	Collection  = 42,
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExFeatureDetails
{
	GENERATED_BODY()

	FPCGExFeatureDetails() = default;

	UPROPERTY(VisibleAnywhere, Category=Settings)
	TMap<FName, FString> StringValues;

	UPROPERTY(VisibleAnywhere, Category=Settings)
	TMap<FName, double> NumericValues;

	bool IsEmpty() const { return StringValues.IsEmpty() && NumericValues.IsEmpty(); }
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExFeatureIdentifier
{
	GENERATED_BODY()

	FPCGExFeatureIdentifier() = default;
	FPCGExFeatureIdentifier(const EPCGExWatabouFeatureType InType, const FName InId);

	UPROPERTY(VisibleAnywhere, Category=Settings)
	EPCGExWatabouFeatureType Type = EPCGExWatabouFeatureType::Unknown;

	UPROPERTY(VisibleAnywhere, Category=Settings)
	FName Id = NAME_None;

	bool operator==(const FPCGExFeatureIdentifier& Other) const
	{
		return Other.Id == Id && Other.Type == Type;
	}

	FORCEINLINE friend uint32 GetTypeHash(const FPCGExFeatureIdentifier InIdentifier)
	{
		return HashCombineFast(GetTypeHash(InIdentifier.Id), GetTypeHash(InIdentifier.Type));
	}
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct PCGEXTENDEDTOOLKITWATABOU_API FPCGExWatabouFeature
{
	GENERATED_BODY()

	FPCGExWatabouFeature() = default;
	FPCGExWatabouFeature(const EPCGExWatabouFeatureType InType, const FName InId);

	UPROPERTY(VisibleAnywhere, Category=Settings)
	EPCGExWatabouFeatureType Type = EPCGExWatabouFeatureType::Unknown;

	UPROPERTY(VisibleAnywhere, Category=Settings)
	FName Id = NAME_None;

	UPROPERTY(VisibleAnywhere, Category=Settings)
	TArray<FVector2D> Coordinates;

	FPCGExFeatureIdentifier GetIdentifier() const;
};

/**
 * 
 */
UCLASS(BlueprintType)
class PCGEXTENDEDTOOLKITWATABOU_API UPCGExWatabouFeaturesCollection : public UObject
{
	GENERATED_BODY()

public:
	UPCGExWatabouFeaturesCollection() = default;

	/**  */
	UPROPERTY(VisibleAnywhere, Category=Data)
	FPCGExFeatureDetails Details;
	
	UPROPERTY(VisibleAnywhere, Category=Data)
	FName Id = NAME_None;

	/** Isolated segments elements */
	UPROPERTY(VisibleAnywhere, Category=Data, meta=(TitleProperty="{Id}"))
	TArray<FPCGExWatabouFeature> Elements;

	/** Sub-collections of elements */
	UPROPERTY(VisibleAnywhere, Instanced, Category=Data, meta=(TitleProperty="{Id}"))
	TArray<TObjectPtr<UPCGExWatabouFeaturesCollection>> SubCollections;

	/** Feature details are rare, so they're stored per-index */
	UPROPERTY(VisibleAnywhere, Category=Data)
	TMap<int32, FPCGExFeatureDetails> ElementsDetails;

	void Reset();
	bool IsValidCollection() const;
};
