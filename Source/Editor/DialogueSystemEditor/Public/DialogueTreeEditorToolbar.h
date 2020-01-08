// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FDialogueTreeGraphEditor;
class FExtender;
class FToolBarBuilder;
/**
 * @author ∑…ŒË«·—Ô
 */
class DIALOGUESYSTEMEDITOR_API FDialogueTreeEditorToolbar : public TSharedFromThis<FDialogueTreeEditorToolbar>
{
public:
	FDialogueTreeEditorToolbar(TSharedPtr<FDialogueTreeGraphEditor> InDialogueTreeGraphEditor) :DialogueTreeEditor(InDialogueTreeGraphEditor) {};

	void AddModesToolbar(TSharedPtr<FExtender> Extender);
	void AddDialogueTreeToolbar(TSharedPtr<FExtender> Extender);
private:
	void FillModesToolbar(FToolBarBuilder& ToolbarBuilder);
	void FillDialogueTreeToolbar(FToolBarBuilder& ToolbarBuilder);
protected:
	TWeakPtr<FDialogueTreeGraphEditor> DialogueTreeEditor;
};
