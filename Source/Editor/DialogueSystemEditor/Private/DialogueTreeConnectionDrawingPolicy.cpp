// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeConnectionDrawingPolicy.h"
#include "DialogueTreeColors.h"
#include "DialogueTreeGraphNode.h"

FDialogueTreeConnectionDrawingPolicy::FDialogueTreeConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
	:FDTGraphConnectionDrawingPolicy(InBackLayerID,InFrontLayerID,ZoomFactor,InClippingRect,InDrawElements,InGraphObj)
{

}

void FDialogueTreeConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params)
{
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;
	Params.WireThickness = 1.5f;

	Params.WireColor = DialogueTreeColors::Connection::Default;

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;
	if (bDeemphasizeUnhoveredPins)
	{
		ApplyHoverDeemphasis(OutputPin, InputPin, Params.WireThickness, Params.WireColor);
	}

	UDialogueTreeGraphNode* FromNode = OutputPin ? Cast<UDialogueTreeGraphNode>(OutputPin->GetOwningNode()) : nullptr;
	UDialogueTreeGraphNode* ToNode = InputPin ? Cast<UDialogueTreeGraphNode>(InputPin->GetOwningNode()) : nullptr;
	if (ToNode && FromNode)
	{
		if ((ToNode->bDebuggerMarkCurrentlyActive&&FromNode->bDebuggerMarkCurrentlyActive) ||
			(ToNode->bDebuggerMarkPreviouslyActive&&FromNode->bDebuggerMarkPreviouslyActive))
		{
			Params.WireThickness = 10.f;
			Params.bDrawBubbles = true;
		}
	}
}
