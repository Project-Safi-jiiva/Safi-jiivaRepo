// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project_Safi_jiiva : ModuleRules
{
	public Project_Safi_jiiva(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });


        PublicIncludePaths.AddRange(new string[] { "Project_Safi_jiiva/Public" });

        PrivateIncludePaths.AddRange(new string[] { "Project_Safi_jiiva/Private" });
    }
}
