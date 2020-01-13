// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "DTNode.h"
#include "DTCompositeNode.generated.h"

class UDTCompositeNode;

struct FSTCompositeMemory
{
	int8 CurrentChild;
	int8 OverrideChild;
};

UENUM()
enum class EDTChildIndex : uint8
{
	FirstNode,
	TaskNode,
};

UENUM()
namespace EDTDecoratorLogic
{
	enum Type
	{
		Invalid,
		Test,
		And,
		Or,
		Not,
	};
}

USTRUCT()
struct FDTDecoratorLogic
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TEnumAsByte<EDTDecoratorLogic::Type> Operation;

	UPROPERTY()
		uint16 Number;

	FDTDecoratorLogic():Operation(EDTDecoratorLogic::Invalid),Number(0){}
	FDTDecoratorLogic(uint8 InOperationi,uint16 InNumber) : Operation(InOperationi),Number(InNumber){}
};

USTRUCT()
struct FDTCompositeChild
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		UDTCompositeNode* ChildComposite;

};

UCLASS(Abstract)
class DIALOGUESYSTEM_API UDTCompositeNode : public UDTNode
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
	TArray<FDTCompositeChild> Children;

	~UDTCompositeNode();

	void InitializeComposite(uint16 InLastExecuionIndex);


	int32 GetChildIndex(const UDTNode& ChildNode) const;

	bool IsApplyingDecoratorScope()const;

#if WITH_EDITOR
	virtual bool CanAbortLowerPriority() const;
	virtual bool CanAbortSelf()const;
#endif

protected:
	UPROPERTY(EditAnywhere, Category = Composite)
		uint32 bApplyDecoratorScope : 1;

	uint16 LastExecutionIndex;
};

