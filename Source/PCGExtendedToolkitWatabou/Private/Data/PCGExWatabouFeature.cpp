// Copyright (c) Nebukam


#include "Data/PCGExWatabouFeature.h"

void UPCGExWatabouFeatureGeometryCollection::Reset()
{
	LineStrings.Empty();
	MultiLineStrings.Empty();
	Polygons.Empty();
	MultiPolygons.Empty();
	Points.Empty();
	MultiPoints.Empty();
	Collections.Empty();
}
