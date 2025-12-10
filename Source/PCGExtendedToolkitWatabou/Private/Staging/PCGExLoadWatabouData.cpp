// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#include "Staging/PCGExLoadWatabouData.h"

#include "PCGComponent.h"
#include "PCGExStreamingHelpers.h"
#include "PCGParamData.h"
#include "Data/PCGBasePointData.h"
#include "Data/PCGExDataHelpers.h"
#include "Data/PCGExDataTag.h"
#include "Data/PCGExPointIO.h"
#include "Data/PCGExWatabouData.h"
#include "Paths/PCGExPaths.h"

#define LOCTEXT_NAMESPACE "PCGExLoadWatabouData"
#define PCGEX_NAMESPACE LoadWatabouData

PCGEX_INITIALIZE_ELEMENT(LoadWatabouData)

TArray<FPCGPinProperties> UPCGExLoadWatabouDataSettings::OutputPinProperties() const
{
	TArray<FPCGPinProperties> PinProperties;
	PCGEX_PIN_POINTS(GetMainOutputPin(), "Default output pin. Data that doesn't make it to specific pins ends up here.", Normal)

	for (const FName PinName : IdToPins)
	{
		PCGEX_PIN_POINTS(PinName, "Pin that will contain all data with the specified identifier.", Normal)
	}
	
	PCGEX_PIN_PARAM(FName("Params"), "Top-level values. Arbitrary, depends on generator used.", Normal)
	return PinProperties;
}

#if WITH_EDITOR
void UPCGExLoadWatabouDataSettings::EDITOR_IdToPins()
{
	UPCGExWatabouData* WatabouData = DataAsset.LoadSynchronous();
	if (!WatabouData) { return; }

	Modify(true);

	IdToPins.Empty();

	for (const TPair<FPCGExFeatureIdentifier, int32>& Pair : WatabouData->Identifiers)
	{
		if (!Pair.Value) { continue; }
		IdToPins.Add(Pair.Key.Id);
	}

	FPropertyChangedEvent EmptyEvent(nullptr);
	PostEditChangeProperty(EmptyEvent);

	MarkPackageDirty();
}

void UPCGExLoadWatabouDataSettings::EDITOR_AppendIdToPins()
{
	UPCGExWatabouData* WatabouData = DataAsset.LoadSynchronous();
	if (!WatabouData) { return; }

	Modify(true);

	for (const TPair<FPCGExFeatureIdentifier, int32>& Pair : WatabouData->Identifiers)
	{
		if (!Pair.Value) { continue; }
		IdToPins.AddUnique(Pair.Key.Id);
	}

	FPropertyChangedEvent EmptyEvent(nullptr);
	PostEditChangeProperty(EmptyEvent);

	MarkPackageDirty();
}
#endif

FPCGExLoadWatabouDataContext::~FPCGExLoadWatabouDataContext()
{
	if (WatabouData) { WatabouData->RemoveFromRoot(); }
}

void FPCGExLoadWatabouDataContext::WriteDetails(UPCGData* InData, const int32 ElementIndex, const UPCGExWatabouFeaturesCollection* Collection) const
{
	const FPCGExFeatureDetails* Details = Collection->ElementsDetails.Find(ElementIndex);

	if (!Details) { return; }

	UPCGMetadata* Metadata = InData->MutableMetadata();

	for (const TPair<FName, FString>& Pair : Details->StringValues)
	{
		FPCGAttributeIdentifier Identifier(Pair.Key);
		Identifier.MetadataDomain = EPCGMetadataDomainFlag::Data;
		Metadata->CreateAttribute(Identifier, Pair.Value, true, true);
	}

	for (const TPair<FName, double>& Pair : Details->NumericValues)
	{
		FPCGAttributeIdentifier Identifier(Pair.Key);
		Identifier.MetadataDomain = EPCGMetadataDomainFlag::Data;
		Metadata->CreateAttribute(Identifier, Pair.Value * DataScaleFactor, true, true);
	}
}

