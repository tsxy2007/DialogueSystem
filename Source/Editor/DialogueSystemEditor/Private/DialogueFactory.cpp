// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueFactory.h"
#include "Dialogue.h"

#define LOCTEXT_NAMESPACE "DialogueFactory"
UDialogueFactory::UDialogueFactory(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SupportedClass = UDialogue::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UDialogueFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(Class->IsChildOf(UDialogue::StaticClass()));
	return NewObject<UDialogue>(InParent, Class, Name, Flags);;
}

bool UDialogueFactory::CanCreateNew() const
{
	return true;
}
FText UDialogueFactory::GetDisplayName() const
{
	return LOCTEXT("DialogueFactoryDescription", "Dialogue");;
}

#undef LOCTEXT_NAMESPACE
