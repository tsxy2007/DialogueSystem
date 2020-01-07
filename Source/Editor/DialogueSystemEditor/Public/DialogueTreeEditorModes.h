// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Docking/TabManager.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "DialogueTreeGraphEditor.h"
/**
 * 
 */

class FDialogueTreeEditorApplicationMode : public FApplicationMode
{
public:
	FDialogueTreeEditorApplicationMode(TSharedPtr<class FDialogueTreeGraphEditor> InDialogueTreeGraphEditor);

	virtual void RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager) override;

	virtual void PreDeactivateMode() override;

	virtual void PostActivateMode() override;

protected:
	TWeakPtr<class FDialogueTreeGraphEditor> DialogueTreeGraphEditor;

	FWorkflowAllowedTabSet DialogueTreeEditorTabFactories;

};
