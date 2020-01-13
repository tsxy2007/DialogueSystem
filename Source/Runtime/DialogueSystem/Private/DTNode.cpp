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

void UDTNode::InitializeNode(UDTCompositeNode* InParentNode, uint16 InExecutionIndex, uint16 InMemoryOffset, uint8 InTreeDepth)
{
	ParentNode = InParentNode;
	ExecutionIndex = InExecutionIndex;
	MemoryOffset = InMemoryOffset;
	TreeDepth = InTreeDepth;
}

void UDTNode::InitializeFromAsset(UDialogue& Asset)
{

}

void UDTNode::InitializeMemory(UDialogueTreeComponent& OwnerComp, uint8* NodeMemory, EDTMemoryInit::Type InitType) const
{

}

void UDTNode::CleanupMemory(UDialogueTreeComponent& OwnerComp, uint8* NodeMemory, EDTMemoryClear::Type CleanupType) const
{

}

void UDTNode::OnInstanceCreated(UDialogueTreeComponent& OwnerComp)
{

}

UGameplayTasksComponent * UDTNode::GetGameplayTasksComponent(const UGameplayTask & Task) const
{
	return nullptr;
}

AActor * UDTNode::GetGameplayTaskOwner(const UGameplayTask * Task) const
{
	return nullptr;
}

AActor * UDTNode::GetGameplayTaskAvatar(const UGameplayTask * Task) const
{
	return nullptr;
}

uint8 UDTNode::GetGameplayTaskDefaultPriority() const
{
	return uint8();
}

void UDTNode::OnGameplayTaskInitialized(UGameplayTask & Task)
{
}

void UDTNode::OnGameplayTaskActivated(UGameplayTask & Task)
{
}

void UDTNode::OnGameplayTaskDeactivated(UGameplayTask & Task)
{
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
