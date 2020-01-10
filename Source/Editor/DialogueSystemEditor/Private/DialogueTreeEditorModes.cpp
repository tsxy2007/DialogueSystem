// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeEditorModes.h"
#include "DialogueTreeEditorTabFactories.h"
#include "Framework/Docking/TabManager.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "DialogueTreeEditorTabs.h"
#include "SharedPointer.h"
#include "DialogueTreeGraphEditor.h"
#include "DialogueTreeEditorToolbar.h"

FDialogueTreeEditorApplicationMode::FDialogueTreeEditorApplicationMode(TSharedPtr<class FDialogueTreeGraphEditor> InDialogueTreeGraphEditor)
	:FApplicationMode(FDialogueTreeGraphEditor::DialogueTreeMode,FDialogueTreeGraphEditor::GetLocalizeMode)
{
	DialogueTreeGraphEditor = InDialogueTreeGraphEditor;

	DialogueTreeEditorTabFactories.RegisterFactory(MakeShareable(new FDialogueDetailsSummoner(InDialogueTreeGraphEditor)));
	DialogueTreeEditorTabFactories.RegisterFactory(MakeShareable(new FDialogueTreeSearchSummoner(InDialogueTreeGraphEditor)));

	TabLayout = FTabManager::NewLayout("Standalone_DialogueTree_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(InDialogueTreeGraphEditor->GetToolbarTabId(), ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab(FDialogueTreeGraphEditorTabs::GraphEditorID, ETabState::ClosedTab)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.3f)
					->Split
					(
						FTabManager::NewStack()
						//->SetSizeCoefficient(0.6f)
						->AddTab(FDialogueTreeGraphEditorTabs::GraphDetailsID, ETabState::OpenedTab)
						->AddTab(FDialogueTreeGraphEditorTabs::SearchID, ETabState::ClosedTab)
					)
				)
			)
		);

	InDialogueTreeGraphEditor->GetToolbarBuilder()->AddModesToolbar(ToolbarExtender);
	InDialogueTreeGraphEditor->GetToolbarBuilder()->AddDialogueTreeToolbar(ToolbarExtender);
}

void FDialogueTreeEditorApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	check(DialogueTreeGraphEditor.IsValid());
	TSharedPtr<FDialogueTreeGraphEditor> DialogueTreeGraphEditorPtr = DialogueTreeGraphEditor.Pin();
	DialogueTreeGraphEditorPtr->RegisterToolbarTab(InTabManager.ToSharedRef());

	DialogueTreeGraphEditorPtr->PushTabFactories(DialogueTreeEditorTabFactories);
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FDialogueTreeEditorApplicationMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
	check(DialogueTreeGraphEditor.IsValid());
	TSharedPtr<FDialogueTreeGraphEditor> DialogueTreeGraphEditorPtr = DialogueTreeGraphEditor.Pin();
	DialogueTreeGraphEditorPtr->SaveEditedObjectState();
}

void FDialogueTreeEditorApplicationMode::PostActivateMode()
{
	check(DialogueTreeGraphEditor.IsValid());
	TSharedPtr<FDialogueTreeGraphEditor> BehaviorTreeEditorPtr = DialogueTreeGraphEditor.Pin();
	BehaviorTreeEditorPtr->RestoreDialogueTree();
	FApplicationMode::PostActivateMode();
}
