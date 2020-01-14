
// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueEditorModes.h"
#include "DialogueTreeEditorTabs.h"
#include "DialogueTreeEditorTabFactories.h"
#include "DialogueTreeEditorToolbar.h"
#include "DialogueEditor.h"

FDialogueEditorApplicationMode::FDialogueEditorApplicationMode(TSharedPtr<class FDialogueEditor> InDialogueEditor)
	:FApplicationMode(FDialogueEditor::DialogueMode,FDialogueEditor::GetLocalizedMode)
{
	DialogueTreeEditor = InDialogueEditor;

	DialogueTreeEditorTabFactories.RegisterFactory(MakeShareable(new FDialogueTreeDetailSummoner(InDialogueEditor)));
	DialogueTreeEditorTabFactories.RegisterFactory(MakeShareable(new FDialogueTreeSearchSummoner(InDialogueEditor)));

	TabLayout = FTabManager::NewLayout("Standalone_DialogueTree_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(InDialogueEditor->GetToolbarTabId(), ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab(FDialogueTreeEditorTabs::GraphEditorID, ETabState::ClosedTab)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.3f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.6f)
						->AddTab(FDialogueTreeEditorTabs::GraphDetailsID, ETabState::OpenedTab)
						->AddTab(FDialogueTreeEditorTabs::SearchID, ETabState::ClosedTab)
					)
				)
			)
		);

	InDialogueEditor->GetToolbarBuilder()->AddModesToolbar(ToolbarExtender);
	InDialogueEditor->GetToolbarBuilder()->AddDialogueTreeToolBar(ToolbarExtender);
	//InDialogueEditor->GetToolbarBuilder()->AddDebuggerToolBar(ToolbarExtender);
}

void FDialogueEditorApplicationMode::RegisterTabFactories(TSharedPtr< class FTabManager> InTabManager)
{
	check(DialogueTreeEditor.IsValid());
	TSharedPtr<FDialogueEditor> DialogueTreeEditorPtr = DialogueTreeEditor.Pin();
	DialogueTreeEditorPtr->RegisterToolbarTab(InTabManager.ToSharedRef());
	DialogueTreeEditorPtr->PushTabFactories(DialogueTreeEditorTabFactories);
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FDialogueEditorApplicationMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
	check(DialogueTreeEditor.IsValid());
	TSharedPtr<FDialogueEditor> DialogueTreeEditorPtr = DialogueTreeEditor.Pin();
	DialogueTreeEditorPtr->SaveEditedObjectState();
}

void FDialogueEditorApplicationMode::PostActivateMode()
{
	check(DialogueTreeEditor.IsValid());
	TSharedPtr<FDialogueEditor> DialogueTreeEditorPtr = DialogueTreeEditor.Pin();
	DialogueTreeEditorPtr->RestoreBehaviorTree();

	FApplicationMode::PostActivateMode();
}
