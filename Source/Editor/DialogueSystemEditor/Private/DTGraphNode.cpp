// Fill out your copyright notice in the Description page of Project Settings.


#include "DTGraphNode.h"
#include "AssetData.h"
#include "EdGraph/EdGraphSchema.h"
#include "DialogueGraphTypes.h"
#include "DialogueEdGraph.h"
#include "DiffResults.h"
#include "ScopedTransaction.h"
#include "BlueprintNodeHelpers.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "Engine/Blueprint.h"
#include "DTNode.h"

#define LOCTEXT_NAMESPACE "DialogueGraph"
UDTGraphNode::UDTGraphNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	NodeInstance = nullptr;
	CopySubNodeIndex = 0;
	bIsReadOnly = false;
	bIsSubNode = false;
}

void UDTGraphNode::AddSubNode(UDTGraphNode* SubNode, class UEdGraph* ParentGraph)
{
	const FScopedTransaction Transaction(LOCTEXT("AddNode", "Add Node"));
	ParentGraph->Modify();
	Modify();
	SubNode->SetFlags(RF_Transactional);
	SubNode->Rename(nullptr, ParentGraph, REN_NonTransactional);

	SubNode->ParentNode = this;
	SubNode->CreateNewGuid();
	SubNode->PostPlacedNewNode();
	SubNode->AllocateDefaultPins();
	SubNode->AutowireNewNode(nullptr);

	SubNode->NodePosX = 0;
	SubNode->NodePosY = 0;
	SubNodes.Add(SubNode);
	
	OnSubNodeAdded(SubNode);

	ParentGraph->NotifyGraphChanged();
	GetSTGraph()->UpdateAsset();
}

void UDTGraphNode::RemoveSubNode(UDTGraphNode* SubNode)
{
	SubNodes.RemoveSingle(SubNode);
	Modify();
	OnSubNodeRemoved(SubNode);
}

void UDTGraphNode::RemoveAllSubNodes()
{
	SubNodes.Reset();
}

void UDTGraphNode::OnSubNodeRemoved(UDTGraphNode * SubNode)
{
}

void UDTGraphNode::OnSubNodeAdded(UDTGraphNode * SubNode)
{
}

class UDialogueEdGraph* UDTGraphNode::GetSTGraph()
{
	return CastChecked<UDialogueEdGraph>(GetGraph());
}

FText UDTGraphNode::GetDescription() const
{
	FString StoredClassName = ClassData.GetClassName();
	StoredClassName.RemoveFromEnd(TEXT("_C"));
	return FText::Format(LOCTEXT("NodeClassError", "Class {0} not found , make sure it's saved!"), FText::FromString(StoredClassName));
}

void UDTGraphNode::PostCopyNode()
{
	ResetNodeOwner();
}

bool UDTGraphNode::IsSubNode() const
{
	return bIsSubNode || (ParentNode != nullptr);
}

int32 UDTGraphNode::FindSubNodeDropIndex(UDTGraphNode * SubNode) const
{
	const int32 InsertIndex = SubNodes.IndexOfByKey(SubNode);
	return InsertIndex;
}

void UDTGraphNode::InsertSubNodeAt(UDTGraphNode * SubNode, int32 DropIndex)
{
	if (DropIndex > -1)
	{
		SubNodes.Insert(SubNode, DropIndex);
	}
	else
	{
		SubNodes.Add(SubNode);
	}
}

void UDTGraphNode::InitializeInstance()
{
}

bool UDTGraphNode::RefreshNodeClass()
{
	bool bUpdate = false;
	if (NodeInstance == nullptr)
	{
		if (FDialogueGraphNodeClassHelper::IsClassKnown(ClassData))
		{
			PostPlacedNewNode();
			bUpdate = (NodeInstance != nullptr);
		}
		else
		{
			FDialogueGraphNodeClassHelper::AddUnknownClass(ClassData);
		}
	}
	return bUpdate;
}

void UDTGraphNode::UpdateNodeClassData()
{
	if (NodeInstance)
	{
		UpdateNodeClassDataFrom(NodeInstance->GetClass(), ClassData);
		ErrorMessage = ClassData.GetDeprecatedMessage();
	}
}

bool UDTGraphNode::UsesBlueprint() const
{
	return NodeInstance && NodeInstance->GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint);
}

bool UDTGraphNode::HasErrors() const
{
	return ErrorMessage.Len() > 0 || NodeInstance == nullptr;
}

void UDTGraphNode::UpdateNodeClassDataFrom(UClass* InstanceClass, FDialogueGraphNodeClassData& UpdatedData)
{
	if (InstanceClass)
	{
		UBlueprint* BPOwner = Cast<UBlueprint>(InstanceClass->ClassGeneratedBy);
		if (BPOwner)
		{
			UpdatedData = FDialogueGraphNodeClassData(BPOwner->GetName(), BPOwner->GetOutermost()->GetName(), InstanceClass->GetName(), InstanceClass);
		}
		else
		{
			UpdatedData = FDialogueGraphNodeClassData(InstanceClass,FDialogueGraphNodeClassHelper::GetDeprecationMessage(InstanceClass));
		}
	}
}

