// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/


#include "Importers/PCGExWatabouImporter.h"

#include "PCGExWatabou.h"
#include "Data/PCGExWatabouData.h"
#include "Data/PCGExWatabouFeature.h"


namespace PCGExWatabouImporter
{
	int32 LoopOverCoordinates(const TArray<TSharedPtr<FJsonValue>>& InCoordinates, FLoopOverCoordinates&& InFunc)
	{
		int32 i = 0;
		FVector2D Coord;
		for (const TSharedPtr<FJsonValue>& Coordinates : InCoordinates) { if (GetVector2D(Coordinates, Coord)) { InFunc(Coord, i++); } }
		return i;
	}

	bool GetVector2D(const TSharedPtr<FJsonValue>& InObject, FVector2D& OutValue)
	{
		const TArray<TSharedPtr<FJsonValue>>& Vector2D = InObject->AsArray();
		if (Vector2D.Num() != 2) { return false; }
		OutValue = FVector2D(Vector2D[0]->AsNumber(), Vector2D[0]->AsNumber());
		return true;
	}

	void IImporter::Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouFeaturesCollection* InCollection, UPCGExWatabouData* InData)
	{
	}

	void IGeometryImporter::Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouFeaturesCollection* InCollection, UPCGExWatabouData* InData)
	{
		const TArray<TSharedPtr<FJsonValue>>* FeaturesArray = nullptr;

		if (!InJson->TryGetArrayField(TEXT("features"), FeaturesArray) &&
			!InJson->TryGetArrayField(TEXT("geometries"), FeaturesArray))
		{
			return;
		}

		// If we have a feature array this narrows down the types of data to VG/MFCG/Hood
		// Find the feature object
		for (const TSharedPtr<FJsonValue>& FeatureValue : *FeaturesArray)
		{
			TSharedPtr<FJsonObject> FeatureObj = FeatureValue->AsObject();
			if (!FeatureObj) continue;

			FString FeatureType;
			FString FeatureId;

			FeatureObj->TryGetStringField(TEXT("id"), FeatureId);
			if (FeatureObj->TryGetStringField(TEXT("type"), FeatureType))
			{
				if (FeatureType == PCGExWatabou::FeatureTypeGeometryCollection)
				{
					UPCGExWatabouFeaturesCollection* NewGeometryCollection = NewObject<UPCGExWatabouFeaturesCollection>();
					Build(FeatureObj, NewGeometryCollection, InData);

					if (NewGeometryCollection->IsValidCollection())
					{
						NewGeometryCollection->Id = FName(FeatureId);
						InCollection->Collections.Add(NewGeometryCollection);
					}

					continue;
				}

				const TArray<TSharedPtr<FJsonValue>>* Coordinates = nullptr;
				if (!FeatureObj->TryGetArrayField(TEXT("coordinates"), Coordinates)) { continue; }

				const int32 NumCoordinates = Coordinates ? Coordinates->Num() : 0;
				
				if (FeatureType == PCGExWatabou::FeatureTypePoint)
				{
				}
				else if (FeatureType == PCGExWatabou::FeatureTypeMultiPoint)
				{
					if (NumCoordinates <= 0)
					{
					}
					else if (NumCoordinates == 1)
					{
						FVector2D Coord;
						if (GetVector2D((*Coordinates)[0], Coord))
						{
							FPCGExWatabouFeaturePoint& Point = InCollection->Points.Emplace_GetRef(FName(FeatureId));
							Point.Location = Coord;
						}
					}
					else
					{
						FPCGExWatabouFeatureMultiPoint& MultiPoints = InCollection->MultiPoints.Emplace_GetRef(FName(FeatureId), NumCoordinates);
						if (!Build(MultiPoints, *Coordinates)) { InCollection->MultiPoints.RemoveAt(InCollection->MultiPoints.Num() - 1); }
					}
				}
				else if (FeatureType == PCGExWatabou::FeatureTypeLineString)
				{					
					FPCGExWatabouFeatureLineString& NewString = InCollection->LineStrings.Emplace_GetRef(FName(FeatureId), NumCoordinates);
					if (!Build(NewString, *Coordinates)) { InCollection->LineStrings.RemoveAt(InCollection->LineStrings.Num() - 1); }
				}
				else if (FeatureType == PCGExWatabou::FeatureTypeMultiLineString)
				{
					if (!Coordinates) { continue; }
					
					for (const TSharedPtr<FJsonValue>& LineList : *Coordinates)
					{
						const TArray<TSharedPtr<FJsonValue>>& LineListAsArray = LineList->AsArray();
						for (const TSharedPtr<FJsonValue>& SubCoordinates : LineListAsArray)
						{
							const TArray<TSharedPtr<FJsonValue>>& LineCoordsAsArray = SubCoordinates->AsArray();

							const int32 NumCoords = LineCoordsAsArray.Num();
							if (!NumCoords) { continue; }

							FPCGExWatabouFeatureLineString& NewString = InCollection->LineStrings.Emplace_GetRef(FName(FeatureId), NumCoords);
							if (!Build(NewString, *Coordinates)) { InCollection->LineStrings.RemoveAt(InCollection->LineStrings.Num() - 1); }
						}
					}
				}
				else if (FeatureType == PCGExWatabou::FeatureTypePolygon)
				{
					FPCGExWatabouFeaturePolygon& NewPolygon = InCollection->Polygons.Emplace_GetRef(FName(FeatureId), NumCoordinates);

					if (!Build(NewPolygon, *Coordinates))
					{
						InCollection->Polygons.RemoveAt(InCollection->Polygons.Num() - 1);
						continue;
					}

					if (FeatureId == TEXT("earth"))
					{
						// Special polygon representing world bounds
						if (InData)
						{
							FBox Bounds = FBox(ForceInit);
							for (FVector2D& Point : NewPolygon.Elements) { Bounds += FVector(Point, 0); }

							// Pad Z
							Bounds += FVector(0, 0, -1);
							Bounds += FVector(0, 0, 1);

							InData->Bounds = Bounds;
						}
					}
				}
				else if (FeatureType == PCGExWatabou::FeatureTypeMultiPolygon)
				{					
					for (const TSharedPtr<FJsonValue>& PolyList : *Coordinates)
					{
						const TArray<TSharedPtr<FJsonValue>>& PolyListAsArray = PolyList->AsArray();
						for (const TSharedPtr<FJsonValue>& SubCoordinates : PolyListAsArray)
						{
							const TArray<TSharedPtr<FJsonValue>>& PolyCoordsAsArray = SubCoordinates->AsArray();

							const int32 NumCoords = PolyCoordsAsArray.Num();
							if (!NumCoords) { continue; }

							FPCGExWatabouFeaturePolygon& NewPolygon = InCollection->Polygons.Emplace_GetRef(FName(FeatureId), NumCoords);
							if (!Build(NewPolygon, PolyCoordsAsArray)) { InCollection->Polygons.RemoveAt(InCollection->Polygons.Num() - 1); }
						}
					}
				}
			}
		}
	}

	int32 IGeometryImporter::Build(FPCGExWatabouFeatureMultiPoint& InElement, const TArray<TSharedPtr<FJsonValue>>& InCoordinates)
	{
		return LoopOverCoordinates(
			InCoordinates,
			[&](const FVector2D& Point, const int32 i)
			{
				InElement.Elements.Emplace(Point);
			});
	}

	int32 IGeometryImporter::Build(FPCGExWatabouFeatureLineString& InElement, const TArray<TSharedPtr<FJsonValue>>& InCoordinates)
	{
		return LoopOverCoordinates(
			InCoordinates,
			[&](const FVector2D& Point, const int32 i)
			{
				InElement.Elements.Emplace(Point);
			});
	}

	int32 IGeometryImporter::Build(FPCGExWatabouFeaturePolygon& InElement, const TArray<TSharedPtr<FJsonValue>>& InCoordinates)
	{
		return LoopOverCoordinates(
			InCoordinates,
			[&](const FVector2D& Point, const int32 i)
			{
				InElement.Elements.Emplace(Point);
			});
	}
}
