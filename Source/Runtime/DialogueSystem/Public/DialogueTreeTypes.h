// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueTreeTypes.generated.h"

class USTNode;


#define USE_DialogueTREE_DEBUGGER (1&&WITH_EDITORONLY_DATA)

UENUM(BlueprintType)
namespace EDTNodeResult
{
	enum Type
	{
		Successed,
		Failed,
		Aborted,
		InProgress,
	};
}

namespace EDTExecutionMode
{
	enum Type
	{
		SingleRun,
		Looped,
	};
}

namespace EDTStopMode
{
	enum Type
	{
		Safe,
		Forced,
	};
}

namespace EDTMemoryInit
{
	enum Type
	{
		Initialize,
		RestoreSubtree,
	};
}

namespace EDTMemoryClear
{
	enum Type
	{
		Destory,
		StoreSubtree,
	};
}

UENUM()
namespace EDTFlowAbortMode
{
	enum Type
	{
		None			UMETA(DisplayName="Nothing"),
		LowerPriority	UMETA(DisplayName="Lower Priority"),
		Self			UMETA(DisplayName="Self"),
		Both			UMETA(DisplayName="Both"),
	};
}

namespace EDTActiveNode
{
	enum Type
	{
		Composite,
		ActiveTask,
		AbortingTask,
		InactiveTask,
	};
}

namespace EDTTaskStatus
{
	enum Type
	{
		Active,
		Aborting,
		Inactive,
	};
}

namespace EDTNodeUpdateMode
{
	enum Type
	{
		Unkown,
		Add,
		Remove,
	};
}



UCLASS(Abstract)
class DIALOGUESYSTEM_API UDialogueTreeTypes : public UObject
{
	GENERATED_BODY()

	static FString STLoggingContext;
public:
	//static FString DescribeNodeHelper(const USTNode* Node);
	//static FString DescribeNodeResult(EDTNodeResult::Type NodeResult);
	//static FString DescribeActiveNode(EDTFlowAbortMode::Type FlowAbortMode);

	static FString GetShortTypeName(const UObject* Ob);
};
