// Fill out your copyright notice in the Description page of Project Settings.


#include "Manage/DialogueTreeManager.h"
#include "Dialogue.h"
#include "DTCompositeNode.h"

UDialogueTreeManager::UDialogueTreeManager(const FObjectInitializer& Obj)
	:Super(Obj)
{

}

bool UDialogueTreeManager::LoadTree(UDialogue& Asset, USTCompositeNode* Root, uint16& InstanceMemorySize)
{

	return true;
}