// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueGraphNode_Root.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphPin.h"
#include "DialogueTreeEditorTypes.h"


#define LOCTEXT_NAMESPACE "DialogueSystem"

UDialogueGraphNode_Root::UDialogueGraphNode_Root(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bIsReadOnly = true;
}

void UDialogueGraphNode_Root::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();
}

void UDialogueGraphNode_Root::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, UDialogueTreeEditorTypes::PinCategory_MultipleNodes, TEXT("In"));
}

FText UDialogueGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("DialogueTreeEditor", "Root", "ROOT");
}

FName UDialogueGraphNode_Root::GetNameIcon() const
{
	return FName("BTEditor.Graph.BTNode.Root.Icon");
}

FText UDialogueGraphNode_Root::GetTooltipText() const
{
	return UEdGraphNode::GetTooltipText();
}

void UDialogueGraphNode_Root::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	//TODO:
	/*if (PropertyChangedEvent.Property &&
		PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UDialogueGraphNode_Root, BlackboardAsset))
	{
	}*/
}

void UDialogueGraphNode_Root::PostEditUndo()
{
	Super::PostEditUndo();
	//TODO:
}

FText UDialogueGraphNode_Root::GetDescription() const
{
	return FText::FromString(GetNameSafe(this));
}

#undef LOCTEXT_NAMESPACE
