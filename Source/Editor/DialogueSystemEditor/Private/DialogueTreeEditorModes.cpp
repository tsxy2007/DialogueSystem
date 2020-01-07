// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeEditorModes.h"
#include "DialogueTreeEditorTabFactories.h"
#include "Framework/Docking/TabManager.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/ApplicationMode.h"

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
			/*->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab(FBehaviorTreeEditorTabs::GraphEditorID, ETabState::ClosedTab)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.3f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.6f)
						->AddTab(FBehaviorTreeEditorTabs::GraphDetailsID, ETabState::OpenedTab)
						->AddTab(FBehaviorTreeEditorTabs::SearchID, ETabState::ClosedTab)
					)
				)
			)*/
		);

	//InDialogueTreeGraphEditor->GetToolbarBuilder()->
}

void FDialogueTreeEditorApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
}

void FDialogueTreeEditorApplicationMode::PreDeactivateMode()
{
}

void FDialogueTreeEditorApplicationMode::PostActivateMode()
{
}
