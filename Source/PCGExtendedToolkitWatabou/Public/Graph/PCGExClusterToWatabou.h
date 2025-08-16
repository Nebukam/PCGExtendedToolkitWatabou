// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "CoreMinimal.h"
#include "Graph/PCGExChain.h"
#include "Graph/PCGExEdgesProcessor.h"
#include "Transform/PCGExTransform.h"

#include "PCGExClusterToWatabou.generated.h"

UCLASS(MinimalAPI, BlueprintType, ClassGroup = (Procedural), Category="PCGEx|Clusters")
class UPCGExClusterToWatabouSettings : public UPCGExEdgesProcessorSettings
{
	GENERATED_BODY()

public:
	//~Begin UPCGSettings
#if WITH_EDITOR
	PCGEX_NODE_INFOS(ClusterToWatabou, "Cluster to Zone Graph", "Create Zone Graph from clusters.");
	virtual FLinearColor GetNodeTitleColor() const override { return GetDefault<UPCGExGlobalSettings>()->NodeColorCluster; }
#endif

protected:
	virtual FPCGElementPtr CreateElement() const override;
	//~End UPCGSettings

	//~Begin UPCGExPointsProcessorSettings
public:
	virtual bool SupportsEdgeSorting() const override { return DirectionSettings.RequiresSortingRules(); }
	virtual PCGExData::EIOInit GetMainOutputInitMode() const override;
	virtual PCGExData::EIOInit GetEdgeOutputInitMode() const override;
	//~End UPCGExPointsProcessorSettings

	/** Defines the direction in which points will be ordered to form the final paths. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(PCG_Overridable))
	FPCGExEdgeDirectionSettings DirectionSettings;

	/** Comma separated tags */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(PCG_Overridable))
	FString CommaSeparatedComponentTags = TEXT("PCGExWatabou");

	/** Specify a list of functions to be called on the target actor after dynamic mesh creation. Functions need to be parameter-less and with "CallInEditor" flag enabled. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	TArray<FName> PostProcessFunctionNames;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	FPCGExAttachmentRules AttachmentRules;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	double PolygonRadius = 100;


private:
	friend class FPCGExClusterToWatabouElement;
};

struct FPCGExClusterToWatabouContext final : FPCGExEdgesProcessorContext
{
	friend class FPCGExClusterToWatabouElement;
	TArray<TSharedPtr<PCGExCluster::FNodeChain>> Chains;

	TArray<FString> ComponentTags;

protected:
	PCGEX_ELEMENT_BATCH_EDGE_DECL
};

class FPCGExClusterToWatabouElement final : public FPCGExEdgesProcessorElement
{
protected:
	PCGEX_ELEMENT_CREATE_CONTEXT(ClusterToWatabou)

	virtual bool Boot(FPCGExContext* InContext) const override;
	virtual bool ExecuteInternal(FPCGContext* InContext) const override;

	virtual bool CanExecuteOnlyOnMainThread(FPCGContext* Context) const override { return true; }
};

namespace PCGExClusterToWatabou
{
	class FProcessor;

	class FProcessor final : public PCGExClusterMT::TProcessor<FPCGExClusterToWatabouContext, UPCGExClusterToWatabouSettings>
	{
		friend class FBatch;

	protected:
		FPCGExEdgeDirectionSettings DirectionSettings;

		TWeakPtr<PCGExMT::FAsyncToken> MainThreadToken;

		TSharedPtr<PCGExCluster::FNodeChainBuilder> ChainBuilder;

	public:
		FProcessor(const TSharedRef<PCGExData::FFacade>& InVtxDataFacade, const TSharedRef<PCGExData::FFacade>& InEdgeDataFacade):
			TProcessor(InVtxDataFacade, InEdgeDataFacade)
		{
		}

		virtual bool IsTrivial() const override { return false; }

		virtual bool Process(const TSharedPtr<PCGExMT::FTaskManager>& InAsyncManager) override;
		virtual void CompleteWork() override;
		virtual void ProcessRange(const PCGExMT::FScope& Scope) override;
		virtual void OnRangeProcessingComplete() override;

		virtual void Output() override;

	};

	class FBatch final : public PCGExClusterMT::TBatch<FProcessor>
	{
		friend class FProcessor;

	protected:
		TSharedPtr<TArray<int8>> Breakpoints;
		FPCGExEdgeDirectionSettings DirectionSettings;

	public:
		FBatch(FPCGExContext* InContext, const TSharedRef<PCGExData::FPointIO>& InVtx, const TArrayView<TSharedRef<PCGExData::FPointIO>> InEdges):
			TBatch(InContext, InVtx, InEdges)
		{
			bAllowVtxDataFacadeScopedGet = true;
			DefaultVtxFilterValue = false;
		}

		virtual void RegisterBuffersDependencies(PCGExData::FFacadePreloader& FacadePreloader) override;
		virtual void OnProcessingPreparationComplete() override;
	};
}
