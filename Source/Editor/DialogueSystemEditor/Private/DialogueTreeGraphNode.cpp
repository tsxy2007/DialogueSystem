// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeGraphNode.h"
#include "DialogueTreeEditorTypes.h"
#include "DTNode.h"
#include "Dialogue.h"

UDialogueTreeGraphNode::UDialogueTreeGraphNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bHighlightInAbortRange0 = false;
	bHighlightInAbortRange1 = false;
	bHighlightInSearchRange0 = false;
	bHighlightInSearchRange1 = false;
	bHighlightInSearchTree = false;
	bHighlightChildNodeIndices = false;
	bRootLevel = false;
	bInjectedNode = false;
	bHasObserverError = false;
	bHasBreakpoint = false;
	bIsBreakpointEnabled = false;
	bDebuggerMarkCurrentlyActive = false;
	bDebuggerMarkPreviouslyActive = false;
	bDebuggerMarkFlashActive = false;
	bDebuggerMarkSearchSucceeded = false;
	bDebuggerMarkSearchFailed = false;
	bDebuggerMarkSearchTrigger = false;
	bDebuggerMarkSearchFailedTrigger = false;
	DebuggerSearchPathIndex = -1;
	DebuggerSearchPathSize = 0;
	DebuggerUpdateCounter = -1;
}

UDialogueEdGraph * UDialogueTreeGraphNode::GetDialogueTreeGraph()
{
	return CastChecked<UDialogueEdGraph>(GetGraph());
}

void UDialogueTreeGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UDialogueTreeEditorTypes::PinCategory_MultipleNodes, TEXT("In"));
	CreatePin(EGPD_Output, UDialogueTreeEditorTypes::PinCategory_MultipleNodes, TEXT("Out"));
}

FText UDialogueTreeGraphNode::GetTooltipText() const
{
	return FText();
}

bool UDialogueTreeGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema * Schema) const
{
	return false;
}

void UDialogueTreeGraphNode::FindDiffs(UEdGraphNode * OtherNode, FDiffResults & Results)
{
}

void UDialogueTreeGraphNode::PostEditUndo()
{
}

FText UDialogueTreeGraphNode::GetDescription() const
{
	return FText();
}

bool UDialogueTreeGraphNode::HasErrors() const
{
	return false;
}

void UDialogueTreeGraphNode::InitializeInstance()
{
	UDTNode* STNode = Cast<UDTNode>(NodeInstance);
	UDialogue* STAsset = STNode ? Cast<UDialogue>(STNode->GetOuter()) : nullptr;
	if (STNode && STAsset)
	{
		STNode->InitializeFromAsset(*STAsset);
		STNode->InitializeNode(NULL, MAX_uint16, 0, 0);
		STNode->OnNodeCreated();
	}
}

void UDialogueTreeGraphNode::OnSubNodeAdded(UDTGraphNode * SubNode)
{
	//TODO:
}

void UDialogueTreeGraphNode::OnSubNodeRemoved(UDTGraphNode * SubNode)
{
	// TODO:
}

void UDialogueTreeGraphNode::RemoveAllSubNodes()
{
	Super::RemoveAllSubNodes();
}

int32 UDialogueTreeGraphNode::FindSubNodeDropIndex(UDTGraphNode * SubNode) const
{
	const int32 SubIdx = SubNodes.IndexOfByKey(SubNode) + 1;
	//TODO:
	const int32 CombinedIdx = (SubIdx & 0xff);
	return CombinedIdx;
}

void UDialogueTreeGraphNode::InsertSubNodeAt(UDTGraphNode * SubNode, int32 DropIndex)
{
	const int32 SubIdx = (DropIndex & 0xff) - 1;
	if (SubIdx >= 0)
	{
		SubNodes.Insert(SubNode, SubIdx);
	}
	else
	{
		SubNodes.Add(SubNode);
	}

}

void UDialogueTreeGraphNode::ClearDebuggerState()
{
}

FName UDialogueTreeGraphNode::GetNameIcon() const
{
	UDTNode* BTNodeInstance = Cast<UDTNode>(NodeInstance);
	return BTNodeInstance != nullptr ? BTNodeInstance->GetNodeIconName() : FName("BTEditor.Graph.BTNode.Icon");

}

void UDialogueTreeGraphNode::CreateAddDecoratorSubMenu(FMenuBuilder & MenuBuilder, UEdGraph * Graph) const
{
}

void UDialogueTreeGraphNode::CreateAddServiceSubMenu(FMenuBuilder & MenuBuilder, UEdGraph * Graph) const
{
}

void UDialogueTreeGraphNode::AddContextMenuActionsDecorators(const FGraphNodeContextMenuBuilder & Context) const
{
}

void UDialogueTreeGraphNode::AddContextMenuActionsServices(const FGraphNodeContextMenuBuilder & Context) const
{
}
