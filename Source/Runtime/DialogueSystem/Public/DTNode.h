// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "DialogueTreeTypes.h"
#include "GameplayTaskOwnerInterface.h"
#include "DTNode.generated.h"

class UWorld;
class UDialogueTree;
class UDTCompositeNode;
class UDialogueTreeComponent;
struct FDialogueTreeSearchData;

struct FSTInstanceNodeMemory
{
	int32 NodeIdx;
};
/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API UDTNode : public UObject , public IGameplayTaskOwnerInterface
{
	GENERATED_UCLASS_BODY()
public:
	virtual UWorld* GetWorld() const override;
	
	// fill in data about tree structure
	void InitializeNode(UDTCompositeNode* InParentNode,uint16 InExecutionIndex,uint16 InMemoryOffset,uint8 InTreeDepth);

	// initialize any asset related data
	virtual void InitializeFromAsset(UDialogueTree& Asset);

	// initialize memory block
	virtual void InitializeMemory(UDialogueTreeComponent& OwnerComp, uint8* NodeMemory, EDTMemoryInit::Type InitType)const;

	// cleanup memory block
	virtual void CleanupMemory(UDialogueTreeComponent& OwnerComp, uint8* NodeMemory, EDTMemoryClear::Type CleanupType) const;

	// called when node instance is added to tree
	virtual void OnInstanceCreated(UDialogueTreeComponent& OwnerComp);


	//~Begin GamePlayTaskOwnerInterface
	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const  override;
	virtual AActor* GetGameplayTaskOwner(const UGameplayTask* Task) const override;
	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;
	virtual uint8 GetGameplayTaskDefaultPriority() const override;
	virtual void OnGameplayTaskInitialized(UGameplayTask& Task) override;
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	//~End GamePlayTaskOwnerInterface

	

	UDialogueTree* GetTreeAsset() const;

	UDTCompositeNode* GetParentNode()const;
#if USE_DialogueTREE_DEBUGGER
	UDTNode* GetNextNode()const;
#endif

#if WITH_EDITOR
	virtual FName GetNodeIconName()const;
	virtual void OnNodeCreated() {}
	FString GetNodeName() const;
#endif
	uint16 GetExecutionIndex()const;
	uint16 GetMemoryOffset()const;
	uint8 GetTreeDepth()const;
	void MarkInjectedNode();
	bool IsInjected()const;
	void ForceInstancing(bool bEnable);
	bool HasInstance()const;
	bool IsInstanced()const;


	UPROPERTY(Category = Description, EditAnywhere)
	FString NodeName;
private:
	UPROPERTY()
		UDialogueTree* TreeAsset;

	UPROPERTY()
		UDTCompositeNode* ParentNode;

#if USE_DialogueTREE_DEBUGGER
	UDTNode* NextExecutionNode;
#endif

	uint16 ExecutionIndex;

	uint16 MemoryOffset;

	uint8 TreeDepth;

	uint8 bIsInstanced : 1;

	uint8 bIsInjected : 1;

protected:
	uint8 bCreateNodeInstance : 1;

	uint8 bOwnsGamePlayTasks : 1;

};


FORCEINLINE UDialogueTree* UDTNode::GetTreeAsset()const
{
	return TreeAsset;
}

FORCEINLINE UDTCompositeNode* UDTNode::GetParentNode() const
{
	return ParentNode;
}
#if USE_DialogueTREE_DEBUGGER
FORCEINLINE UDTNode* UDTNode::GetNextNode()const
{
	return NextExecutionNode;
}
#endif

FORCEINLINE uint16 UDTNode::GetExecutionIndex()const
{
	return ExecutionIndex;
}

FORCEINLINE uint16 UDTNode::GetMemoryOffset()const
{
	return MemoryOffset;
}

FORCEINLINE uint8 UDTNode::GetTreeDepth()const
{
	return TreeDepth;
}

FORCEINLINE void UDTNode::MarkInjectedNode()
{
	bIsInjected = true;
}

FORCEINLINE bool UDTNode::IsInjected()const
{
	return bIsInjected;
}

FORCEINLINE void UDTNode::ForceInstancing(bool bEnable)
{
	check(ParentNode == NULL);
	bCreateNodeInstance = bEnable;
}

FORCEINLINE bool UDTNode::HasInstance()const
{
	return bCreateNodeInstance;
}

FORCEINLINE bool UDTNode::IsInstanced()const
{
	return bIsInstanced;
}
