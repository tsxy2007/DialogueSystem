// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/Object.h"
#include "Engine/Blueprint.h"
#include "DialogueTree.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEM_API UDialogueTree : public UObject
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITORONLY_DATA
		UPROPERTY()
		class UEdGraph* BTGraph;

	/** The MyProperty */
	UPROPERTY()
	TArray<FEditedDocumentInfo> LastEditedDocuments;
#endif

	UPROPERTY()
	class UDTNode* RootNode;
};
