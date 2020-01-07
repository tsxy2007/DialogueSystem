// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUndoClient.h"
#include "GraphEditor.h"
#include "WorkflowCentricApplication.h"
#include "NotifyHook.h"
#include "Color.h"
#include "WorkflowTabFactory.h"
#include "WorkflowTabManager.h"

class IToolkitHost;
class FDocumentTracker;
class UDialogueTree;
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

	// ~Begin FNotifyHook Interface
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged) override;
	//~End FNotifyHook Interface

	// Begin FEditorUndoClient
	FGraphPanelSelectionSet GetSelectedNodes()const;
	virtual void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);

	// Delegates
	void OnNodeDoubleClicked(class UEdGraphNode* Node);
	void OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor);
	void OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* PropertyThatChanged);

	void OnAddInputPin();
	bool CanAddInputPin() const;
	void OnRemoveInputPin();
	bool CanRemoveInputPin()const;

	void SearchTree();
	bool CanSearchTree()const;

	void JumpToNode(const UEdGraphNode* Node);
	bool IsPropertyEditable()const;
	void OnPackageSaved(const FString& PackageFileName, UObject* Outer);
	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);
	
	void UpdateToolBar();
	FGraphAppearanceInfo GetGraphAppearance() const;
	bool IsEditingMode(bool bGraphIsEditable)const;


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


	bool InEditingMode(bool bGraphIsEditable)const;

	static FText GetLocalizeMode(FName InMode);

	// get dialogue tree
	UDialogueTree* GetDialogueTree() const;

	// spawns the tab with the update graph inside
	TSharedRef<SWidget> SpawnProperties();

	TSharedRef<SWidget> SpawnSearch();

private:
	TSharedRef<class SGraphEditor> CreateGraphEditorWidget(UEdGraph* InGraph);

	void CreateInternalWidgets();

	void ExtendMenu();

	void BindCommonCommands();

public:
	static const FName DialogueTreeMode;
protected:
	// 当前图表
	TWeakPtr<SGraphEditor> UpdateGraphEdPtr;
	// 命令队列
	TSharedPtr<FUICommandList> GraphEditorCommands;
	// OnClassListUpdated 的句柄
	FDelegateHandle OnClassListUpdatedDelegateHandle;

private:
	TSharedPtr<FDocumentTracker> DocumentManager;

	TWeakPtr<FDocumentTabFactory> GraphEditorTabFactoryPtr;

	UDialogueTree* DialogueTree;

	TSharedPtr<class IDetailsView> DetailsView;


	uint32 SelectdNodeCount;

	TSharedPtr<class SFindInBT> FindReuslts;
};
