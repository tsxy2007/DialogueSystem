// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeGraphEditor.h"
#include "Editor/EditorEngine.h"
#include "GenericCommands.h"

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

}

void FDialogueTreeGraphEditor::PostRedo(bool bSuccess)
{

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

}

bool FDialogueTreeGraphEditor::CanSelectAllNodes() const
{
	return true;
}

void FDialogueTreeGraphEditor::DeleteSelectedNodes()
{

}

bool FDialogueTreeGraphEditor::CanDeleteNodes()
{
	return true;
}

void FDialogueTreeGraphEditor::CutSelectedNodes()
{

}

bool FDialogueTreeGraphEditor::CanCutNodes() const
{
	return true;
}

void FDialogueTreeGraphEditor::CopySelectedNodes()
{

}

bool FDialogueTreeGraphEditor::CanCopyNodes() const
{
	return true;
}

void FDialogueTreeGraphEditor::PasteNodes()
{

}

void FDialogueTreeGraphEditor::PasterNodesHere(const FVector2D& Location)
{

}

bool FDialogueTreeGraphEditor::CanPasteNodes() const
{
	return true;
}

void FDialogueTreeGraphEditor::DuplicateNodes()
{

}

bool FDialogueTreeGraphEditor::CanDuplicateNodes() const
{
	return true;
}

void FDialogueTreeGraphEditor::OnClassListUpdated()
{

}
