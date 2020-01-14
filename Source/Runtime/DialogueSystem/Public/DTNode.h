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
class UDTNode;
class UDialogueTreeComponent;
struct FDialogueTreeSearchData;

struct FSTInstanceNodeMemory
{
	int32 NodeIdx;
};
// �Ի�����
UENUM(BlueprintType)
enum class EDialogueType1 : uint8
{
	EDT_NPC,	// NPC 
	EDT_My,		// �Լ�˵��
	EDT_MySelect,// �Ҳ����ѡ��ĶԻ�
};
USTRUCT()
struct FDTCompositeChild
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	UDTNode* ChildComposite;
};
/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API UDTNode : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	virtual UWorld* GetWorld() const override;
	
	// fill in data about tree structure
	void InitializeNode(UDTNode* InParentNode,uint16 InExecutionIndex,uint16 InMemoryOffset,uint8 InTreeDepth);

	// initialize any asset related data
	virtual void InitializeFromAsset(UDialogueTree& Asset);

	UDialogueTree* GetTreeAsset() const;

	void InitializeIndex(uint16 InLastExecuionIndex);

	UDTNode* GetParentNode()const;
#if USE_DialogueTREE_DEBUGGER
	UDTNode* GetNextNode()const;
#endif

#if WITH_EDITOR
	virtual FName GetNodeIconName()const;
	virtual void OnNodeCreated() {}
	FString GetNodeName() const;
#endif
	uint16 GetExecutionIndex()const;


	UPROPERTY()
	TArray<FDTCompositeChild> Children;

	UPROPERTY(Category = Description, EditAnywhere)
	FString NodeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dialogue)
	FText Speaker;// ˵������

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dialogue)
	FText Content; // ˵��������

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dialogue)
	EDialogueType1 DialogueType; // ˵���˷�ʽ�·����Ҳ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dialogue)
		class UTexture2D* BG;
private:
	UPROPERTY()
		UDialogueTree* TreeAsset;

	UPROPERTY()
		UDTNode* ParentNode;

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

	uint16 LastExecutionIndex;
};


FORCEINLINE UDialogueTree* UDTNode::GetTreeAsset()const
{
	return TreeAsset;
}

FORCEINLINE UDTNode* UDTNode::GetParentNode() const
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
