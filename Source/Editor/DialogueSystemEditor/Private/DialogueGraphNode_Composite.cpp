// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueGraphNode_Composite.h"
#include "DTNode.h"
#include "DTCompositeNode.h"

UDialogueGraphNode_Composite::UDialogueGraphNode_Composite(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

FText UDialogueGraphNode_Composite::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	const UDTNode* MyNode = Cast<UDTNode>(NodeInstance);
	if (MyNode != nullptr)
	{
		return FText::FromString(MyNode->GetNodeName());
	}
	return Super::GetNodeTitle(TitleType);
}

FText UDialogueGraphNode_Composite::GetDescription() const
{
	const UDTCompositeNode* CompositeNode = Cast<UDTCompositeNode>(NodeInstance);
	if (CompositeNode&& CompositeNode->IsApplyingDecoratorScope())
	{
		return FText::Format(FText::FromString(TEXT("{0}\n{1}")),
			Super::GetDescription(),
			NSLOCTEXT("DialogueTreeEditor", "CompositeNodeScopeDesc", "Local scope for observers"));
	}
	return Super::GetDescription();
}

FText UDialogueGraphNode_Composite::GetTooltipText() const
{
	const UDTCompositeNode* CompositeNode = Cast<UDTCompositeNode>(NodeInstance);
	if (CompositeNode&& CompositeNode->IsApplyingDecoratorScope())
	{
		return FText::Format(FText::FromString(TEXT("{0}\n\n{1}")),
			Super::GetDescription(),
			NSLOCTEXT("DialogueTreeEditor", "CompositeNodeScopeTooltip", "Local scope for observers"));
	}
	return Super::GetDescription();
}

void UDialogueGraphNode_Composite::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
	AddContextMenuActionsDecorators(Context);
	AddContextMenuActionsServices(Context);
}
