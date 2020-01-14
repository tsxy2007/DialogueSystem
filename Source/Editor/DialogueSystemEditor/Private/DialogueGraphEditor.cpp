// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueGraphEditor.h"
#include "Editor/EditorEngine.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/GenericCommands.h"
#include "DialogueEdGraph.h"
#include "DialogueTree.h"

#define LOCTEXT_NAMESPACE "DialogueGraphEditor"
FDialogueGraphEditor::FDialogueGraphEditor()
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor!=nullptr)
	{
		Editor->RegisterForUndo(this);
	}
}

FDialogueGraphEditor::~FDialogueGraphEditor()
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor != nullptr)
	{
		Editor->UnregisterForUndo(this);
	}
}

void FDialogueGraphEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{

}

void FDialogueGraphEditor::PostUndo(bool bSuccess)
{
	if (bSuccess)
	{
		if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdptr.Pin())
		{
			CurrentGraphEditor->ClearSelectionSet();
			CurrentGraphEditor->NotifyGraphChanged();
		}
		FSlateApplication::Get().DismissAllMenus();
	}
}

void FDialogueGraphEditor::PostRedo(bool bSuccess)
{
	if (bSuccess)
	{
		if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdptr.Pin())
		{
			CurrentGraphEditor->ClearSelectionSet();
			CurrentGraphEditor->NotifyGraphChanged();
		}
		FSlateApplication::Get().DismissAllMenus();
	}
}

void FDialogueGraphEditor::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}
	GraphEditorCommands = MakeShareable(new FUICommandList);
	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FDialogueGraphEditor::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this,&FDialogueGraphEditor::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FDialogueGraphEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueGraphEditor::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FDialogueGraphEditor::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueGraphEditor::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FDialogueGraphEditor::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueGraphEditor::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FDialogueGraphEditor::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueGraphEditor::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FDialogueGraphEditor::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueGraphEditor::CanDuplicateNodes)
	);
}

void FDialogueGraphEditor::SelectAllNodes()
{
	if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdptr.Pin())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FDialogueGraphEditor::CanSelectAllNodes()
{
	return true;
}

void FDialogueGraphEditor::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdptr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction (FGenericCommands::Get().Delete->GetDescription());

	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes);NodeIt;++NodeIt)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
		{
			if (Node->CanUserDeleteNode())
			{
				Node->Modify();
				Node->DestroyNode();
			}
		}
	}

}

bool FDialogueGraphEditor::CanDeleteNodes() const
{
	const FGraphPanelSelectionSet SelectNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanUserDeleteNode())
		{
			return true;
		}
	}
	return false;
}

void FDialogueGraphEditor::DeleteSelectedDulicatableNodes()
{
}

void FDialogueGraphEditor::CutSelectedNodes()
{
}

bool FDialogueGraphEditor::CanCutNodes()
{
	return false;
}

void FDialogueGraphEditor::CopySelectedNodes()
{
}

bool FDialogueGraphEditor::CanCopyNodes() const
{
	return false;
}

void FDialogueGraphEditor::PasteNodes()
{
}

void FDialogueGraphEditor::PasteNodesHere(const FVector2D & Location)
{
}

bool FDialogueGraphEditor::CanPasteNodes() const
{
	return false;
}

void FDialogueGraphEditor::DuplicateNodes()
{
}

bool FDialogueGraphEditor::CanDuplicateNodes() const
{
	return false;
}

FGraphPanelSelectionSet FDialogueGraphEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	if (TSharedPtr<SGraphEditor> FocusedGraphEd = UpdateGraphEdptr.Pin())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}
	return CurrentSelection;
}

void FDialogueGraphEditor::OnPackageSaved(const FString& PackageFileName, UObject* Outer)
{
	UDialogueEdGraph* MyGraph = DialogueTree ? Cast<UDialogueEdGraph>(DialogueTree->BTGraph) : NULL;
	if (MyGraph)
	{
		const bool bUpdated = false;
		if (bUpdated)
		{
			MyGraph->UpdateAsset(UDialogueEdGraph::ClearDebuggerFlags);
		}
	}
}

void FDialogueGraphEditor::OnClassListUpdated()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdptr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}
	UDialogueEdGraph* MyGraph = Cast<UDialogueEdGraph>(CurrentGraphEditor->GetCurrentGraph());
	if (MyGraph)
	{
		const bool bUpdated = MyGraph->UpdateUnknownNodeClasses();
		if (bUpdated)
		{
			FGraphPanelSelectionSet CurrentSelection = GetSelectedNodes();
			OnSelectedNodesChanged(CurrentSelection);
			MyGraph->UpdateAsset();
		}
	}
}

#undef LOCTEXT_NAMESPACE