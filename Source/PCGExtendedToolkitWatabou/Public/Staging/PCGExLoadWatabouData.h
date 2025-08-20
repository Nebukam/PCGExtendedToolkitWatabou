// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "CoreMinimal.h"
#include "PCGExPointsProcessor.h"

#include "PCGExLoadWatabouData.generated.h"

class UPCGExWatabouData;

UCLASS(MinimalAPI, BlueprintType, ClassGroup = (Procedural), Category="PCGEx|Clusters")
class UPCGExLoadWatabouDataSettings : public UPCGExPointsProcessorSettings
{
	GENERATED_BODY()

public:
	//~Begin UPCGSettings
#if WITH_EDITOR
	PCGEX_NODE_INFOS(LoadWatabouData, "Load Watabou Data", "Load & Read watabou data and output points, paths & clusters.");
	virtual FLinearColor GetNodeTitleColor() const override { return GetDefault<UPCGExGlobalSettings>()->NodeColorCluster; }
#endif

protected:
	virtual FPCGElementPtr CreateElement() const override;
	virtual TArray<FPCGPinProperties> OutputPinProperties() const override;
	//~End UPCGSettings

	//~Begin UPCGExPointsProcessorSettings
public:
	virtual bool IsInputless() const override{return true;}
	//~End UPCGExPointsProcessorSettings

	/**  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(PCG_Overridable))
	TSoftObjectPtr<UPCGExWatabouData> DataAsset;
	
	/**  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	TArray<FName> IdToPins = { TEXT("roads"), TEXT("buildings"), TEXT("fields") };

#if WITH_EDITOR
	/** Rebuild Staging data for all collection within this project. */
	UFUNCTION(CallInEditor, Category = Tools, meta=(DisplayName="Create pins from selected data", ShortToolTip="Populate the Id-to-Pins array from data.", DisplayOrder=2))
	void EDITOR_IdToPins();
#endif
	
	/**  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(InlineEditConditionToggle))
	bool bDoPointifyPolygons = false;
	
	/** If enabled, polygons with the specified identifiers will be output as bounds instead of paths. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings, meta=(EditCondition="bDoPointifyPolygons"))
	TSet<FName> PointifyPolygons = { TEXT("buildings") };
	
	/** Specify a list of functions to be called on the target actor after dynamic mesh creation. Functions need to be parameter-less and with "CallInEditor" flag enabled. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	TArray<FName> PostProcessFunctionNames;

private:
	friend class FPCGExLoadWatabouDataElement;
};

struct FPCGExLoadWatabouDataContext final : FPCGExPointsProcessorContext
{
	friend class FPCGExLoadWatabouDataElement;

protected:
};

class FPCGExLoadWatabouDataElement final : public FPCGExPointsProcessorElement
{
protected:
	PCGEX_ELEMENT_CREATE_CONTEXT(LoadWatabouData)

	virtual bool Boot(FPCGExContext* InContext) const override;
	virtual bool ExecuteInternal(FPCGContext* InContext) const override;

	virtual bool CanExecuteOnlyOnMainThread(FPCGContext* Context) const override { return true; }
};

namespace PCGExLoadWatabouData
{
	
}
