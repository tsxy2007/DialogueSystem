// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueTreeGraphNode.h"
#include "DialogueGraphNode_Composite.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEMEDITOR_API UDialogueGraphNode_Composite : public UDialogueTreeGraphNode
{
	GENERATED_UCLASS_BODY()
	
public:

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType)const override;
	virtual FText GetDescription() const override;
	virtual FText GetTooltipText() const override;
	virtual bool RefreshNodeClass() override { return false; }

	virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;

	virtual bool CanPlaceBreakpoints() const override { return true; }
};
