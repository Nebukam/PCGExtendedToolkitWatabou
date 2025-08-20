// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#include "Staging/PCGExLoadWatabouData.h"

#include "PCGComponent.h"
#include "Data/PCGExWatabouData.h"
#include "Paths/PCGExPaths.h"

#define LOCTEXT_NAMESPACE "PCGExLoadWatabouData"
#define PCGEX_NAMESPACE LoadWatabouData

PCGEX_INITIALIZE_ELEMENT(LoadWatabouData)

TArray<FPCGPinProperties> UPCGExLoadWatabouDataSettings::InputPinProperties() const
{
	TArray<FPCGPinProperties> PinProperties;
	PCGEX_PIN_SPATIAL(FName("Bounds"), "Single spatial data whose bounds will be used to do the triage", Normal, {})
	return PinProperties;
}

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
bool UPCGExLoadWatabouDataSettings::IsPinUsedByNodeExecution(const UPCGPin* InPin) const
{
	if (InPin->Properties.Label == FName("Bounds")) { return InPin->EdgeCount() > 0; }
	return Super::IsPinUsedByNodeExecution(InPin);
}
#endif

#if WITH_EDITOR
void UPCGExLoadWatabouDataSettings::EDITOR_IdToPins()
{
	UPCGExWatabouData* WatabouData = DataAsset.LoadSynchronous();
	if (!WatabouData) { return; }

	IdToPins.Empty();

	for (const TPair<FPCGExFeatureIdentifier, int32>& Pair : WatabouData->Identifiers)
	{
		if (!Pair.Value) { continue; }
		IdToPins.Add(Pair.Key.Id);
	}

	MarkPackageDirty();
}
#endif

FPCGExLoadWatabouDataContext::~FPCGExLoadWatabouDataContext()
{
	if (WatabouData) { WatabouData->RemoveFromRoot(); }
}

void FPCGExLoadWatabouDataContext::ProcessCollection(const UPCGExWatabouFeaturesCollection* InCollection)
{
	for (const FPCGExWatabouFeature& Feature : InCollection->Elements)
	{
		switch (Feature.Type)
		{
		case EPCGExWatabouFeatureType::Unknown:
			continue;
			break;
		case EPCGExWatabouFeatureType::Point:
			// TODO : Merge all, not supported yet
			continue;
			break;
		case EPCGExWatabouFeatureType::MultiPoints:
			// One data per
			break;
		case EPCGExWatabouFeatureType::LineString:
			// One data per
			break;
		case EPCGExWatabouFeatureType::Polygon:
			// The tricky one. One data per for now
			// Need to allow conversion to points if has a specific id
			break;
		case EPCGExWatabouFeatureType::Collection:
			// Should never hit that
			break;
		}
		IndexTracker++;
	}

	for (const UPCGExWatabouFeaturesCollection* Collection : InCollection->SubCollections) { ProcessCollection(Collection); }
}

bool FPCGExLoadWatabouDataElement::Boot(FPCGExContext* InContext) const
{
	if (!FPCGExPointsProcessorElement::Boot(InContext)) { return false; }

	PCGEX_CONTEXT_AND_SETTINGS(LoadWatabouData)

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
		Context->SetAsyncState(PCGExCommon::State_WaitingOnAsyncWork);
		Context->ProcessCollection(Context->WatabouData->Features);
		// TODO : If features are to be quadified, they need to be processed now
		// TODO : Output values attribute set
	}

	PCGEX_ON_ASYNC_STATE_READY(PCGExCommon::State_WaitingOnAsyncWork)
	{
		Context->MainPoints->StageOutputs();
		Context->ExecuteOnNotifyActors(Settings->PostProcessFunctionNames);
	}

	return Context->TryComplete();
}

bool FPCGExLoadWatabouDataElement::CanExecuteOnlyOnMainThread(FPCGContext* Context) const
{
	return Context ? Context->CurrentPhase == EPCGExecutionPhase::PrepareData : false;
}

namespace PCGExLoadWatabouData
{
}


#undef LOCTEXT_NAMESPACE


#undef LOCTEXT_NAMESPACE
#undef PCGEX_NAMESPACE
