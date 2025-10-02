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
	virtual FLinearColor GetNodeTitleColor() const override { return GetDefault<UPCGExGlobalSettings>()->WantsColor(GetDefault<UPCGExGlobalSettings>()->ColorMiscWrite); }
#endif

protected:
	virtual FPCGElementPtr CreateElement() const override;
	virtual TArray<FPCGPinProperties> OutputPinProperties() const override;

	//~End UPCGSettings

	//~Begin UPCGExPointsProcessorSettings
public:
	virtual bool IsInputless() const override { return true; }
	//~End UPCGExPointsProcessorSettings

	/**  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(PCG_Overridable))
	TSoftObjectPtr<UPCGExWatabouData> DataAsset;

	/**  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(PCG_Overridable))
	FTransform Transform;

	/** Scale applied on top of input transform. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(PCG_Overridable))
	double ScaleFactor = 1;
	
	/** Scale applied to numeric data values. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(PCG_Overridable))
	double DataScaleFactor = 1;
	
	/**  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	TArray<FName> IdToPins;
	
#if WITH_EDITOR
	/** Rebuild Staging data for all collection within this project. */
	UFUNCTION(CallInEditor, Category = Tools, meta=(DisplayName="Reset pins from selected data", ShortToolTip="Populate the Id-to-Pins array from data.", DisplayOrder=2))
	void EDITOR_IdToPins();
	
	/** Rebuild Staging data for all collection within this project. */
	UFUNCTION(CallInEditor, Category = Tools, meta=(DisplayName="Append pins from selected data", ShortToolTip="Add to the Id-to-Pins array any Id that's missing, but does not remove existing ones.", DisplayOrder=2))
	void EDITOR_AppendIdToPins();
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

	/**  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(InlineEditConditionToggle))
	bool bDoPointifyLines = false;
	
	/** If enabled, polygons with the specified identifiers will be output as bounds instead of paths. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(EditCondition="bDoPointifyLines"))
	TSet<FName> PointifyLines = {TEXT("planks")};

	/** Tag to apply to path-like data. Cannot necessarily be inferred from id alone, so this is a failsafe. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Tagging & Forwarding")
	FString PathlikeTag = TEXT("path");

	/** A list of Ids to omit from processing. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(EditCondition="bDoPointifyLines"))
	TSet<FName> SkipIds;
	
private:
	friend class FPCGExLoadWatabouDataElement;
};

struct FPCGExLoadWatabouDataContext final : FPCGExPointsProcessorContext
{
	friend class FPCGExLoadWatabouDataElement;

	~FPCGExLoadWatabouDataContext();

	UPCGExWatabouData* WatabouData = nullptr;
	TArray<FName> IdAsPins;
	TSet<FName> SkipIds;
	FTransform MainTransform;
	double DataScaleFactor = 1;

	void WriteDetails(UPCGData* InData, const int32 ElementIndex, const UPCGExWatabouFeaturesCollection* Collection)const;
	
protected:
	TSet<FName> PointifyPolygons;
	TSet<FName> PointifyLines;
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

	class FBuildPointified final : public FBuildFeature
	{
	public:
		PCGEX_ASYNC_TASK_NAME(FBuildPointified)

		TArray<int32> Elements;
		
		FBuildPointified() = default;
		virtual void ExecuteTask(const TSharedPtr<PCGExMT::FTaskManager>& AsyncManager) override;
	};
}
