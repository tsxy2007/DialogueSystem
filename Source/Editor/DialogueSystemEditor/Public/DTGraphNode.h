// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphNode.h"
#include "DialogueGraphTypes.h"
#include "DTGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEMEDITOR_API UDTGraphNode : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY()
	struct FDialogueGraphNodeClassData ClassData;

	UPROPERTY()
	UObject* NodeInstance;

	UPROPERTY(transient)
	UDTGraphNode* ParentNode;

	UPROPERTY()
	TArray<UDTGraphNode*> SubNodes;

	UPROPERTY()
	int32 CopySubNodeIndex;

	UPROPERTY()
	uint32 bIsReadOnly : 1;

	UPROPERTY()
	uint32 bIsSubNode : 1;

	UPROPERTY()
	FString ErrorMessage;
public:

	// ~begin UEdGraphNode Interface
	virtual class UDialogueEdGraph* GetSTGraph();
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual void PostPlacedNewNode() override;
	virtual void PrepareForCopying() override;
	virtual bool CanDuplicateNode() const override;
	virtual bool CanUserDeleteNode() const override;
	virtual void DestroyNode() override;
	virtual FText GetTooltipText() const override;
	virtual void NodeConnectionListChanged() override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void FindDiffs(class UEdGraphNode* OtherNode, FDiffResults& Results) override;
	virtual FString GetPropertyNameAndValueForDiff(const UProperty* Prop, const uint8* PropertyAddr) const override;
	//~end UEdGraphNode Interface

	// ~Begin UOBject Interface
#if WITH_EDITOR
	virtual void PostEditImport() override;
	virtual void PostEditUndo() override;
#endif
	//~End UObject Interface
	
	//@return the input pin for this state
	virtual UEdGraphPin* GetInputPin(int32 InputIndex = 0) const;
	//@return the output pin for this state
	virtual UEdGraphPin* GetOutputPin(int32 OutputIndex = 0) const;
	virtual UEdGraphPin* GetBoundGraph()const { return nullptr; }

	virtual FText GetDescription()const;
	virtual void PostCopyNode();

	void AddSubNode(UDTGraphNode* SubNode, class UEdGraph* ParentGraph);
	void RemoveSubNode(UDTGraphNode* SubNode);
	virtual void RemoveAllSubNodes();
	virtual void OnSubNodeRemoved(UDTGraphNode* SubNode);
	virtual void OnSubNodeAdded(UDTGraphNode* SubNode);

	virtual int32 FindSubNodeDropIndex(UDTGraphNode* SubNode)const;
	virtual void InsertSubNodeAt(UDTGraphNode* SubNode, int32 DropIndex);

	/*check if node is subnode*/
	virtual bool IsSubNode()const;

	/**/
	virtual void InitializeInstance();

	// 
	virtual bool RefreshNodeClass();

	//
	virtual void UpdateNodeClassData();

	//
	bool UsesBlueprint()const;

	//
	virtual bool HasErrors()const;

	//
	static void UpdateNodeClassDataFrom(UClass* InstanceClass, FDialogueGraphNodeClassData& UpdateData);

protected:
	virtual void ResetNodeOwner();
};
