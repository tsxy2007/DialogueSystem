// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
class FDialogueEditor;
class FExtender;
class FToolBarBuilder;

/**
 * 
 */
class FDialogueTreeEditorToolbar : public TSharedFromThis<FDialogueTreeEditorToolbar>
{
public:
	FDialogueTreeEditorToolbar(TSharedPtr<FDialogueEditor> InDialogueTreeEditor);
	
	void AddModesToolbar(TSharedPtr<FExtender> Extender);
	void AddDebuggerToolBar(TSharedPtr<FExtender> Extender);
	void AddDialogueTreeToolBar(TSharedPtr<FExtender> Extender);
public:
	void FillModesToolbar(FToolBarBuilder& ToolbarBuilder);
	void FillDebuggerToolbar(FToolBarBuilder& ToolbarBuilder);
	void FillDialogueTreeToolbar(FToolBarBuilder& ToolbarBuilder);
protected:
	TWeakPtr<FDialogueEditor> DialogueTreeEditor;
};
