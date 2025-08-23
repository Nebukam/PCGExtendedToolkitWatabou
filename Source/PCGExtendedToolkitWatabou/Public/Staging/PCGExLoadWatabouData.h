// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "CoreMinimal.h"
#include "PCGExPointsProcessor.h"

#include "PCGExLoadWatabouData.generated.h"

namespace PCGExLoadWatabouData
{
	class FBuildFeature;
}

struct FPCGExWatabouFeature;
class UPCGExWatabouFeaturesCollection;
class UPCGExWatabouData;

UCLASS(MinimalAPI, BlueprintType, ClassGroup = (Procedural), Category="PCGEx|Clusters")
class UPCGExLoadWatabouDataSettings : public UPCGExPointsProcessorSettings
{
	GENERATED_BODY()

public:
	//~Begin UPCGSettings
#if WITH_EDITOR
	PCGEX_NODE_INFOS(LoadWatabouData, "Load Watabou Data", "Load & Read Watabou data and output points & paths.");
	virtual EPCGSettingsType GetType() const override { return EPCGSettingsType::InputOutput; }
	virtual FLinearColor GetNodeTitleColor() const override { return GetDefault<UPCGExGlobalSettings>()->WantsColor(GetDefault<UPCGExGlobalSettings>()->NodeColorPrimitives); }
#endif

protected:
	virtual FPCGElementPtr CreateElement() const override;
	virtual TArray<FPCGPinProperties> InputPinProperties() const override;
	virtual TArray<FPCGPinProperties> OutputPinProperties() const override;

#if WITH_EDITOR
	virtual bool IsPinUsedByNodeExecution(const UPCGPin* InPin) const override;
#endif

	//~End UPCGSettings

	//~Begin UPCGExPointsProcessorSettings
public:
	virtual bool IsInputless() const override { return true; }
	//~End UPCGExPointsProcessorSettings

	/**  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(PCG_Overridable))
	TSoftObjectPtr<UPCGExWatabouData> DataAsset;

	/**  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	TArray<FName> IdToPins;

#if WITH_EDITOR
	/** Rebuild Staging data for all collection within this project. */
	UFUNCTION(CallInEditor, Category = Tools, meta=(DisplayName="Create pins from selected data", ShortToolTip="Populate the Id-to-Pins array from data.", DisplayOrder=2))
	void EDITOR_IdToPins();
#endif

	/**  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	double GlobalScale = 1;

	/**  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(InlineEditConditionToggle))
	bool bDoPointifyPolygons = false;

	/** If enabled, polygons with the specified identifiers will be output as bounds instead of paths. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(EditCondition="bDoPointifyPolygons"))
	TSet<FName> PointifyPolygons = {TEXT("buildings")};

	/** Tag to apply to path-like data. Cannot necessarily be inferred from id alone, so this is a failsafe. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Tagging & Forwarding")
	FString PathlikeTag = TEXT("path");
	
private:
	friend class FPCGExLoadWatabouDataElement;
};

struct FPCGExLoadWatabouDataContext final : FPCGExPointsProcessorContext
{
	friend class FPCGExLoadWatabouDataElement;

	~FPCGExLoadWatabouDataContext();

	UPCGExWatabouData* WatabouData = nullptr;
	
protected:
	int32 IndexTracker = 0;
	TArray<TSharedPtr<PCGExLoadWatabouData::FBuildFeature>> Tasklist;
	void ProcessCollection(const UPCGExWatabouFeaturesCollection* InCollection);
};

class FPCGExLoadWatabouDataElement final : public FPCGExPointsProcessorElement
{
protected:
	PCGEX_ELEMENT_CREATE_CONTEXT(LoadWatabouData)

	virtual bool Boot(FPCGExContext* InContext) const override;
	virtual bool ExecuteInternal(FPCGContext* InContext) const override;

	virtual bool CanExecuteOnlyOnMainThread(FPCGContext* Context) const override;
};

namespace PCGExLoadWatabouData
{
	class FBuildFeature : public PCGExMT::FTask
	{
	public:
		FBuildFeature() = default;

		const UPCGExWatabouFeaturesCollection* ParentCollection = nullptr;
		int32 ElementIndex = -1;
		TSharedPtr<PCGExData::FPointIO> PointIO;
	};

	class FBuildMultiPoints final : public FBuildFeature
	{
	public:
		PCGEX_ASYNC_TASK_NAME(FBuildMultiPoints)

		FBuildMultiPoints() = default;
		virtual void ExecuteTask(const TSharedPtr<PCGExMT::FTaskManager>& AsyncManager) override;
	};

	class FBuildLineString final : public FBuildFeature
	{
	public:
		PCGEX_ASYNC_TASK_NAME(FBuildLineString)

		FBuildLineString() = default;
		virtual void ExecuteTask(const TSharedPtr<PCGExMT::FTaskManager>& AsyncManager) override;
	};

	class FBuildPolygon final : public FBuildFeature
	{
	public:
		PCGEX_ASYNC_TASK_NAME(FBuildPolygon)

		FBuildPolygon() = default;
		virtual void ExecuteTask(const TSharedPtr<PCGExMT::FTaskManager>& AsyncManager) override;
	};
}
