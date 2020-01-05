// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "DialogueGraph.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEMEDITOR_API UDialogueGraph : public UEdGraph
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
	int32 GraphVersion;

public:
	virtual void OnCreated();
	virtual void OnLoaded();
	virtual void Initialize();
	virtual void UpdateAsset(int32 UpdateFlags = 0);

	virtual void UpdateVersion();
	virtual void MarkVersion();

	virtual void OnSubNodeDropped();
	virtual void OnNodesPasted(const FString& ImportStr);

	bool UpdateUnKonwnNodeClasses();
	void UpdateDeprecatedClasses();
	void RemoveOrphanedNodes();
	void UpdateClassData();

	bool IsLocked()const;
	void LockUpdates();
	void UnlockUpdates();

	// begin
	virtual void Serialize(FArchive& Ar) override;
	// end

protected:
	uint32 bLockUpdates : 1;

	virtual void CollectAllNodeInstances(TSet<UObject*>& NodeInstances);
	virtual bool CanRemoveNestedObject(UObject* TestObject)const;
	virtual void OnNodeInstanceRemoved(UObject* NodeInstance);

	UEdGraphPin* FindGraphNodePin(UEdGraphNode* Node, EEdGraphPinDirection Dir);
};
