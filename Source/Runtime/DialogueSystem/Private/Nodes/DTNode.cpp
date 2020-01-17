// Fill out your copyright notice in the Description page of Project Settings.


#include "DTNode.h"

UDTNode::UDTNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

UWorld* UDTNode::GetWorld() const
{
	if (GetOuter() == nullptr)
	{
		return nullptr;
	}
	if (Cast<UPackage>(GetOuter()) != nullptr)
	{
		return Cast<UWorld>(GetOuter()->GetOuter());
	}
	return GetOuter()->GetWorld();
}

void UDTNode::InitializeNode(UDTNode* InParentNode, uint16 InExecutionIndex, uint16 InMemoryOffset, uint8 InTreeDepth)
{
	ParentNode = InParentNode;
	ExecutionIndex = InExecutionIndex;
	MemoryOffset = InMemoryOffset;
	TreeDepth = InTreeDepth;
}

void UDTNode::InitializeFromAsset(UDialogueTree& Asset)
{
	TreeAsset = &Asset;
}

void UDTNode::InitializeIndex(uint16 InLastExecuionIndex)
{
	LastExecutionIndex = InLastExecuionIndex;
}

#if WITH_EDITOR

FName UDTNode::GetNodeIconName() const
{
	return NAME_None;
}
FString UDTNode::GetNodeName() const
{
	return NodeName.Len() ? NodeName : UDialogueTreeTypes::GetShortTypeName(this);
}
#endif
