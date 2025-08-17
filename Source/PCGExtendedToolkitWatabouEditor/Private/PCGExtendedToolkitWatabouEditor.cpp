// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#define LOCTEXT_NAMESPACE "FPCGExtendedToolkitWatabouEditorModule"

#include "PCGExtendedToolkitWatabouEditor.h"

#include "Styling/SlateStyle.h"
#include "Interfaces/IPluginManager.h"
#include "AssetRegistry/AssetData.h"
#include "Styling/SlateStyleRegistry.h"


#define PCGEX_ADD_ICON(_NAME) \
Style->Set("ClassIcon." # _NAME, new FSlateImageBrush(Style->RootToContentDir(TEXT( "" #_NAME), TEXT(".png")), SizeIcon));\
Style->Set("ClassThumbnail." # _NAME, new FSlateImageBrush(Style->RootToContentDir(TEXT( "" #_NAME), TEXT(".png")), SizeThumbnail));

void FPCGExtendedToolkitWatabouEditorModule::StartupModule()
{
	// I know this is cursed
	FSlateStyleSet& AppStyle = const_cast<FSlateStyleSet&>(static_cast<const FSlateStyleSet&>(FAppStyle::Get()));

	Style = MakeShared<FSlateStyleSet>("PCGExWatabouStyleSet");
	Style->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("PCGExtendedToolkitWatabou"))->GetBaseDir() / TEXT("Resources") / TEXT("Icons"));

	const FVector2D SizeIcon = FVector2D(16.0f, 16.0f);
	const FVector2D SizeThumbnail = FVector2D(128.0f, 128.0f);

	PCGEX_ADD_ICON(PCGExWatabouData)

	FSlateStyleRegistry::RegisterSlateStyle(*Style.Get());
}

#undef PCGEX_ADD_ICON

void FPCGExtendedToolkitWatabouEditorModule::ShutdownModule()
{
	//FSlateStyleRegistry::UnRegisterSlateStyle(Style->GetStyleSetName());
	//Style.Reset();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPCGExtendedToolkitWatabouEditorModule, PCGExtendedToolkitWatabouEditor)
