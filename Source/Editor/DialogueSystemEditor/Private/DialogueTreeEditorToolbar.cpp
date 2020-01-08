// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeEditorToolbar.h"
#include "MultiBoxExtender.h"
#include "DialogueTreeGraphEditor.h"
#include "SModeWidget.h"
#include "SSpacer.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "DialogueTreeEditorToolbar"
void FDialogueTreeEditorToolbar::AddModesToolbar(TSharedPtr<FExtender> Extender)
{
	check(DialogueTreeEditor.IsValid());
	TSharedPtr<FDialogueTreeGraphEditor> DialogueTreeGraphEditorPtr = DialogueTreeEditor.Pin();
	Extender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		DialogueTreeGraphEditorPtr->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FDialogueTreeEditorToolbar::FillModesToolbar)
	);
}

void FDialogueTreeEditorToolbar::AddDialogueTreeToolbar(TSharedPtr<FExtender> Extender)
{
	check(DialogueTreeEditor.IsValid());
	TSharedPtr<FDialogueTreeGraphEditor> DialogueTreeEditorPtr = DialogueTreeEditor.Pin();
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		DialogueTreeEditorPtr->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FDialogueTreeEditorToolbar::FillDialogueTreeToolbar)
	);
	DialogueTreeEditorPtr->AddToolbarExtender(ToolbarExtender);
}

void FDialogueTreeEditorToolbar::FillModesToolbar(FToolBarBuilder & ToolbarBuilder)
{
	check(DialogueTreeEditor.IsValid());
	TSharedPtr<FDialogueTreeGraphEditor> DialogueTreeEditorPtr = DialogueTreeEditor.Pin();
	TAttribute<FName> GetActiveMode(DialogueTreeEditorPtr.ToSharedRef(), &FDialogueTreeGraphEditor::GetCurrentMode);
	FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(DialogueTreeEditorPtr.ToSharedRef(), &FDialogueTreeGraphEditor::SetCurrentMode);
	DialogueTreeEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));
	DialogueTreeEditorPtr->AddToolbarWidget(
		SNew(SModeWidget, FDialogueTreeGraphEditor::GetLocalizeMode(FDialogueTreeGraphEditor::DialogueTreeMode), FDialogueTreeGraphEditor::DialogueTreeMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(DialogueTreeEditorPtr.Get(), &FDialogueTreeGraphEditor::CanAccessDialogueTreeMode)
		.ToolTipText(LOCTEXT("DialogueTreeModeButtonTooltip", "Swtich to Dialogue Tree mode"))
		.IconImage(FEditorStyle::GetBrush("BTEditor.SwitchToBehaviorTreeMode"))
		.SmallIconImage(FEditorStyle::GetBrush("BTEditor.SwitchToBehaviorTreeMode.Small"))
	);

	DialogueTreeEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.f, 1.f)));
}

void FDialogueTreeEditorToolbar::FillDialogueTreeToolbar(FToolBarBuilder & ToolbarBuilder)
{
	check(DialogueTreeEditor.IsValid());
	TSharedPtr<FDialogueTreeGraphEditor> DialogueTreeEditorPtr = DialogueTreeEditor.Pin();
	if (DialogueTreeEditorPtr->GetCurrentMode() == FDialogueTreeGraphEditor::DialogueTreeMode)
	{
		// TODO: Commands
		//ToolbarBuilder.BeginSection("DialogueTree");
		//{
		//	//ToolbarBuilder.AddToolBarButton(FBTCommonCommands::Get().NewBlackboard);
		//}
		//ToolbarBuilder.EndSection();
	}
}

#undef LOCTEXT_NAMESPACE