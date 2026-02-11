// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/


#include "Importers/PCGExWatabouImporter.h"

#include "PCGExElementsWatabou.h"
#include "PCGExWatabou.h"
#include "Data/PCGExWatabouData.h"
#include "Data/PCGExWatabouFeature.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"


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
		//X axis is flipped
		OutValue = FVector2D(Vector2D[0]->AsNumber()*-1, Vector2D[1]->AsNumber());
		return true;
	}

	bool IImporter::PreBuild(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouData* InData)
	{
		return true;
	}

	void IImporter::Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouFeaturesCollection* InCollection, UPCGExWatabouData* InData)
	{
	}

	void IImporter::BuildDetails(const TSharedPtr<FJsonObject>& InFeatureObj, UPCGExWatabouFeaturesCollection* InCollection, FPCGExFeatureDetails* InDetails)
	{
		const int32 InIdx = InCollection->Elements.Num() - 1;

		FPCGExFeatureDetails* OutDetails = InDetails;
		FPCGExFeatureDetails LocalDetails{};

		if (!InDetails) { OutDetails = &LocalDetails; }

		for (const auto& Pair : InFeatureObj->Values)
		{
			const FName DetailId = FName(Pair.Key);

			if (IgnoredDetails.Contains(DetailId)) { continue; }

			if (Pair.Value->Type == EJson::Number) { OutDetails->NumericValues.Add(FName(Pair.Key), Pair.Value->AsNumber()); }
			else if (Pair.Value->Type == EJson::String) { OutDetails->StringValues.Add(FName(Pair.Key), Pair.Value->AsString()); }
			else if (Pair.Value->Type == EJson::Object) { BuildDetails(InFeatureObj, InCollection, OutDetails); }
		}

		if (!LocalDetails.IsEmpty()) { InCollection->ElementsDetails.Add(InIdx, MoveTemp(LocalDetails)); }
	}

	bool IImporter::PostBuild(UPCGExWatabouData* InData)
	{
		if (!InData->Features->IsValidCollection())
		{
			UE_LOG(LogPCGExWatabou, Error, TEXT("Found no valid data."))
			return false;
		}

		return true;
	}

	void IGeometryImporter::Build(const TSharedPtr<FJsonObject>& InJson, UPCGExWatabouFeaturesCollection* InCollection, UPCGExWatabouData* InData)
	{
#define PCGEX_REMOVE_LAST_FEATURE InCollection->Elements.RemoveAt(InCollection->Elements.Num() - 1); continue;

		const TArray<TSharedPtr<FJsonValue>>* FeaturesArray = nullptr;

		BuildDetails(InJson, InCollection, &InCollection->Details);

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

			EPCGExWatabouFeatureType Type = EPCGExWatabouFeatureType::Unknown;

			FString FeatureType;
			FString FeatureIdStr;
			FName FeatureId = NAME_None;

			if (FeatureObj->TryGetStringField(TEXT("id"), FeatureIdStr)) { FeatureId = FName(*FeatureIdStr); }
			else { FeatureId = InCollection->Id; }

			if (FeatureObj->TryGetStringField(TEXT("type"), FeatureType))
			{
				if (FeatureType == PCGExWatabou::FeatureTypeFeature)
				{
					BuildDetails(FeatureObj, InCollection, &InCollection->Details);
					continue;
				}

				if (FeatureType == PCGExWatabou::FeatureTypeGeometryCollection) { Type = EPCGExWatabouFeatureType::Collection; }
				else if (FeatureType == PCGExWatabou::FeatureTypePoint) { Type = EPCGExWatabouFeatureType::Point; }
				else if (FeatureType == PCGExWatabou::FeatureTypeMultiPoint) { Type = EPCGExWatabouFeatureType::MultiPoints; }
				else if (FeatureType == PCGExWatabou::FeatureTypeLineString) { Type = EPCGExWatabouFeatureType::LineString; }
				else if (FeatureType == PCGExWatabou::FeatureTypeMultiLineString) { Type = EPCGExWatabouFeatureType::LineString; }
				else if (FeatureType == PCGExWatabou::FeatureTypePolygon) { Type = EPCGExWatabouFeatureType::Polygon; }
				else if (FeatureType == PCGExWatabou::FeatureTypeMultiPolygon) { Type = EPCGExWatabouFeatureType::Polygon; }

				int32& Count = InData->Identifiers.FindOrAdd(FPCGExFeatureIdentifier(Type, FeatureId), 0);

				if (FeatureType == PCGExWatabou::FeatureTypeGeometryCollection)
				{
					UPCGExWatabouFeaturesCollection* NewGeometryCollection = NewObject<UPCGExWatabouFeaturesCollection>(InCollection);
					NewGeometryCollection->Id = FeatureId;

					Build(FeatureObj, NewGeometryCollection, InData);

					if (NewGeometryCollection->IsValidCollection())
					{
						NewGeometryCollection->SetFlags(RF_Transactional);
						NewGeometryCollection->MarkPackageDirty();

						InCollection->SubCollections.Add(NewGeometryCollection);
						Count++;
					}

					continue;
				}

				const TArray<TSharedPtr<FJsonValue>>* Coordinates = nullptr;
				if (!FeatureObj->TryGetArrayField(TEXT("coordinates"), Coordinates)) { continue; }

				const int32 NumCoordinates = Coordinates ? Coordinates->Num() : 0;
				if (!NumCoordinates) { continue; }

				if (FeatureType == PCGExWatabou::FeatureTypePoint)
				{
					UE_LOG(LogPCGExWatabou, Error, TEXT("Single point parsing not implemented yet"));
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
							FPCGExWatabouFeature& Point = InCollection->Elements.Emplace_GetRef(EPCGExWatabouFeatureType::Point, FeatureId);
							Point.Coordinates.Add(Coord);
							Count++;
						}
					}
					else
					{
						FPCGExWatabouFeature& MultiPoints = InCollection->Elements.Emplace_GetRef(EPCGExWatabouFeatureType::MultiPoints, FeatureId);
						MultiPoints.Coordinates.Reserve(NumCoordinates);
						if (!Build(MultiPoints, *Coordinates)) { PCGEX_REMOVE_LAST_FEATURE }
						BuildDetails(FeatureObj, InCollection);
						Count++;
					}
				}
				else if (FeatureType == PCGExWatabou::FeatureTypeLineString)
				{
					FPCGExWatabouFeature& NewString = InCollection->Elements.Emplace_GetRef(EPCGExWatabouFeatureType::LineString, FeatureId);
					NewString.Coordinates.Reserve(NumCoordinates);

					if (!Build(NewString, *Coordinates)) { PCGEX_REMOVE_LAST_FEATURE }
					BuildDetails(FeatureObj, InCollection);
					Count++;
				}
				else if (FeatureType == PCGExWatabou::FeatureTypeMultiLineString)
				{
					for (const TSharedPtr<FJsonValue>& PathObj : *Coordinates)
					{
						const TArray<TSharedPtr<FJsonValue>>& PathPoints = PathObj->AsArray();

						const int32 NumCoords = PathPoints.Num();
						if (!NumCoords) { continue; }

						FPCGExWatabouFeature& NewString = InCollection->Elements.Emplace_GetRef(EPCGExWatabouFeatureType::LineString, FeatureId);
						NewString.Coordinates.Reserve(NumCoords);

						if (!Build(NewString, PathPoints)) { PCGEX_REMOVE_LAST_FEATURE }
						BuildDetails(FeatureObj, InCollection);
						Count++;
					}
				}
				else if (FeatureType == PCGExWatabou::FeatureTypePolygon)
				{
					for (const TSharedPtr<FJsonValue>& VerticesObj : *Coordinates)
					{
						const TArray<TSharedPtr<FJsonValue>>& Vertices = VerticesObj->AsArray();

						const int32 NumCoords = Vertices.Num();
						if (!NumCoords) { continue; }

						FPCGExWatabouFeature& NewPolygon = InCollection->Elements.Emplace_GetRef(EPCGExWatabouFeatureType::Polygon, FeatureId);
						NewPolygon.Coordinates.Reserve(NumCoords);

						if (!Build(NewPolygon, Vertices)) { PCGEX_REMOVE_LAST_FEATURE }
						BuildDetails(FeatureObj, InCollection);
						Count++;

						if (FeatureIdStr == TEXT("earth"))
						{
							// Special polygon representing world bounds
							if (InData)
							{
								FBox Bounds = FBox(ForceInit);
								for (FVector2D& Point : NewPolygon.Coordinates) { Bounds += FVector(Point, 0); }

								// Pad Z
								Bounds += FVector(0, 0, -1);
								Bounds += FVector(0, 0, 1);

								InData->Bounds = Bounds;
							}
						}
					}
				}
				else if (FeatureType == PCGExWatabou::FeatureTypeMultiPolygon)
				{
					for (const TSharedPtr<FJsonValue>& PolygonsObj : *Coordinates)
					{
						const TArray<TSharedPtr<FJsonValue>>& Polygons = PolygonsObj->AsArray();
						for (const TSharedPtr<FJsonValue>& VerticesObj : Polygons)
						{
							const TArray<TSharedPtr<FJsonValue>>& Vertices = VerticesObj->AsArray();

							const int32 NumCoords = Vertices.Num();
							if (!NumCoords) { continue; }

							FPCGExWatabouFeature& NewPolygon = InCollection->Elements.Emplace_GetRef(EPCGExWatabouFeatureType::Polygon, FeatureId);
							NewPolygon.Coordinates.Reserve(NumCoords);

							if (!Build(NewPolygon, Vertices)) { PCGEX_REMOVE_LAST_FEATURE }
							BuildDetails(FeatureObj, InCollection);
							Count++;
						}
					}
				}
			}
		}

#undef PCGEX_REMOVE_LAST_FEATURE
	}

	int32 IGeometryImporter::Build(FPCGExWatabouFeature& InElement, const TArray<TSharedPtr<FJsonValue>>& InCoordinates)
	{
		return LoopOverCoordinates(
			InCoordinates,
			[&](const FVector2D& Point, const int32 i)
			{
				InElement.Coordinates.Emplace(Point);
			});
	}
}
