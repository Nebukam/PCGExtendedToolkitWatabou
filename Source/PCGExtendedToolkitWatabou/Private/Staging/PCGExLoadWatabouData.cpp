// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#include "Staging/PCGExLoadWatabouData.h"

#include "PCGComponent.h"
#include "Paths/PCGExPaths.h"

#define LOCTEXT_NAMESPACE "PCGExLoadWatabouData"
#define PCGEX_NAMESPACE LoadWatabouData

PCGEX_INITIALIZE_ELEMENT(LoadWatabouData)

TArray<FPCGPinProperties> UPCGExLoadWatabouDataSettings::OutputPinProperties() const
{
	return Super::OutputPinProperties();
}

#if WITH_EDITOR
void UPCGExLoadWatabouDataSettings::EDITOR_IdToPins()
{
	UPCGExWatabouData* WatabouData = DataAsset.LoadSynchronous();
	if (!WatabouData){return;}

	IdToPins.Empty();
}
#endif

bool FPCGExLoadWatabouDataElement::Boot(FPCGExContext* InContext) const
{
	if (!FPCGExPointsProcessorElement::Boot(InContext)) { return false; }
	
	PCGEX_CONTEXT_AND_SETTINGS(LoadWatabouData)

	

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
		
	}

	PCGEX_ON_ASYNC_STATE_READY(PCGExCommon::State_WaitingOnAsyncWork)
	{
		Context->MainPoints->StageOutputs();
		Context->ExecuteOnNotifyActors(Settings->PostProcessFunctionNames);
	}

	return Context->TryComplete();
}

namespace PCGExLoadWatabouData
{
}


#undef LOCTEXT_NAMESPACE


#undef LOCTEXT_NAMESPACE
#undef PCGEX_NAMESPACE
