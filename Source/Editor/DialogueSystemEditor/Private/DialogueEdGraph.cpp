// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueEdGraph.h"
#include "DTGraphNode.h"
#include "DialogueSystemEditor.h"
#include "DialogueTreeEditorTypes.h"
#include "DTNode.h"
#include "DialogueTree.h"

#include "DialogueTreeGraphNode.h"
#include "DialogueGraphNode_Root.h"
#include "EdGraph/EdGraphSchema.h"


namespace STGraphHelpers
{
	void InitializeInjectedNodes(UDialogueTreeGraphNode* GraphNode, UDTNode* RootNode, uint16 ExecutionIndex, uint8 TreeDepth, int32 Index)
	{

	}

	void VerifyDecorators(UDialogueTreeGraphNode* GraphNode)
	{

	}

	void CreateChildren(UDialogueTree* STAsset, UDTNode* RootNode, const UDialogueTreeGraphNode* RootEdNode, uint16* ExcutionIndex, uint8 TreeDepth)
	{
	//	if (RootEdNode == nullptr)
	//	{
	//		return;
	//	}
	//	RootNode->Children.Reset();
	//	
	//	int32 ChildIdx = 0;
	//	for (int32 PinIdx = 0; PinIdx < RootEdNode->Pins.Num(); PinIdx++)
	//	{
	//		UEdGraphPin* Pin = RootEdNode->Pins[PinIdx];
	//		if (Pin->Direction != EGPD_Output)
	//		{
	//			continue;
	//		}
	//		Pin->LinkedTo.Sort(FCompareNodeXLocation());
	//		for (int32 Index = 0; Index < Pin->LinkedTo.Num(); ++Index)
	//		{
	//			UDialogueTreeGraphNode* GraphNode = Cast<UDialogueTreeGraphNode>(Pin->LinkedTo[Index]->GetOwningNode());
	//			if (GraphNode == nullptr)
	//			{
	//				continue;;
	//			}
	//			// TODO: TASK


	//			//comp
	//			UDTNode* CompositeInstance = Cast<UDTNode>(GraphNode->NodeInstance);
	//			if (CompositeInstance&&Cast<UDialogueTree>(CompositeInstance->GetOuter()) == nullptr)
	//			{
	//				CompositeInstance->Rename(nullptr, STAsset);
	//			}

	//			if (CompositeInstance == nullptr)
	//			{
	//				continue;
	//			}

	//			ChildIdx = RootNode->Children.AddDefaulted();
	//			FDTCompositeChild& ChildInfo = RootNode->Children[ChildIdx];
	//			ChildInfo.ChildComposite = CompositeInstance;

	//			UDTNode* ChildNode = CompositeInstance ? (UDTNode*)CompositeInstance : nullptr;
	//			if (ChildNode && Cast<UDialogueTree>(ChildNode->GetOuter()) == nullptr)
	//			{
	//				ChildNode->Rename(nullptr, STAsset);
	//			}

	//			InitializeInjectedNodes(GraphNode, RootNode, *ExcutionIndex, TreeDepth, ChildIdx);

	//			// 

	//			ChildNode->InitializeNode(RootNode, *ExcutionIndex, 0, TreeDepth);
	//			*ExcutionIndex += 1;

	//			VerifyDecorators(GraphNode);
	//			
	//			if (CompositeInstance)
	//			{
	//				CreateChildren(STAsset, CompositeInstance, GraphNode, ExcutionIndex, TreeDepth + 1);
	//				CompositeInstance->InitializeComposite((*ExcutionIndex) - 1);
	//			}
	//			
	//		}
	//	}

	}
}


UDialogueEdGraph::UDialogueEdGraph(const FObjectInitializer& Objectinitlializer)
	:Super(Objectinitlializer)
{
	bLockUpdates = false;
}

void UDialogueEdGraph::OnCreated()
{
	MarkVersion();
	
}

void UDialogueEdGraph::OnLoaded()
{
	UpdateDeprecatedClasses();
	UpdateUnknownNodeClasses();
}

void UDialogueEdGraph::Initialize()
{
	UpdateVersion();
}

