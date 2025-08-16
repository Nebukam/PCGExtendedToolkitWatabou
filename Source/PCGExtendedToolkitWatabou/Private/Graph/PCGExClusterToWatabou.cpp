// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#include "Graph/PCGExClusterToWatabou.h"

#include "PCGComponent.h"
#include "Paths/PCGExPaths.h"

#define LOCTEXT_NAMESPACE "PCGExClusterToWatabou"
#define PCGEX_NAMESPACE ClusterToWatabou

PCGExData::EIOInit UPCGExClusterToWatabouSettings::GetEdgeOutputInitMode() const { return PCGExData::EIOInit::Forward; }
PCGExData::EIOInit UPCGExClusterToWatabouSettings::GetMainOutputInitMode() const { return PCGExData::EIOInit::Forward; }

PCGEX_INITIALIZE_ELEMENT(ClusterToWatabou)
PCGEX_ELEMENT_BATCH_EDGE_IMPL_ADV(ClusterToWatabou)

bool FPCGExClusterToWatabouElement::Boot(FPCGExContext* InContext) const
{
	PCGEX_CONTEXT_AND_SETTINGS(ClusterToWatabou)

	if (!FPCGExEdgesProcessorElement::Boot(InContext)) { return false; }

	if (const UPCGComponent* PCGComponent = InContext->GetComponent())
	{
		if (PCGComponent->GenerationTrigger == EPCGComponentGenerationTrigger::GenerateAtRuntime)
		{
			PCGE_LOG_C(Error, GraphAndLog, Context, FTEXT("Zone Graph PCG Nodes should not be used in runtime-generated PCG components."));
			return false;
		}
	}

	return true;
}

bool FPCGExClusterToWatabouElement::ExecuteInternal(
	FPCGContext* InContext) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FPCGExClusterToWatabouElement::Execute);

	PCGEX_CONTEXT_AND_SETTINGS(ClusterToWatabou)
	PCGEX_EXECUTION_CHECK
	PCGEX_ON_INITIAL_EXECUTION
	{
		if (!Context->StartProcessingClusters(
			[](const TSharedPtr<PCGExData::FPointIOTaggedEntries>& Entries) { return true; },
			[&](const TSharedPtr<PCGExClusterMT::IBatch>& NewBatch)
			{
				NewBatch->bRequiresWriteStep = true; // Not really but we need the step
				NewBatch->VtxFilterFactories = &Context->FilterFactories;
			}))
		{
			return Context->CancelExecution(TEXT("Could not build any clusters."));
		}
	}

	PCGEX_CLUSTER_BATCH_PROCESSING(PCGExCommon::State_Done)

	Context->OutputBatches();
	Context->OutputPointsAndEdges();
	Context->ExecuteOnNotifyActors(Settings->PostProcessFunctionNames);

	return Context->TryComplete();
}

namespace PCGExClusterToWatabou
{
	bool FProcessor::Process(const TSharedPtr<PCGExMT::FTaskManager>& InAsyncManager)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(PCGExClusterToWatabou::Process);

		if (!IProcessor::Process(InAsyncManager)) { return false; }

		if (!DirectionSettings.InitFromParent(ExecutionContext, GetParentBatch<FBatch>()->DirectionSettings, EdgeDataFacade)) { return false; }

		return true;
	}

	void FProcessor::CompleteWork()
	{
	}

	void FProcessor::ProcessRange(const PCGExMT::FScope& Scope)
	{
		PCGEX_SCOPE_LOOP(Index)
		{
		}
	}

	void FProcessor::OnRangeProcessingComplete()
	{
	}

	void FProcessor::Output()
	{
		if (!this->bIsProcessorValid) { return; }

		TRACE_CPUPROFILER_EVENT_SCOPE(UPCGExClusterToWatabou::FProcessor::Output);
	}

	void FBatch::RegisterBuffersDependencies(PCGExData::FFacadePreloader& FacadePreloader)
	{
		TBatch<FProcessor>::RegisterBuffersDependencies(FacadePreloader);
		PCGEX_TYPED_CONTEXT_AND_SETTINGS(ClusterToWatabou)
		DirectionSettings.RegisterBuffersDependencies(ExecutionContext, FacadePreloader);
	}

	void FBatch::OnProcessingPreparationComplete()
	{
		PCGEX_TYPED_CONTEXT_AND_SETTINGS(ClusterToWatabou)

		DirectionSettings = Settings->DirectionSettings;
		if (!DirectionSettings.Init(Context, VtxDataFacade, Context->GetEdgeSortingRules()))
		{
			PCGE_LOG_C(Warning, GraphAndLog, Context, FTEXT("Some vtx are missing the specified Direction attribute."));
			return;
		}

		TBatch<FProcessor>::OnProcessingPreparationComplete();
	}
}


#undef LOCTEXT_NAMESPACE


#undef LOCTEXT_NAMESPACE
#undef PCGEX_NAMESPACE
