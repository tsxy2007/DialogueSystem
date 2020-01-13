// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTree.h"

UDialogueTree::UDialogueTree(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

#if WITH_EDITOR

void UDialogueTree::AddNode(UDialogueNode* InNode)
{
	
	InNode->Rename(nullptr, this);
	int32 Index = Data.Add(InNode);
}

void UDialogueTree::RemoveNode(UDialogueNode* NodeToRemove)
{

}

#endif
