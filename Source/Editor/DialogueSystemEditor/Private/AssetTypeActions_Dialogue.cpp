// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypeActions_Dialogue.h"
#include "Framework/Application/SlateApplication.h"
#include "Modules/ModuleManager.h"
#include "DialogueSystem.h"
#include "Dialogue.h"
#include "DialogueSystemEditor.h"
#include "DialogueEditor.h"

#define  LOCTEXT_NAMESPACE "AssetTypeActions"
UClass* FAssetTypeActions_Dialogue::GetSupportedClass() const
{
	return UDialogue::StaticClass();
}

void FAssetTypeActions_Dialogue::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto Object : InObjects)
	{
		auto Dialogue = Cast<UDialogue>(Object);
		if (Dialogue != nullptr)
		{
			bool  bFoundInAsset = false;
			bool bOPen= false;
			{
				FDialogueEditor* ExistingInstance = static_cast<FDialogueEditor*>(FAssetEditorManager::Get().FindEditorForAsset(Dialogue,bOPen));
				if (ExistingInstance != nullptr && ExistingInstance->GetDialogueTree() == nullptr)
				{
					ExistingInstance->InitDialogueEditor(Mode,EditWithinLevelEditor,Dialogue);
					bFoundInAsset = true;
				}
			}
			if (!bFoundInAsset)
			{
				FDialogueSystemEditorModule& DialogueSystemEditorModule = FModuleManager::LoadModuleChecked<FDialogueSystemEditorModule>("DialogueSystemEditor").Get();
				TSharedRef<FDialogueEditor> ExistingInstance = DialogueSystemEditorModule.CreateDialogueSystemEditor(Mode,EditWithinLevelEditor,Dialogue);
			}
		}
	}
}

uint32 FAssetTypeActions_Dialogue::GetCategories()
{
	FDialogueSystemEditorModule& DialogueSystemEditorModule = FModuleManager::LoadModuleChecked<FDialogueSystemEditorModule>("DialogueSystemEditor").Get();
	return DialogueSystemEditorModule.GetDialogueAssetCategoryBit();
}

void FAssetTypeActions_Dialogue::PerformAssetDiff(UObject* Asset1, UObject* Asset2, const struct FRevisionInfo& OldRevision, const struct FRevisionInfo& NewRevision) const
{

}

void FAssetTypeActions_Dialogue::OpenInDefaults(class UDialogue* OldBehaviorTree, class UDialogue* NewBehaviorTree) const
{

}
#undef LOCTEXT_NAMESPACE