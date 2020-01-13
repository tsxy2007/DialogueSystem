// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "DialogueTree.h"
#include "DialogueGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEMEDITOR_API UDialogueGraphNode : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(transient)
		UDialogueGraphNode* ParentNode;
	UPROPERTY()
		TArray<UDialogueGraphNode*> SubNodes;

	UPROPERTY()
		UObject* Instance;
	// ~Begin UEdGraphNode Interface
	virtual void PostPlacedNewNode() override;
	virtual class UDialogueGraph* GetDialogueGraph();
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual void PrepareForCopying() override;
	virtual bool CanDuplicateNode() const override;
	virtual bool CanUserDeleteNode() const override;
	virtual void DestroyNode() override;
	virtual FText GetTooltipText() const override;
	virtual void NodeConnectionListChanged() override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void FindDiffs(class UEdGraphNode* OtherNode, FDiffResults& Results) override;
	virtual FString GetPropertyNameAndValueForDiff(const UProperty* Prop, const uint8* PropertyAddr) const override;
	// ~End UEdGraphNode Interface


#if WITH_EDITOR
	virtual void PostEditImport()override;
	virtual void PostEditUndo()override;
#endif

	void AddSubNode(UDialogueGraphNode* SubNode, class UEdGraph* ParentGraph);
	void RemoveSubNode(UDialogueGraphNode* SubNode);

	virtual UEdGraphPin* GetInputPin(int32 InputIndex = 0) const;

	virtual UEdGraphPin* GetOutputPin(int32 InputIndex = 0)const;
};
