// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypeActions_DialogueTree.h"
#include "Framework/Application/SlateApplication.h"
#include "Modules/ModuleManager.h"
#include "DialogueTree.h"
#include "DialogueSystemEditor.h"

#define LOCTEXT_NAMESPACE "AssetTyipeActions"
UClass* FAssetTypeActions_DialogueTree::GetSupportedClass() const
{
	return UDialogueTree::StaticClass();
}

void FAssetTypeActions_DialogueTree::OpenAssetEditor(const TArray<UObject *>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto& Object : InObjects)
	{
		auto Dialogue = Cast<UDialogueTree>(Object);
		if (Dialogue != nullptr)
		{
			bool bFoundInAsset = false;
			bool bOpen = false;
			if (!bFoundInAsset)
			{
				FDialogueSystemEditorModule& DialogueSystemEditorModule = FDialogueSystemEditorModule::Get();
			}
		}
	}
}

uint32 FAssetTypeActions_DialogueTree::GetCategories()
{
	FDialogueSystemEditorModule& DialogueSystemEditorModule = FDialogueSystemEditorModule::Get();
	return DialogueSystemEditorModule.GetDialogueTreeAssetCategoryBit();
}

void FAssetTypeActions_DialogueTree::PerformAssetDiff(UObject* OldAsset, UObject* NewAsset, const struct FRevisionInfo& OldRevision, const struct FRevisionInfo& NewRevision) const
{

}

void FAssetTypeActions_DialogueTree::OpenInDefaults(class UDialogueTree* OldDialogueTree, class UDialogueTree* NewDialogueTree) const
{

}
#undef LOCTEXT_NAMESPACE
