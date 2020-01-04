// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUndoClient.h"
#include "GraphEditor.h"

/**
 * 
 */
class DIALOGUESYSTEMEDITOR_API FDialogueTreeGraphEditor : public FEditorUndoClient
{
public:
	FDialogueTreeGraphEditor();
	virtual ~FDialogueTreeGraphEditor();

	FGraphPanelSelectionSet GetSelectedNodes()const;
	virtual void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);

	// Begin of FEditorUndoClient Interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	// End of  FEditorUndoClient Interface

	void CreateCommandList();

	// Delegate for graph editor commands
	void SelectAllNodes();
	bool CanSelectAllNodes()const;
	void DeleteSelectedNodes();
	bool CanDeleteNodes();
	void CutSelectedNodes();
	bool CanCutNodes()const;
	void CopySelectedNodes();
	bool CanCopyNodes()const;
	void PasteNodes();
	void PasterNodesHere(const FVector2D& Location);
	bool CanPasteNodes()const;
	void DuplicateNodes();
	bool CanDuplicateNodes()const;

	virtual void OnClassListUpdated();
protected:
	// ��ǰͼ��
	TWeakPtr<SGraphEditor> UpdateGraphEdPtr;
	// �������
	TSharedPtr<FUICommandList> GraphEditorCommands;
	// OnClassListUpdated �ľ��
	FDelegateHandle OnClassListUpdatedDelegateHandle;
};