void FPCGExLoadWatabouDataContext::ProcessCollection(const UPCGExWatabouFeaturesCollection* InCollection)
{
	TMap<FName, TArray<int32>> PointifyList;

	auto Pointify = [&](const FPCGExWatabouFeature& Feature, int32 ElementIndex)
	{
		TArray<int32>* ListPtr = PointifyList.Find(Feature.Id);
		if (!ListPtr)
		{
			TArray<int32>& List = PointifyList.Add(Feature.Id);
			const int32* NumPtr = WatabouData->Identifiers.Find(Feature.GetIdentifier());
			List.Reserve(NumPtr ? *NumPtr : InCollection->Elements.Num());
			List.Add(ElementIndex);
		}
		else { ListPtr->Emplace(ElementIndex); }
	};

	for (int i = 0; i < InCollection->Elements.Num(); i++)
	{
		const FPCGExWatabouFeature& Feature = InCollection->Elements[i];
		if (SkipIds.Contains(Feature.Id)) { continue; }
		
		TSharedPtr<PCGExLoadWatabouData::FBuildFeature> NewTask = nullptr;

		switch (Feature.Type)
		{
		case EPCGExWatabouFeatureType::Unknown:
			break;
		case EPCGExWatabouFeatureType::Point:
			// TODO : Merge all, not supported yet
			break;
		case EPCGExWatabouFeatureType::MultiPoints:
			NewTask = MakeShared<PCGExLoadWatabouData::FBuildMultiPoints>();
			break;
		case EPCGExWatabouFeatureType::LineString:
			if (PointifyLines.Contains(Feature.Id))
			{
				Pointify(Feature, i);
				continue;
			}
			else { NewTask = MakeShared<PCGExLoadWatabouData::FBuildLineString>(); }
			break;
		case EPCGExWatabouFeatureType::Polygon:
			if (PointifyPolygons.Contains(Feature.Id))
			{
				Pointify(Feature, i);
				continue;
			}
			else { NewTask = MakeShared<PCGExLoadWatabouData::FBuildPolygon>(); }
			break;
		case EPCGExWatabouFeatureType::Collection:
			// Should never hit that
			break;
		}

		if (!NewTask) { continue; }

		NewTask->ParentCollection = InCollection;
		NewTask->ElementIndex = i;
		NewTask->PointIO = MainPoints->Emplace_GetRef();

		Tasklist.Add(NewTask);
		IndexTracker++;
	}

	for (TPair<FName, TArray<int32>>& Pair : PointifyList)
	{
		TSharedPtr<PCGExLoadWatabouData::FBuildPointified> NewTask = MakeShared<PCGExLoadWatabouData::FBuildPointified>();
		NewTask->ParentCollection = InCollection;
		NewTask->ElementIndex = IndexTracker++;
		NewTask->Elements = MoveTemp(Pair.Value);
		NewTask->PointIO = MainPoints->Emplace_GetRef();

		Tasklist.Add(NewTask);
		IndexTracker++;
	}

	for (const UPCGExWatabouFeaturesCollection* Collection : InCollection->SubCollections) { ProcessCollection(Collection); }
}

bool FPCGExLoadWatabouDataElement::Boot(FPCGExContext* InContext) const
{
	if (!FPCGExPointsProcessorElement::Boot(InContext)) { return false; }

	PCGEX_CONTEXT_AND_SETTINGS(LoadWatabouData)

	Context->IdAsPins.Append(Settings->IdToPins);

	PCGEX_FWD(SkipIds)

	if (Settings->bDoPointifyPolygons) { Context->PointifyPolygons = Settings->PointifyPolygons; }
	if (Settings->bDoPointifyLines) { Context->PointifyLines = Settings->PointifyLines; }

	Context->WatabouData = PCGExHelpers::LoadBlocking_AnyThread(Settings->DataAsset);
	if (!Context->WatabouData)
	{
		PCGE_LOG_C(Error, GraphAndLog, InContext, FTEXT("Missing specified Tag attribute."));
		return false;
	}

	Context->WatabouData->AddToRoot();
	Context->DataScaleFactor = Settings->DataScaleFactor;
	Context->MainTransform = Settings->Transform;
	Context->MainTransform.SetScale3D(Settings->Transform.GetScale3D() * Settings->ScaleFactor);

	return true;
}

bool FPCGExLoadWatabouDataElement::AdvanceWork(FPCGExContext* InContext, const UPCGExSettings* InSettings) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FPCGExLoadWatabouDataElement::Execute);

	PCGEX_CONTEXT_AND_SETTINGS(LoadWatabouData)
	PCGEX_EXECUTION_CHECK
	PCGEX_ON_INITIAL_EXECUTION
	{
		Context->ProcessCollection(Context->WatabouData->Features);
		if (Context->Tasklist.IsEmpty()) { return Context->CancelExecution(TEXT("Could not generate any points from the selected data.")); }

		const TSharedPtr<PCGExMT::FTaskGroup> TaskGroup = Context->GetAsyncManager() ? Context->GetAsyncManager()->TryCreateTaskGroup(FName("TaskGroup")) : nullptr;
		if (!TaskGroup) { return true; }

		Context->SetAsyncState(PCGExCommon::State_WaitingOnAsyncWork);
		TaskGroup->StartTasksBatch(Context->Tasklist);
		return false;
	}

	PCGEX_ON_ASYNC_STATE_READY(PCGExCommon::State_WaitingOnAsyncWork)
	{
		Context->MainPoints->StageOutputs();
		Context->Done();
	}

	return Context->TryComplete();
}

