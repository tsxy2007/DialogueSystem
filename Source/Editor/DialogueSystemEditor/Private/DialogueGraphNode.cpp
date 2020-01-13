// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueGraphNode.h"
#include "ScopedTransaction.h"
#include "EdGraph/EdGraph.h"

#define LOCTEXT_NAMESPACE "Dialogue"

UDialogueGraphNode::UDialogueGraphNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UDialogueGraphNode::PostPlacedNewNode()
{

}

UDialogueGraph * UDialogueGraphNode::GetDialogueGraph()
{
	return CastChecked<UDialogueGraph>(GetGraph());
}

void UDialogueGraphNode::AutowireNewNode(UEdGraphPin * FromPin)
{
	Super::AutowireNewNode(FromPin);
	if (FromPin != nullptr)
	{
		UEdGraphPin* OutputPin = GetOutputPin();
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
		else if (OutputPin != nullptr&& GetSchema()->TryCreateConnection(OutputPin, FromPin))
		{
			NodeConnectionListChanged();
		}
	}
}

void UDialogueGraphNode::PrepareForCopying()
{
}

bool UDialogueGraphNode::CanDuplicateNode() const
{
	return false;
}

bool UDialogueGraphNode::CanUserDeleteNode() const
{
	return false;
}

void UDialogueGraphNode::DestroyNode()
{
	if (ParentNode)
	{
		ParentNode->RemoveSubNode(this);
	}
	UEdGraphNode::DestroyNode();
}

FText UDialogueGraphNode::GetTooltipText() const
{
	return FText();
}

void UDialogueGraphNode::NodeConnectionListChanged()
{
}

bool UDialogueGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema * Schema) const
{
	return false;
}

void UDialogueGraphNode::FindDiffs(UEdGraphNode * OtherNode, FDiffResults & Results)
{
}

FString UDialogueGraphNode::GetPropertyNameAndValueForDiff(const UProperty * Prop, const uint8 * PropertyAddr) const
{
	return FString();
}
#if WITH_EDITOR

void UDialogueGraphNode::PostEditImport()
{
}

void UDialogueGraphNode::PostEditUndo()
{
}

void UDialogueGraphNode::AddSubNode(UDialogueGraphNode * SubNode, UEdGraph * ParentGraph)
{
	const FScopedTransaction Transaction(LOCTEXT("AddNode", "Add Node"));
	ParentGraph->Modify();
	Modify();
	SubNode->SetFlags(RF_Transactional);

	SubNode->Rename(nullptr, ParentNode, REN_NonTransactional);
	SubNode->ParentNode = this;
	SubNode->CreateNewGuid();
	SubNode->PostPlacedNewNode();
	SubNode->AllocateDefaultPins();
	SubNode->AutowireNewNode(nullptr);
	SubNode->NodePosX = 0;
	SubNode->NodePosY = 0;
	SubNodes.Add(SubNode);
	
	// OnSubNodeAdded(subnode);
	ParentGraph->NotifyGraphChanged();
	GetDialogueGraph()->UpdateAsset();
}

void UDialogueGraphNode::RemoveSubNode(UDialogueGraphNode * SubNode)
{
	SubNodes.RemoveSingle(SubNode);
	Modify();
	// OnSubNodeRemoved(SubNode);
}

#endif

UEdGraphPin * UDialogueGraphNode::GetInputPin(int32 InputIndex) const
{
	check(InputIndex >= 0);
	for (int32 PinIndex = 0,FoundInputs = 0 ;PinIndex<Pins.Num();PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Input)
		{
			if (InputIndex == FoundInputs)
			{
				return Pins[PinIndex];
			}
			else
			{
				FoundInputs++;
			}
		}
	}
	return nullptr;
}

UEdGraphPin* UDialogueGraphNode::GetOutputPin(int32 InputIndex /*= 0*/) const
{
	check(InputIndex >= 0);
	for (int32 PinIndex = 0, FoundInputs = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Output)
		{
			if (InputIndex == FoundInputs)
			{
				return Pins[PinIndex];
			}
			else
			{
				FoundInputs++;
			}
		}
	}
	return nullptr;
}
#undef LOCTEXT_NAMESPACE