// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DialogueSystemEditor : ModuleRules
{
	public DialogueSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PrivatePCHHeaderFile = "Editor/DialogueSystemEditor/Public/DialogueSystemEditor.h";
        PrivateIncludePaths.AddRange(
            new string[] {
                "Editor/DialogueSystemEditor/Private",
            }
        );

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
                 "Editor/DialogueSystemEditor/Public",
            }
			);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "InputCore",
                "UnrealEd",
                "LevelEditor",
                "DialogueSystem",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
