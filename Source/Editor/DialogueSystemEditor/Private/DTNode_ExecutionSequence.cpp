// Fill out your copyright notice in the Description page of Project Settings.


#include "DTNode_ExecutionSequence.h"
#include "EdGraphSchema_K2.h"

UDTNode_ExecutionSequence::UDTNode_ExecutionSequence(const FObjectInitializer& Objectilializer)
	:Super(Objectilializer)
{

}

void UDTNode_ExecutionSequence::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	//add two default pins
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, GetPinNameGivenIndex(0));
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, GetPinNameGivenIndex(1));

	Super::AllocateDefaultPins();
}

FText UDTNode_ExecutionSequence::GetTooltipText() const
{
	return NSLOCTEXT("STNode", "ExecutePinInOrder_Tooltip", "Executes a series of pins in order");
}

FLinearColor UDTNode_ExecutionSequence::GetNodeTitleColor() const
{
	return FLinearColor::White;
}

FText UDTNode_ExecutionSequence::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("STNode", "Sequence", "Sequence");
}

FSlateIcon UDTNode_ExecutionSequence::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.Sequence_16x");
	return Icon;
}

void UDTNode_ExecutionSequence::AddInputPin()
{

}

bool UDTNode_ExecutionSequence::CanAddPin() const
{

	return true;
}

FName UDTNode_ExecutionSequence::GetPinNameGivenIndex(int32 Index) const
{
	return *FString::Printf(TEXT("%s_%d"), *UEdGraphSchema_K2::PN_Then.ToString(), Index);
}
