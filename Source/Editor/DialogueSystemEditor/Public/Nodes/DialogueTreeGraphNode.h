// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DTGraphNode.h"
#include "DialogueTreeGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEMEDITOR_API UDialogueTreeGraphNode : public UDTGraphNode
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY()
	TArray<UDialogueTreeGraphNode*> Services;
public:

	//~begin UEdgraphNode Interface
	virtual class UDialogueEdGraph* GetDialogueTreeGraph();
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void FindDiffs(class UEdGraphNode* OtherNode, struct FDiffResults& Results) override;
	//~end UEdgraphNode Interface

#if WITH_EDITOR
	virtual void PostEditUndo() override;
#endif

	virtual FText GetDescription() const override;
	virtual bool HasErrors() const override;
	virtual void InitializeInstance() override;
	virtual void OnSubNodeAdded(UDTGraphNode* SubNode) override;
	virtual void OnSubNodeRemoved(UDTGraphNode* SubNode) override;
	virtual void RemoveAllSubNodes() override;
	virtual int32 FindSubNodeDropIndex(UDTGraphNode* SubNode) const override;
	virtual void InsertSubNodeAt(UDTGraphNode* SubNode, int32 DropIndex) override;

	virtual bool CanPlaceBreakpoints()const { return false; }

	void ClearDebuggerState();

	virtual FName GetNameIcon() const;

public:
	/** if set, this node was injected from subtree and shouldn't be edited */
	UPROPERTY()
		uint32 bInjectedNode : 1;

	/** if set, this node is root of tree or sub node of it */
	uint32 bRootLevel : 1;

	/** if set, observer setting is invalid (injected nodes only) */
	uint32 bHasObserverError : 1;

	/** highlighting nodes in abort range for more clarity when setting up decorators */
	uint32 bHighlightInAbortRange0 : 1;

	/** highlighting nodes in abort range for more clarity when setting up decorators */
	uint32 bHighlightInAbortRange1 : 1;

	/** highlighting connections in search range for more clarity when setting up decorators */
	uint32 bHighlightInSearchRange0 : 1;

	/** highlighting connections in search range for more clarity when setting up decorators */
	uint32 bHighlightInSearchRange1 : 1;

	/** highlighting nodes during quick find */
	uint32 bHighlightInSearchTree : 1;

	/** highlight other child node indexes when hovering over a child */
	uint32 bHighlightChildNodeIndices : 1;

	/** debugger flag: breakpoint exists */
	uint32 bHasBreakpoint : 1;

	/** debugger flag: breakpoint is enabled */
	uint32 bIsBreakpointEnabled : 1;

	bool IsVisited = false;
protected:

	/** creates add decorator... submenu */
	void CreateAddDecoratorSubMenu(class FMenuBuilder& MenuBuilder, UEdGraph* Graph) const;

	/** creates add service... submenu */
	void CreateAddServiceSubMenu(class FMenuBuilder& MenuBuilder, UEdGraph* Graph) const;

	/** add right click menu to create subnodes: Decorators */
	void AddContextMenuActionsDecorators(const FGraphNodeContextMenuBuilder& Context) const;

	/** add right click menu to create subnodes: Services */
	void AddContextMenuActionsServices(const FGraphNodeContextMenuBuilder& Context) const;
};
