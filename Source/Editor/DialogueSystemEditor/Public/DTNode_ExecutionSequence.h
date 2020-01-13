// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueTreeGraphNode.h"
#include "Textures/SlateIcon.h"
#include "D2Node_AddPinInterface.h"
#include "EdGraph/EdGraphPin.h"
#include "DTNode_ExecutionSequence.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEMEDITOR_API UDTNode_ExecutionSequence : public UDialogueTreeGraphNode,public ID2Node_AddPinInterface
{
	GENERATED_UCLASS_BODY()
public:
	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	//~ End UEdGraphNode Interface

	// Begin AddPinInterface
	virtual void AddInputPin() override;
	virtual bool CanAddPin()const override;
	// End

private:
	virtual FName GetPinNameGivenIndex(int32 Index) const;
};
