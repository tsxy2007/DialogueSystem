// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DialogueSystemEditor.h"
#include "AssetTypeActions_DialogueTree.h"
#include "DialogueTreeGraphEditor.h"

#define LOCTEXT_NAMESPACE "FDialogueSystemEditorModule"

const FName FDialogueSystemEditorModule::DialogueTreeEditorAppIdentifier(TEXT("DialogueTreeEditorApp"));

void FDialogueSystemEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	TSharedPtr<FAssetTypeActions_DialogueTree> DialogueTreeAssetTypeAction = MakeShareable(new FAssetTypeActions_DialogueTree);

	AssetToolsModule.RegisterAssetTypeActions(DialogueTreeAssetTypeAction.ToSharedRef());
	if (GIsEditor)
	{
		DialogueTreeAssetCategoryBit = AssetToolsModule.RegisterAdvancedAssetCategory(FName(TEXT("Dialogue")), LOCTEXT("DialogueAssetCategory", "Custom Dialogue"));
	}
}

void FDialogueSystemEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

TSharedRef<class FDialogueTreeGraphEditor> FDialogueSystemEditorModule::CreateDialogueSystemEditor(EToolkitMode::Type Mode, TSharedPtr<class IToolkitHost> InEditWithInLevel, UObject* InDialogue)
{
	TSharedPtr<FDialogueTreeGraphEditor> NewDialogueTreeEditor = MakeShareable(new FDialogueTreeGraphEditor);
	if (NewDialogueTreeEditor.IsValid())
	{
		NewDialogueTreeEditor->InitDialogueTreeEditor(Mode, InEditWithInLevel, InDialogue);
	}
	return NewDialogueTreeEditor.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogueSystemEditorModule, DialogueSystemEditor)