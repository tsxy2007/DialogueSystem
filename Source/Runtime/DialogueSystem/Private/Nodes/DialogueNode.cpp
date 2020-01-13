// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogueNode.h"
#include "DialogueTree.h"

#define LOCTEXT_NAMESPACE "DialogueNode" 
UDialogueNode::UDialogueNode(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	DialogueTree = nullptr;
}

void UDialogueNode::InitializeFromAsset(UDialogueTree& Asset)
{
	DialogueTree = &Asset;
}

#undef LOCTEXT_NAMESPACE