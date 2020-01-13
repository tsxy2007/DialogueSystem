// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueNode.generated.h"
//
// �Ի�����
UENUM(BlueprintType)
enum class EDialogueSpeakType : uint8
{
	EDT_NPC,	// NPC 
	EDT_My,		// �Լ�˵��
	EDT_MySelect,// �Ҳ����ѡ��ĶԻ�
};

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API UDialogueNode : public UObject
{
	GENERATED_BODY()
	
public:
	UDialogueNode();
public:
	UPROPERTY()
	FText Speaker;

	UPROPERTY()
	FText Content;

	UPROPERTY()
	EDialogueSpeakType SpeakType;

	UPROPERTY()
	TArray<UDialogueNode*> NextDialogues;

	UPROPERTY()
	class UTexture2D* BG;
};