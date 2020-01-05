// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUndoClient.h"
#include "GraphEditor.h"
#include "WorkflowCentricApplication.h"
#include "NotifyHook.h"
#include "Color.h"

class IToolkitHost;
class FDocumentTracker;
/**
 * 
 */
class DIALOGUESYSTEMEDITOR_API FDialogueTreeGraphEditor : public FEditorUndoClient, public FWorkflowCentricApplication, public FNotifyHook
{
public:
	FDialogueTreeGraphEditor();
	virtual ~FDialogueTreeGraphEditor();

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	void InitDialogueTreeEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* InObject);

	// ~ Begin IToolit Interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	// ~End IToolit Interface


	// Begin FEditorUndoClient
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
	bool CanDeleteNodes() const;
	void DeleteSelectedDuplicatableNodes();
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

	// End FEditorUndoClient
protected:
	// 当前图表
	TWeakPtr<SGraphEditor> UpdateGraphEdPtr;
	// 命令队列
	TSharedPtr<FUICommandList> GraphEditorCommands;
	// OnClassListUpdated 的句柄
	FDelegateHandle OnClassListUpdatedDelegateHandle;

private:
	TSharedPtr<FDocumentTracker> DocumentManager;

	UDialogueTree* DialogueTree;
};
