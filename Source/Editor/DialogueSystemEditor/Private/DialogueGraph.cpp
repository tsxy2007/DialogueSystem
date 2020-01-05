// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueGraph.h"

UDialogueGraph::UDialogueGraph(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UDialogueGraph::OnCreated()
{

}

void UDialogueGraph::OnLoaded()
{

}

void UDialogueGraph::Initialize()
{

}

void UDialogueGraph::UpdateAsset(int32 UpdateFlags)
{
}

void UDialogueGraph::UpdateVersion()
{
}

void UDialogueGraph::MarkVersion()
{
}

void UDialogueGraph::OnSubNodeDropped()
{
}

void UDialogueGraph::OnNodesPasted(const FString & ImportStr)
{
}

bool UDialogueGraph::UpdateUnKonwnNodeClasses()
{
	return false;
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
	return false;
}

void UDialogueGraph::LockUpdates()
{
}

void UDialogueGraph::UnlockUpdates()
{
}

void UDialogueGraph::Serialize(FArchive & Ar)
{
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
