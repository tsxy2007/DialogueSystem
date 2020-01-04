// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Blueprint.h"
#include "DialogueTree.generated.h"

class UEdGraph;
/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API UDialogueTree : public UObject
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UEdGraph* DTGraph;

	UPROPERTY()
	TArray<FEditedDocumentInfo> LastEditedDocuments;
#endif // WITH_EDITORONLY_DATA

};
