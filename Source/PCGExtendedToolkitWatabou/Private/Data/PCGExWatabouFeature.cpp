// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/


#include "Data/PCGExWatabouFeature.h"

FPCGExFeatureIdentifier::FPCGExFeatureIdentifier(const EPCGExWatabouFeatureType InType, const FName InId)
	: Type(InType), Id(InId)
{
}

FPCGExWatabouFeature::FPCGExWatabouFeature(const EPCGExWatabouFeatureType InType, const FName InId)
	: Type(InType), Id(InId)
{
}

FPCGExFeatureIdentifier FPCGExWatabouFeature::GetIdentifier() const
{
	return FPCGExFeatureIdentifier(Type, Id);
}

void UPCGExWatabouFeaturesCollection::Reset()
{
	Elements.Empty();
	SubCollections.Empty();
}

bool UPCGExWatabouFeaturesCollection::IsValidCollection() const
{
	return !(Elements.IsEmpty() && SubCollections.IsEmpty());
}