void UDialogueEdGraph::UpdateAsset(int32 UpdateFlags /*= 0*/)
{
	if (bLockUpdates)
	{
		return;
	}
	UDialogueGraphNode_Root* RootNode = nullptr;
	for (int32 Index = 0; Index < Nodes.Num(); ++Index)
	{
		UDialogueTreeGraphNode* Node = Cast<UDialogueTreeGraphNode>(Nodes[Index]);
		if (Node == nullptr)
		{
			continue;
		}
		// TODO Debugger flag;
		//parent chain
		Node->ParentNode = nullptr;
		for (int32 iAux = 0; iAux < Node->Services.Num(); iAux++)
		{
			Node->Services[iAux]->ParentNode = Node;
		}

		// prepare node instance 
		UDTNode* NodeInstance = Cast<UDTNode>(Node->NodeInstance);
		if (NodeInstance)
		{
			NodeInstance->InitializeNode(nullptr, MAX_uint16, 0, 0);
		}

		// cache root;
		if (RootNode == nullptr)
		{
			RootNode = Cast<UDialogueGraphNode_Root>(Nodes[Index]);
		}
	}

	// we 
	UEdGraphPin::ResolveAllPinReferences();
	if (RootNode && RootNode->Pins.Num() > 0 && RootNode->Pins[0]->LinkedTo.Num() > 0)
	{
		UDialogueTreeGraphNode* Node = Cast<UDialogueTreeGraphNode>(RootNode->Pins[0]->LinkedTo[0]->GetOwningNode());
		if (Node)
		{
			CreateSTFromGraph(Node);
			if ((UpdateFlags & KeepRebuildCounter) == 0)
			{
				ModCounter++;
			}
		}
	}
	// TODO Update blockboardchange();
}

void UDialogueEdGraph::UpdateVersion()
{
	if (GrapVersion == 1)
	{
		return;
	}
	MarkVersion();
	Modify();
}

void UDialogueEdGraph::MarkVersion()
{
	GrapVersion = 1;
}

void UDialogueEdGraph::CreateSTFromGraph(class UDialogueTreeGraphNode* RootEdNode)
{
	UDialogueTree* STAsset = Cast<UDialogueTree>(GetOuter());
	STAsset->RootNode = nullptr;

	uint16 ExecutionIndex = 0;
	uint8 TreeDepth = 0;
	STAsset->RootNode = Cast<UDTNode>(RootEdNode->NodeInstance);
	if (STAsset->RootNode)
	{
		STAsset->RootNode->InitializeNode(nullptr, ExecutionIndex, 0, TreeDepth);
		ExecutionIndex++;
	}

	

	// connect tree nodes;
	STGraphHelpers::CreateChildren(STAsset, STAsset->RootNode, RootEdNode, &ExecutionIndex, TreeDepth + 1);

	RootEdNode->bRootLevel = true;
	
	if (STAsset->RootNode)
	{
		//STAsset->RootNode->InitializeComposite(ExecutionIndex - 1);
	}
	RemoveOrphanedNodes();
}

void UDialogueEdGraph::OnSubNodeDropped()
{
	NotifyGraphChanged();
}

void UDialogueEdGraph::OnNodesPasted(const FString& ImportStr)
{

}

void UDialogueEdGraph::OnSave()
{
	UpdateAsset();
}

bool UDialogueEdGraph::UpdateUnknownNodeClasses()
{
	bool bUpdated = false;
	for (int32 NodeIdx = 0; NodeIdx < Nodes.Num(); NodeIdx++)
	{
		UDTGraphNode* MyNode = Cast<UDTGraphNode>(Nodes[NodeIdx]);
		if (MyNode)
		{
			const bool bUpdatedNode = MyNode->RefreshNodeClass();
			bUpdated = bUpdated || bUpdatedNode;
			for (int32 SubNodeIdx = 0; SubNodeIdx < MyNode->SubNodes.Num(); SubNodeIdx++)
			{
				if (MyNode->SubNodes[SubNodeIdx])
				{
					const bool bUpdatedSubNode = MyNode->SubNodes[SubNodeIdx]->RefreshNodeClass();
					bUpdated = bUpdated || bUpdatedSubNode;
				}
			}
		}
	}
	return bUpdated;
}

void UpdateDialogueGraphNodeErrorMessage(UDTGraphNode& Node)
{
	if (Node.NodeInstance)
	{
		Node.ErrorMessage = FDialogueGraphNodeClassHelper::GetDeprecationMessage(Node.NodeInstance->GetClass());
	}
	else
	{
		FString StoredClassName = Node.ClassData.GetClassName();
		StoredClassName.RemoveFromEnd(TEXT("_C"));
		if (!StoredClassName.IsEmpty())
		{
			static const FString IsMissingClassMessage(" class missing. Referenced by ");
			Node.ErrorMessage = StoredClassName + IsMissingClassMessage + Node.GetFullName();
		}
	}
	if (Node.HasErrors())
	{
		UE_LOG(LogDialogueEditor, Error, TEXT("%s"),*Node.ErrorMessage);
	}
}

