// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueFactory.h"
#include "DialogueTree.h"

#define LOCTEXT_NAMESPACE "DialogueFactory"
UDialogueFactory::UDialogueFactory(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SupportedClass = UDialogueTree::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UDialogueFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(Class->IsChildOf(UDialogueTree::StaticClass()));
	return NewObject<UDialogueTree>(InParent, Class, Name, Flags);;
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
