// Copyright 2026 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

using UnrealBuildTool;

public class PCGExElementsWatabouEditor : ModuleRules
{
	public PCGExElementsWatabouEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"UnrealEd",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"Projects",
				"PCGExCore",
				"PCGExCoreEditor",
				"PCGExElementsWatabou"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"PCG",
				"PCGExtendedToolkit",
				"PCGExtendedToolkitEditor",
				"PCGExElementsWatabou",
				"Json"
			}
		);
		
	}
}