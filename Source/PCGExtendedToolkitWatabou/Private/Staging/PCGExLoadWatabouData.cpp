// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#include "Staging/PCGExLoadWatabouData.h"

#include "PCGComponent.h"
#include "Data/PCGExDataTag.h"
#include "Data/PCGExWatabouData.h"
#include "Paths/PCGExPaths.h"

#define LOCTEXT_NAMESPACE "PCGExLoadWatabouData"
#define PCGEX_NAMESPACE LoadWatabouData

PCGEX_INITIALIZE_ELEMENT(LoadWatabouData)

TArray<FPCGPinProperties> UPCGExLoadWatabouDataSettings::OutputPinProperties() const
{
	TArray<FPCGPinProperties> PinProperties;
	PCGEX_PIN_POINTS(GetMainOutputPin(), "Default output pin. Data that doesn't make it to specific pins ends up here.", Normal, {})
	for (const FName PinName : IdToPins)
	{
		PCGEX_PIN_POINTS(PinName, "Pin that will contain all data with the specified identifier.", Normal, {})
	}
	PCGEX_PIN_PARAM(FName("Params"), "Top-level values. Arbitrary, depends on generator used.", Normal, {})
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

void FPCGExLoadWatabouDataContext::ProcessCollection(const UPCGExWatabouFeaturesCollection* InCollection)
{
	for (int i = 0; i < InCollection->Elements.Num(); i++)
	{
		const FPCGExWatabouFeature& Feature = InCollection->Elements[i];
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
			NewTask = MakeShared<PCGExLoadWatabouData::FBuildLineString>();
			break;
		case EPCGExWatabouFeatureType::Polygon:
			if (PointifyPolygons.Contains(Feature.Id))
			{
			}
			else
			{
				NewTask = MakeShared<PCGExLoadWatabouData::FBuildPolygon>();
			}
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

	for (const UPCGExWatabouFeaturesCollection* Collection : InCollection->SubCollections) { ProcessCollection(Collection); }
}

bool FPCGExLoadWatabouDataElement::Boot(FPCGExContext* InContext) const
{
	if (!FPCGExPointsProcessorElement::Boot(InContext)) { return false; }

	PCGEX_CONTEXT_AND_SETTINGS(LoadWatabouData)

	Context->IdAsPins.Append(Settings->IdToPins);
	
	if (Settings->bDoPointifyPolygons) { Context->PointifyPolygons = Settings->PointifyPolygons; }
	if (Settings->bDoPointifyLines) { Context->PointifyLines = Settings->PointifyLines; }

	Context->WatabouData = PCGExHelpers::LoadBlocking_AnyThread(Settings->DataAsset);
	if (!Context->WatabouData)
	{
		PCGE_LOG_C(Error, GraphAndLog, InContext, FTEXT("Missing specified Tag attribute."));
		return false;
	}

	Context->WatabouData->AddToRoot();

	return true;
}

bool FPCGExLoadWatabouDataElement::ExecuteInternal(
	FPCGContext* InContext) const
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
const FPCGExWatabouFeature& Feature = ParentCollection->Elements[ElementIndex]; \
if(!Feature.Id.IsNone() && Context->IdAsPins.Contains(Feature.Id)){ PointIO->OutputPin = Feature.Id; }


namespace PCGExLoadWatabouData
{
	void FBuildMultiPoints::ExecuteTask(const TSharedPtr<PCGExMT::FTaskManager>& AsyncManager)
	{
		PCGEX_FEATURE

		PCGEx::SetNumPointsAllocated(PointIO->GetOut(), Feature.Coordinates.Num());

		TPCGValueRange<FTransform> OutTransform = PointIO->GetOut()->GetTransformValueRange();

		for (int i = 0; i < Feature.Coordinates.Num(); i++)
		{
			OutTransform[i].SetLocation(FVector(Feature.Coordinates[i], 0));
		}
	}

	void FBuildLineString::ExecuteTask(const TSharedPtr<PCGExMT::FTaskManager>& AsyncManager)
	{
		PCGEX_FEATURE

		PCGEx::SetNumPointsAllocated(PointIO->GetOut(), Feature.Coordinates.Num());

		TPCGValueRange<FTransform> OutTransform = PointIO->GetOut()->GetTransformValueRange();

		for (int i = 0; i < Feature.Coordinates.Num(); i++)
		{
			OutTransform[i].SetLocation(FVector(Feature.Coordinates[i], 0));
		}

		PCGExPaths::SetClosedLoop(PointIO->GetOut(), false);
		PointIO->Tags->AddRaw(Settings->PathlikeTag);
	}

	void FBuildPolygon::ExecuteTask(const TSharedPtr<PCGExMT::FTaskManager>& AsyncManager)
	{
		PCGEX_FEATURE

		PCGEx::SetNumPointsAllocated(PointIO->GetOut(), Feature.Coordinates.Num());

		TPCGValueRange<FTransform> OutTransform = PointIO->GetOut()->GetTransformValueRange();

		for (int i = 0; i < Feature.Coordinates.Num(); i++)
		{
			OutTransform[i].SetLocation(FVector(Feature.Coordinates[i], 0));
		}

		PCGExPaths::SetClosedLoop(PointIO->GetOut(), true);
		PointIO->Tags->AddRaw(Settings->PathlikeTag);
	}
}

#undef PCGEX_FEATURE
#undef LOCTEXT_NAMESPACE


#undef LOCTEXT_NAMESPACE
#undef PCGEX_NAMESPACE
