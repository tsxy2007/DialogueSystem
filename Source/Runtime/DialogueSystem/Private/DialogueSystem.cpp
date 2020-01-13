// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DialogueSystem.h"

IMPLEMENT_GAME_MODULE(FDialogueSystemModule, DialogueSystem);
DEFINE_LOG_CATEGORY(LogDialogue);

void FDialogueSystemModule::StartupModule()
{
	
}

void FDialogueSystemModule::ShutdownModule()
{
	
}

bool FDialogueSystemModule::IsGameModule() const
{
	return true;
}