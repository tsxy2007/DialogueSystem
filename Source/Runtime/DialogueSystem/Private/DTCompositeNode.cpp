// Fill out your copyright notice in the Description page of Project Settings.


#include "DTCompositeNode.h"

UDTCompositeNode::UDTCompositeNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bApplyDecoratorScope = true;
}

UDTCompositeNode::~UDTCompositeNode()
{

}

void UDTCompositeNode::InitializeComposite(uint16 InLastExecuionIndex)
{
	LastExecutionIndex = InLastExecuionIndex;
}

int32 UDTCompositeNode::GetChildIndex(const UDTNode& ChildNode) const
{
	return 0;
}

bool UDTCompositeNode::IsApplyingDecoratorScope() const
{
	return bApplyDecoratorScope;
}

bool UDTCompositeNode::CanAbortLowerPriority() const
{
	return false;
}

bool UDTCompositeNode::CanAbortSelf() const
{
	return false;
}