bool UDialogueEdGraph::UpdateDeprecatedClasses()
{
	for (int32 Idx = 0, IdxNum = Nodes.Num(); Idx < IdxNum; ++Idx)
	{
		UDTGraphNode* Node = Cast<UDTGraphNode>(Nodes[Idx]);
		if (Node != nullptr)
		{
			UpdateDialogueGraphNodeErrorMessage(*Node);
			for (int32 SubIdx = 0, SubIdxNum = Node->SubNodes.Num(); SubIdx < SubIdxNum; ++SubIdx)
			{
				if (Node->SubNodes[SubIdx] != nullptr)
				{
					UpdateDialogueGraphNodeErrorMessage(*Node->SubNodes[SubIdx]);
				}
			}
		}
	}
	return true;
}

void UDialogueEdGraph::RemoveOrphanedNodes()
{
	TSet<UObject*> NodeInstances;
	CollectAllNodeInstance(NodeInstances);

	NodeInstances.Remove(nullptr);

	TArray<UObject*> AllInners;

	const bool bIncludeNestedObjects = false;
	GetObjectsWithOuter(GetOuter(), AllInners, bIncludeNestedObjects);

	for (auto InnerIt = AllInners.CreateConstIterator(); InnerIt; ++InnerIt)
	{
		UObject* TestObject = *InnerIt;
		if (!NodeInstances.Contains(TestObject) && CanRemoveNestdObject(TestObject))
		{
			OnNodeInstanceRemoved(TestObject);

			TestObject->SetFlags(RF_Transient);
			TestObject->Rename(NULL, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional | REN_ForceNoResetLoaders);
		}
	}
}

void UDialogueEdGraph::UpdateClassData()
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		UDTGraphNode* Node = Cast<UDTGraphNode>(Nodes[Idx]);
		if (Node)
		{
			Node->UpdateNodeClassData();
			for (int32 SubIdx = 0; SubIdx < Node->SubNodes.Num(); SubIdx++)
			{
				if (Node->SubNodes[SubIdx])
				{
					Node->UpdateNodeClassData();
				}
			}
		}
	}
}

bool UDialogueEdGraph::IsLocked() const
{
	return bLockUpdates;
}

void UDialogueEdGraph::LockedUpdates()
{

}

void UDialogueEdGraph::UnlockUpdates()
{

}

void UDialogueEdGraph::RebuildChildOrder(UEdGraphNode* ParentNode)
{
	bool bUpdateExecutionOrder = false;
	if (ParentNode)
	{
		for (int32 i = 0; i < ParentNode->Pins.Num(); i++)
		{
			UEdGraphPin* Pin = ParentNode->Pins[i];
			if (Pin->Direction == EGPD_Output)
			{
				TArray<UEdGraphPin*> PrevOrder(Pin->LinkedTo);
				Pin->LinkedTo.Sort(FCompareNodeXLocation());
				bUpdateExecutionOrder = bUpdateExecutionOrder || (PrevOrder != Pin->LinkedTo);
			}
		}
	}
	if (bUpdateExecutionOrder)
	{
		UpdateAsset(KeepRebuildCounter);
		Modify();
	}
}

void UDialogueEdGraph::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsSaving() || Ar.IsCooking())
	{
		UpdateDeprecatedClasses();
	}
}

void UDialogueEdGraph::AutoArrange()
{

}

void UDialogueEdGraph::CollectAllNodeInstance(TSet<UObject*>& NodeInstances)
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		UDTGraphNode* MyNode = Cast<UDTGraphNode>(Nodes[Idx]);
		if (MyNode)
		{
			NodeInstances.Add(MyNode->NodeInstance);
			for (int32 SubIdx = 0; SubIdx < MyNode->SubNodes.Num(); SubIdx++)
			{
				if (MyNode->SubNodes[SubIdx])
				{
					NodeInstances.Add(MyNode->SubNodes[SubIdx]->NodeInstance);
				}
			}
		}
	}
}

bool UDialogueEdGraph::CanRemoveNestdObject(UObject* TestObject) const
{
	return !TestObject->IsA(UEdGraphNode::StaticClass()) &&
		!TestObject->IsA(UEdGraph::StaticClass()) &&
		!TestObject->IsA(UEdGraphSchema::StaticClass());
}

void UDialogueEdGraph::OnNodeInstanceRemoved(UObject* NodeInstance)
{

}

UEdGraphPin* UDialogueEdGraph::FindGraphNodePin(UEdGraphNode* Node, EEdGraphPinDirection Dir)
{
	return nullptr;
}
