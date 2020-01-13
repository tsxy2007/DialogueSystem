// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphEditor.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"
#include "DialogueGraphEditor.h"
#include "Misc/NotifyHook.h"

class IDialogueEditor : public FWorkflowCentricApplication
{
public:
	virtual uint32 GetSelectedNodesCount() const = 0;

	virtual void InitializeDebuggerState(class FBehaviorTreeDebugger* ParentDebugger) const = 0;
	virtual UEdGraphNode* FindInjectedNode(int32 Index) const = 0;
	virtual void DoubleClickNode(class UEdGraphNode* Node) = 0;
	virtual bool GetBoundsForSelectedNodes(class FSlateRect& Rect, float Padding) const = 0;
};
/**
 * 
 */
class DIALOGUESYSTEMEDITOR_API FDialogueEditor : public IDialogueEditor, public FDialogueGraphEditor, public FNotifyHook
{
public:
	FDialogueEditor();
	virtual ~FDialogueEditor();

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	void InitDialogueEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* InObject);

	//~ Begin IToolkit Interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	//~ End IToolkit Interface

	//~ Begin IBehaviorTreeEditor Interface
	virtual uint32 GetSelectedNodesCount() const override { return SelectedNodesCount; }
	virtual void InitializeDebuggerState(class FBehaviorTreeDebugger* ParentDebugger) const override;
	virtual UEdGraphNode* FindInjectedNode(int32 Index) const override;
	virtual void DoubleClickNode(class UEdGraphNode* Node) override;
	virtual void FocusWindow(UObject* ObjectToFocusOn = NULL) override;
	virtual bool GetBoundsForSelectedNodes(class FSlateRect& Rect, float Padding) const override;
	//~ End IBehaviorTreeEditor Interface

	//~ Begin FEditorUndoClient Interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	// End of FEditorUndoClient

	//~ Begin FNotifyHook Interface
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged) override;
	// End of FNotifyHook

	static FText GetLocalizedMode(FName InMode);

	TSharedPtr<class FDialogueTreeEditorToolbar> GetToolbarBuilder() { return ToolbarBuilder; }

	bool IsDebuggerReady() const;

	UDialogue* GetDialogueTree() const;
	//virtual void OnClassListUpdated() override;

	TSharedRef<SWidget> SpawnProperties();

	TSharedRef<SWidget> SpawnSearch();

	void BindCommonCommands();
	void ExtendMenu();
	void CreateInternalWidgets();
	virtual void OnClassListUpdated() override;

	void SearchTree();
	bool CanSearchTree() const;
	bool CanAccessDialogueTreeMode() const;


	void OnAddInputPin();
	bool CanAddInputPin() const;
	void OnRemoveInputPin();
	bool CanRemoveInputPin() const;
	void OnEnableBreakpoint();
	bool CanEnableBreakpoint() const;
	void OnToggleBreakpoint();
	bool CanToggleBreakpoint() const;
	void OnDisableBreakpoint();
	bool CanDisableBreakpoint() const;
	void OnAddBreakpoint();
	bool CanAddBreakpoint() const;
	void OnRemoveBreakpoint();
	bool CanRemoveBreakpoint() const;

	bool InEditingMode(bool bGraphIsEditable) const;
	FGraphAppearanceInfo GetGraphAppearance() const;

	//Delegate
	void OnNodeDoubleClicked(class UEdGraphNode* Node);	
	void OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor);
	void OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged);

	void CreateNewTask() const;
	bool CanCreateNewTask() const;
	bool IsNewTaskButtonVisible() const;
	bool IsNewTaskComboVisible() const;
	TSharedRef<SWidget> HandleCreateNewTaskMenu() const;
	void HandleNewNodeClassPicked(UClass* InClass) const;

	bool IsPropertyEditable() const;
	void OnPackageSaved(const FString& PackageFileName, UObject* Outer);
	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);


	void RegisterToolbarTab(const TSharedRef<class FTabManager>& TabManager);
	void RestoreBehaviorTree();

	void SaveEditedObjectState();
protected:
	/** Called when "Save" is clicked for this asset */
	virtual void SaveAsset_Execute() override;

private:
	TSharedRef<class SGraphEditor> CreateGraphEditorWidget(UEdGraph* InGraph);
	TSharedPtr<class IDetailsView> DetailsView;

	TSharedPtr<class FDialogueTreeEditorToolbar> ToolbarBuilder;
	TSharedPtr<class FDocumentTracker> DocumentManager;
	TWeakPtr< class FDocumentTabFactory> GraphEditorTabFactoryPtr;
	class UDialogue* Dialogue;

	/** Called when the selection changes in the GraphEditor */
	virtual void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection) override;
private:
	uint32 SelectedNodesCount;
public:
	static const FName DialogueMode; 
};
