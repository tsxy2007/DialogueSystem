// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueGraph.h"
#include "EdGraphSchema_DialogueTree.h"

UDialogueGraph::UDialogueGraph(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Schema = UEdGraphSchema_DialogueTree::StaticClass();
}

void UDialogueGraph::OnCreated()
{
	MarkVersion();
}

void UDialogueGraph::OnLoaded()
{
	UpdateDeprecatedClasses();
	UpdateUnKonwnNodeClasses();
}

void UDialogueGraph::Initialize()
{
	UpdateVersion();
}

void UDialogueGraph::UpdateAsset(int32 UpdateFlags)
{
	
}

void UDialogueGraph::UpdateVersion()
{
	if (GraphVersion == 1)
	{
		return;
	}
	MarkVersion();
	Modify();
}

void UDialogueGraph::MarkVersion()
{
	GraphVersion = 1;
}

void UDialogueGraph::OnSubNodeDropped()
{
}

void UDialogueGraph::OnNodesPasted(const FString & ImportStr)
{
}

bool UDialogueGraph::UpdateUnKonwnNodeClasses()
{
	bool bUpdated = false;
	for (int32 i = 0; i < Nodes.Num(); i++)
	{
		UEdGraphNode* MyNode = Cast<UEdGraphNode>(Nodes[i]);
		if (MyNode)
		{
			//const bool bUpdatedNode = ;
		}
	}
	return bUpdated;
}

void UDialogueGraph::UpdateDeprecatedClasses()
{
}

void UDialogueGraph::RemoveOrphanedNodes()
{
}

void UDialogueGraph::UpdateClassData()
{
}

bool UDialogueGraph::IsLocked() const
{
	return bLockUpdates;
}

void UDialogueGraph::LockUpdates()
{
	bLockUpdates = true;
}

void UDialogueGraph::UnlockUpdates()
{
	bLockUpdates = false;
	UpdateAsset();
}

void UDialogueGraph::Serialize(FArchive & Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsSaving() || Ar.IsCooking())
	{
		UpdateDeprecatedClasses();
	}
}

void UDialogueGraph::CollectAllNodeInstances(TSet<UObject*>& NodeInstances)
{
}

bool UDialogueGraph::CanRemoveNestedObject(UObject * TestObject) const
{
	return false;
}

void UDialogueGraph::OnNodeInstanceRemoved(UObject * NodeInstance)
{
}

UEdGraphPin * UDialogueGraph::FindGraphNodePin(UEdGraphNode * Node, EEdGraphPinDirection Dir)
{
	return nullptr;
}