bool FPCGExLoadWatabouDataElement::CanExecuteOnlyOnMainThread(FPCGContext* Context) const
{
	return Context ? Context->CurrentPhase == EPCGExecutionPhase::PrepareData : false;
}

#define PCGEX_FEATURE \
FPCGExLoadWatabouDataContext* Context = static_cast<FPCGExLoadWatabouDataContext*>(PointIO->GetContext()); \
PCGEX_SETTINGS(LoadWatabouData) \
Context->WriteDetails(PointIO->GetOut(), ElementIndex, ParentCollection); \
const FPCGExWatabouFeature& Feature = ParentCollection->Elements[ElementIndex]; \
if(!Feature.Id.IsNone()){ \
	if(Context->IdAsPins.Contains(Feature.Id)){ PointIO->OutputPin = Feature.Id; } \
	PointIO->Tags->AddRaw(Feature.Id.ToString());\
}


namespace PCGExLoadWatabouData
{
	void FBuildMultiPoints::ExecuteTask(const TSharedPtr<PCGExMT::FTaskManager>& AsyncManager)
	{
		PCGEX_FEATURE

		PCGEx::SetNumPointsAllocated(PointIO->GetOut(), Feature.Coordinates.Num(), EPCGPointNativeProperties::Transform);

		TPCGValueRange<FTransform> OutTransform = PointIO->GetOut()->GetTransformValueRange();

		for (int i = 0; i < Feature.Coordinates.Num(); i++)
		{
			OutTransform[i].SetLocation(Context->MainTransform.TransformPosition(FVector(Feature.Coordinates[i], 0)));
		}
	}

	void FBuildLineString::ExecuteTask(const TSharedPtr<PCGExMT::FTaskManager>& AsyncManager)
	{
		PCGEX_FEATURE

		PCGEx::SetNumPointsAllocated(PointIO->GetOut(), Feature.Coordinates.Num(), EPCGPointNativeProperties::Transform);

		TPCGValueRange<FTransform> OutTransform = PointIO->GetOut()->GetTransformValueRange();

		for (int i = 0; i < Feature.Coordinates.Num(); i++)
		{
			OutTransform[i].SetLocation(Context->MainTransform.TransformPosition(FVector(Feature.Coordinates[i], 0)));
		}

		PCGExPaths::SetClosedLoop(PointIO->GetOut(), false);
		PointIO->Tags->AddRaw(Settings->PathlikeTag);
	}

	void FBuildPolygon::ExecuteTask(const TSharedPtr<PCGExMT::FTaskManager>& AsyncManager)
	{
		PCGEX_FEATURE

		PCGEx::SetNumPointsAllocated(PointIO->GetOut(), Feature.Coordinates.Num(), EPCGPointNativeProperties::Transform);

		TPCGValueRange<FTransform> OutTransform = PointIO->GetOut()->GetTransformValueRange();

		for (int i = 0; i < Feature.Coordinates.Num(); i++)
		{
			OutTransform[i].SetLocation(Context->MainTransform.TransformPosition(FVector(Feature.Coordinates[i], 0)));
		}

		PCGExPaths::SetClosedLoop(PointIO->GetOut(), true);
		PointIO->Tags->AddRaw(Settings->PathlikeTag);
	}

	void FBuildPointified::ExecuteTask(const TSharedPtr<PCGExMT::FTaskManager>& AsyncManager)
	{
		PCGEX_FEATURE

		PCGEx::SetNumPointsAllocated(
			PointIO->GetOut(), Elements.Num(),
			EPCGPointNativeProperties::Transform | EPCGPointNativeProperties::BoundsMin | EPCGPointNativeProperties::BoundsMax);

		TPCGValueRange<FTransform> OutTransform = PointIO->GetOut()->GetTransformValueRange();
		TPCGValueRange<FVector> OutBoundsMin = PointIO->GetOut()->GetBoundsMinValueRange();
		TPCGValueRange<FVector> OutBoundsMax = PointIO->GetOut()->GetBoundsMaxValueRange();

		const FVector Flattener = FVector(-1, -1, 0);

		for (int i = 0; i < Elements.Num(); i++)
		{
			const FPCGExWatabouFeature& LocalFeature = ParentCollection->Elements[Elements[i]];
			PCGExGeo::FBestFitPlane Plane(LocalFeature.Coordinates);
			Plane.Extents.Z = Plane.Extents.Y * 2;

			OutTransform[i] = Plane.GetTransform() * Context->MainTransform;
			OutBoundsMin[i] = Plane.Extents * Flattener;
			OutBoundsMax[i] = Plane.Extents;
		}
	}
}

#undef PCGEX_FEATURE
#undef LOCTEXT_NAMESPACE


#undef LOCTEXT_NAMESPACE
#undef PCGEX_NAMESPACE
