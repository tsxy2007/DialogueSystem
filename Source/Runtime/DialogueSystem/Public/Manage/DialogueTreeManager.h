// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueTreeManager.generated.h"

class UDialogueTree;
class USTCompositeNode;
/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API UDialogueTreeManager : public UObject
{
	GENERATED_UCLASS_BODY()
	

public:
	bool LoadTree(UDialogueTree& Asset, USTCompositeNode* Root, uint16& InstanceMemorySize);

};
