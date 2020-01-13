// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUndoClient.h"
#include "GraphEditor.h"

class UDialogue;
/**
 * 
 */
class DIALOGUESYSTEMEDITOR_API FDialogueGraphEditor : public FEditorUndoClient
{
public:
	FDialogueGraphEditor();
	virtual ~FDialogueGraphEditor();
	virtual void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

	void CreateCommandList();

	void SelectAllNodes();
	bool CanSelectAllNodes();
	void DeleteSelectedNodes();
	bool CanDeleteNodes()const;
	void DeleteSelectedDulicatableNodes();
	void CutSelectedNodes();
	bool CanCutNodes();
	void CopySelectedNodes();
	bool CanCopyNodes() const;
	void PasteNodes();
	void PasteNodesHere(const FVector2D& Location);
	bool CanPasteNodes()const;
	void DuplicateNodes();
	bool CanDuplicateNodes()const;

	FGraphPanelSelectionSet GetSelectedNodes() const;

	void OnPackageSaved(const FString& PackageFileName, UObject* Outer);

	virtual void OnClassListUpdated();

private:

	UDialogue* DialogueTree;

	FDelegateHandle OnPackageSaveDelegateHandle;
protected:
	TWeakPtr<SGraphEditor> UpdateGraphEdptr;
	TSharedPtr<FUICommandList> GraphEditorCommands;
};
