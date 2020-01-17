// Fill out your copyright notice in the Description page of Project Settings.

#include "DTNode_NPC.h"

UDTNode_NPC::UDTNode_NPC(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	DialogueType = EDialogueType1::EDT_NPC;
}