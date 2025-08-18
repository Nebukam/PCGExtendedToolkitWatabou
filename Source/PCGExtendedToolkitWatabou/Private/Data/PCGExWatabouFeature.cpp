// Copyright (c) Nebukam


#include "Data/PCGExWatabouFeature.h"

FPCGExWatabouFeature::FPCGExWatabouFeature(const FName InId)
	: Id(InId)
{
}

FPCGExWatabouFeaturePoint::FPCGExWatabouFeaturePoint(const FName InId)
	: FPCGExWatabouFeature(InId)
{
}

FPCGExWatabouFeatureMultiPoint::FPCGExWatabouFeatureMultiPoint(const FName InId, const int32 InReserve)
	: FPCGExWatabouFeature(InId)
{
	Elements.Reserve(InReserve);
}

FPCGExWatabouFeaturePolygon::FPCGExWatabouFeaturePolygon(const FName InId, const int32 InReserve)
	: FPCGExWatabouFeature(InId)
{
	Elements.Reserve(InReserve);
}

FPCGExWatabouFeatureMultiPolygon::FPCGExWatabouFeatureMultiPolygon(const FName InId, const int32 InReserve)
: FPCGExWatabouFeature(InId)
{
	Elements.Reserve(InReserve);
}

FPCGExWatabouFeatureLineString::FPCGExWatabouFeatureLineString(const FName InId, const int32 InReserve)
: FPCGExWatabouFeature(InId)
{
	Elements.Reserve(InReserve);
}

FPCGExWatabouFeatureMultiLineString::FPCGExWatabouFeatureMultiLineString(const FName InId, const int32 InReserve)
: FPCGExWatabouFeature(InId)
{
	Elements.Reserve(InReserve);
}

void UPCGExWatabouFeaturesCollection::Reset()
{
	LineStrings.Empty();
	MultiLineStrings.Empty();
	Polygons.Empty();
	MultiPolygons.Empty();
	Points.Empty();
	MultiPoints.Empty();
	Collections.Empty();
}

bool UPCGExWatabouFeaturesCollection::IsValidCollection() const
{
	return
		!(LineStrings.IsEmpty() &&
			MultiLineStrings.IsEmpty() &&
			Polygons.IsEmpty() &&
			MultiPolygons.IsEmpty() &&
			Points.IsEmpty() &&
			MultiPoints.IsEmpty() &&
			Collections.IsEmpty());
}
