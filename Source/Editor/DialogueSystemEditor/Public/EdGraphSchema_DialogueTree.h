// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_DialogueTree.generated.h"

class FSlateRect;
class UEdGraph;

USTRUCT()
struct  DIALOGUESYSTEMEDITOR_API FDTSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();
public:
	class UDTGraphNode*  NodeTemplate;
	FDTSchemaAction_NewNode()
		:FEdGraphSchemaAction()
		, NodeTemplate(nullptr)
	{

	}

	FDTSchemaAction_NewNode(FText InNodeCategory,FText InMenuDesc,FText InToolTip,const int32 InGrouping)
		:FEdGraphSchemaAction(MoveTemp(InNodeCategory),MoveTemp(InMenuDesc),MoveTemp(InToolTip),InGrouping)
		,NodeTemplate(nullptr)
	{}

	//~Begin FEdgraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin *>& FromPins, const FVector2D Location, bool bSelectNewNode  = true ) override;
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin * FromPins, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~End FEdgraphSchemaAction Interface

	template<typename NodeType>
	static NodeType* SpawnNodeFromTemplate(class UEdGraph* ParentGraph, NodeType* InTemplateNode, const FVector2D Location)
	{
		FDTSchemaAction_NewNode Action;
		Action->NodeTemplate = InTemplateNode;
		return Cast<NodeType>(Action->PerformAction(ParentGraph, nullptr, Location));
	}
};

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEMEDITOR_API UEdGraphSchema_DialogueTree : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()
	
public:
	//~ Begin EdGraphSchema Interface
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
	virtual void BreakNodeLinks(UEdGraphNode& TargetNode) const override;
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;
	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;
	//~ End EdGraphSchema Interface
	virtual void GetGraphNodeContextActions(FGraphContextMenuBuilder& ContextMenuBuilder, int32 SubNodeFlags) const;
	//virtual void GetSubNodeClasses(int32 SubNodeFlags, TArray<FGraphNodeClassData>& ClassData, UClass*& GraphNodeClass) const;

protected:
	static TSharedPtr<FDTSchemaAction_NewNode> AddNewNodeAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& MenuDesc, const FText& Tooltip);
};
