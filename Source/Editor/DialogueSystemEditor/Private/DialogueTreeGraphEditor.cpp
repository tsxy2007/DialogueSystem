// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeGraphEditor.h"
#include "Editor/EditorEngine.h"
#include "GenericCommands.h"
#include "SlateApplication.h"
#include "ScopedTransaction.h"
#include "GraphEditor.h"
#include "HAL/PlatformApplicationMisc.h"
#include "EdGraphUtilities.h"

#define LOCTEXT_NAMESPACE "DialogueGraph"
FDialogueTreeGraphEditor::FDialogueTreeGraphEditor()
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor != nullptr)
	{
		Editor->RegisterForUndo(this);
	}
	//OnClassListUpdatedDelegateHandle = FGraphNodeClassHelper::OnPackageListUpdated.
}

FDialogueTreeGraphEditor::~FDialogueTreeGraphEditor()
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor)
	{
		Editor->UnregisterForUndo(this);
	}
}

FGraphPanelSelectionSet FDialogueTreeGraphEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	if (TSharedPtr<SGraphEditor> FocusedGraphEd = UpdateGraphEdPtr.Pin())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}
	return CurrentSelection;
}

void FDialogueTreeGraphEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{

}

void FDialogueTreeGraphEditor::PostUndo(bool bSuccess)
{
	if (bSuccess)
	{
		if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin())
		{
			CurrentGraphEditor->ClearSelectionSet();
			CurrentGraphEditor->NotifyGraphChanged();
		}
		FSlateApplication::Get().DismissAllMenus();
	}
}

void FDialogueTreeGraphEditor::PostRedo(bool bSuccess)
{
	if (bSuccess)
	{
		if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin())
		{
			CurrentGraphEditor->ClearSelectionSet();
			CurrentGraphEditor->NotifyGraphChanged();
		}
		FSlateApplication::Get().DismissAllMenus();
	}
}

void FDialogueTreeGraphEditor::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}
	GraphEditorCommands = MakeShareable(new FUICommandList);

	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CanSelectAllNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CanDeleteNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CanCopyNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CanCutNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CanPasteNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CanDuplicateNodes));
}

void FDialogueTreeGraphEditor::SelectAllNodes()
{
	if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FDialogueTreeGraphEditor::CanSelectAllNodes() const
{
	return true;
}

void FDialogueTreeGraphEditor::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}
	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());
	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes);NodeIt;++NodeIt)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
		{
			Node->Modify();
			Node->DestroyNode();
		}
	}
}

bool FDialogueTreeGraphEditor::CanDeleteNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes);SelectedIter;++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node&&Node->CanUserDeleteNode())
		{
			return true;
		}
	}
	return false;
}

void FDialogueTreeGraphEditor::DeleteSelectedDuplicatableNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}
	const FGraphPanelSelectionSet OldSelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectIter(OldSelectedNodes);SelectIter;++SelectIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectIter);
		if (Node&& Node->CanDuplicateNode())
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
	DeleteSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();
	for (FGraphPanelSelectionSet::TConstIterator SelectIter(OldSelectedNodes); SelectIter; ++SelectIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectIter))
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FDialogueTreeGraphEditor::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FDialogueTreeGraphEditor::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FDialogueTreeGraphEditor::CopySelectedNodes()
{
	FGraphPanelSelectionSet SelectNodes = GetSelectedNodes();
	
}

bool FDialogueTreeGraphEditor::CanCopyNodes() const
{
	const FGraphPanelSelectionSet SelectdNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectdNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			return true;
		}
	}
	return false;
}

void FDialogueTreeGraphEditor::PasteNodes()
{

}

void FDialogueTreeGraphEditor::PasterNodesHere(const FVector2D& Location)
{

}

bool FDialogueTreeGraphEditor::CanPasteNodes() const
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return false;
	}
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
	return FEdGraphUtilities::CanImportNodesFromText(CurrentGraphEditor->GetCurrentGraph(),ClipboardContent);
}

void FDialogueTreeGraphEditor::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FDialogueTreeGraphEditor::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FDialogueTreeGraphEditor::OnClassListUpdated()
{

}
#undef LOCTEXT_NAMESPACE