void UDTGraphNode::AutowireNewNode(UEdGraphPin * FromPin)
{
	Super::AutowireNewNode(FromPin);
	if (FromPin != nullptr)
	{
		UEdGraphPin* OutputPin = GetOutputPin();
		if (GetSchema()->TryCreateConnection(FromPin,GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
		else if (OutputPin != nullptr && GetSchema()->TryCreateConnection(OutputPin,FromPin))
		{
			NodeConnectionListChanged();
		}
	}
}

void UDTGraphNode::PostPlacedNewNode()
{
	UClass* NodeClass = ClassData.GetClass(true);
	if (NodeClass && (NodeInstance == nullptr))
	{
		UEdGraph* MyGraph = GetGraph();
		UObject* GraphOwner = MyGraph ? MyGraph->GetOuter() : nullptr;
		if (GraphOwner)
		{
			NodeInstance = NewObject<UObject>(GraphOwner, NodeClass);
			NodeInstance->SetFlags(RF_Transactional);
			InitializeInstance();
		}
	}
}

void UDTGraphNode::PrepareForCopying()
{
	if (NodeInstance)
	{
		NodeInstance->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

bool UDTGraphNode::CanDuplicateNode() const
{
	return bIsReadOnly ? false : Super::CanDuplicateNode();
}

bool UDTGraphNode::CanUserDeleteNode() const
{
	return bIsReadOnly ? false : Super::CanUserDeleteNode();
}

void UDTGraphNode::DestroyNode()
{
	if (ParentNode)
	{
		ParentNode->RemoveSubNode(this);
	}
	UEdGraphNode::DestroyNode();
}

FText UDTGraphNode::GetTooltipText() const
{
	FText TooltipDesc;
	if (!NodeInstance)
	{
		FString StoredClassName = ClassData.GetClassName();
		StoredClassName.RemoveFromEnd(TEXT("_C"));
		TooltipDesc = FText::Format(LOCTEXT("NodeClassError", "Class {0} not found , make sure it's saved!"), FText::FromString(StoredClassName));
	}
	else
	{
		if (ErrorMessage.Len() > 0)
		{
			TooltipDesc = FText::FromString(ErrorMessage);
		}
		else
		{
			if (NodeInstance->GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
			{
				FAssetData AssetData(NodeInstance->GetClass()->ClassGeneratedBy);
				FString Description = AssetData.GetTagValueRef<FString>(GET_MEMBER_NAME_CHECKED(UBlueprint, BlueprintDescription));
				if (!Description.IsEmpty())
				{
					Description.ReplaceInline(TEXT("\\n"), TEXT("\n"));
					TooltipDesc = FText::FromString(MoveTemp(Description));
				}
			}
			else
			{
				TooltipDesc = NodeInstance->GetClass()->GetToolTipText();
			}
		}
	}
	return TooltipDesc;
}

void UDTGraphNode::NodeConnectionListChanged()
{
	Super::NodeConnectionListChanged();
	GetSTGraph()->UpdateAsset();
}

bool UDTGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema * Schema) const
{
	return false;
}

void UDTGraphNode::FindDiffs(UEdGraphNode * OtherNode, FDiffResults & Results)
{
	Super::FindDiffs(OtherNode, Results);
	if (UDTGraphNode* OtherGraphNode = Cast<UDTGraphNode>(OtherNode))
	{
		if (NodeInstance && OtherGraphNode->NodeInstance)
		{
			FDiffSingleResult Diff;
			Diff.Diff = EDiffType::NODE_PROPERTY;
			Diff.Node1 = this;
			Diff.Node2 = OtherNode;
			Diff.ToolTip = LOCTEXT("DIF_NodeIntancePropertyToolTip", "A property of the node instance has changed");
			Diff.DisplayColor = FLinearColor(0.25f, 0.71f, 0.85f);
			DiffProperties(NodeInstance->GetClass(), OtherGraphNode->NodeInstance->GetClass(), NodeInstance, OtherGraphNode->NodeInstance, Results, Diff);
		}
	}
}

FString UDTGraphNode::GetPropertyNameAndValueForDiff(const UProperty * Prop, const uint8 * PropertyAddr) const
{
	return BlueprintNodeHelpers::DescribeProperty(Prop,PropertyAddr);
}

#if WITH_EDITOR
void UDTGraphNode::PostEditImport()
{
	ResetNodeOwner();
	if (NodeInstance)
	{
		InitializeInstance();
	}
}

void UDTGraphNode::PostEditUndo()
{
	UEdGraphNode::PostEditUndo();
	ResetNodeOwner();
	if (ParentNode)
	{
		ParentNode->SubNodes.AddUnique(this);
	}
}
#endif

UEdGraphPin * UDTGraphNode::GetInputPin(int32 InputIndex) const
{
	check(InputIndex >= 0);
	for (int32 PinIndex = 0, FoundInputs = 0; PinIndex < Pins.Num(); PinIndex++)
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

UEdGraphPin * UDTGraphNode::GetOutputPin(int32 InputIndex) const
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

void UDTGraphNode::ResetNodeOwner()
{
	if (NodeInstance)
	{
		UEdGraph* MyGraph = GetGraph();
		UObject* GraphOwner = MyGraph ? MyGraph->GetOuter() : nullptr;
		NodeInstance->Rename(NULL, GraphOwner, REN_DontCreateRedirectors | REN_DoNotDirty);
		NodeInstance->ClearFlags(RF_Transactional);

		for (auto & SubNode : SubNodes)
		{
			SubNode->ResetNodeOwner();
		}
	}
}


#undef LOCTEXT_NAMESPACE