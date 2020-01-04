// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeFactory.h"
#include "DialogueTree.h"

#define LOCTEXT_NAMESPACE "DialogueTreeFactory"
UDialogueTreeFactory::UDialogueTreeFactory(const FObjectInitializer& Object)
	:Super(Object)
{
	SupportedClass = UDialogueTree::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

bool UDialogueTreeFactory::CanCreateNew() const
{
	return true;
}

UObject*  UDialogueTreeFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(Class->IsChildOf(UDialogueTree::StaticClass()));
	return NewObject<UDialogueTree>(InParent, Class, Name, Flags);;
}

#undef LOCTEXT_NAMESPACE