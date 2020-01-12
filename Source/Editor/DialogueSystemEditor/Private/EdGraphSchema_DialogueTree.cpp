// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraphSchema_DialogueTree.h"
#include "DialogueGraphNode.h"

UEdGraphNode * FDTSchemaAction_NewNode::PerformAction(UEdGraph * ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode)
{
	return nullptr;
}

UEdGraphNode * FDTSchemaAction_NewNode::PerformAction(UEdGraph * ParentGraph, UEdGraphPin * FromPins, const FVector2D Location, bool bSelectNewNode)
{
	return nullptr;
}

void FDTSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector & Collector)
{
}

UEdGraphSchema_DialogueTree::UEdGraphSchema_DialogueTree(const FObjectInitializer& Obj) :Super(Obj)
{}

void UEdGraphSchema_DialogueTree::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<UDialogueGraphNode> NodeCreator(Graph);
	UDialogueGraphNode* MyNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(MyNode, FNodeMetadata::DefaultGraphNode);
}

void UEdGraphSchema_DialogueTree::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{

}

FLinearColor UEdGraphSchema_DialogueTree::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::Black;
}

bool UEdGraphSchema_DialogueTree::ShouldHidePinDefaultValue(UEdGraphPin * Pin) const
{
	return false;
}

FConnectionDrawingPolicy * UEdGraphSchema_DialogueTree::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect & InClippingRect, FSlateWindowElementList & InDrawElements, UEdGraph * InGraphObj) const
{
	return nullptr;
}

void UEdGraphSchema_DialogueTree::BreakNodeLinks(UEdGraphNode & TargetNode) const
{
}

void UEdGraphSchema_DialogueTree::BreakPinLinks(UEdGraphPin & TargetPin, bool bSendsNodeNotifcation) const
{
}

void UEdGraphSchema_DialogueTree::BreakSinglePinLink(UEdGraphPin * SourcePin, UEdGraphPin * TargetPin) const
{
}

void UEdGraphSchema_DialogueTree::GetGraphNodeContextActions(FGraphContextMenuBuilder & ContextMenuBuilder, int32 SubNodeFlags) const
{
}

//void UEdGraphSchema_DialogueTree::GetSubNodeClasses(int32 SubNodeFlags, TArray<FGraphNodeClassData>& ClassData, UClass *& GraphNodeClass) const
//{
//}

TSharedPtr<FDTSchemaAction_NewNode> UEdGraphSchema_DialogueTree::AddNewNodeAction(FGraphActionListBuilderBase & ContextMenuBuilder, const FText & MenuDesc, const FText & Tooltip)
{
	return TSharedPtr<FDTSchemaAction_NewNode>();
}
