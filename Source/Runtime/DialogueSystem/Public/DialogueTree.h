// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Blueprint.h"
#include "DialogueNode.h"
//#include "DialogueNode.h"
#include "DialogueTree.generated.h"

class UEdGraph;
class UDialogueNode;
//
//USTRUCT(Blueprintable)
//struct FDialogueNode 
//{
//	GENERATED_USTRUCT_BODY()
//public:
//	UPROPERTY()
//		FText Speaker;
//
//	UPROPERTY()
//		FText Content;
//
//	UPROPERTY()
//		EDialogueSpeakType SpeakType;
//
//	UPROPERTY()
//		TArray<int32> NextDialogues;
//
//	UPROPERTY()
//		class UTexture2D* BG;
//};

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
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dialogue)
	TArray<UDialogueNode*> Data;
};
