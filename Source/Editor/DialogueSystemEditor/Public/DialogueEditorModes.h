// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "Framework/Docking/TabManager.h"

/**
 * 
 */
class DIALOGUESYSTEMEDITOR_API FDialogueEditorApplicationMode : public FApplicationMode
{
public:
	FDialogueEditorApplicationMode(TSharedPtr<class FDialogueEditor> InDialogueEditor);
	virtual void RegisterTabFactories(TSharedPtr< class FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
protected:
	TWeakPtr<class FDialogueEditor> DialogueTreeEditor;
	FWorkflowAllowedTabSet DialogueTreeEditorTabFactories;
};