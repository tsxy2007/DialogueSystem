// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeEditorToolbar.h"
#include "Misc/Attribute.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Widgets/SWidget.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Widgets/Input/SComboButton.h"
#include "EditorStyleSet.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "DialogueEditor.h"
#include "WorkflowOrientedApp/SModeWidget.h"
#include "DialogueTreeEditorCommands.h"

#define LOCTEXT_NAMESPACE "DialogueTreeEditorToolbar"

class SDialogueTreeModeSeparator : public SBorder
{
public:
	SLATE_BEGIN_ARGS(SDialogueTreeModeSeparator){}
	SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs)
	{
		SBorder::Construct(
			SBorder::FArguments()
			.BorderImage(FEditorStyle::GetBrush("BlueprintEditor.PipelineSeparator"))
			.Padding(0.0f)
		);
	}

	virtual FVector2D ComputeDesiredSize(float) const override
	{
		const float Height = 20.f;
		const float Thickness = 16.f;
		return FVector2D(Thickness, Height);
	}
};

FDialogueTreeEditorToolbar::FDialogueTreeEditorToolbar(TSharedPtr<FDialogueEditor> InDialogueTreeEditor)
	:DialogueTreeEditor(InDialogueTreeEditor)
{

}

void FDialogueTreeEditorToolbar::AddModesToolbar(TSharedPtr<FExtender> Extender)
{
	check(DialogueTreeEditor.IsValid());
	TSharedPtr<FDialogueEditor> DialogueTreeEditorPtr = DialogueTreeEditor.Pin();
	

	Extender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		DialogueTreeEditorPtr->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateRaw(this, &FDialogueTreeEditorToolbar::FillModesToolbar));
}

void FDialogueTreeEditorToolbar::FillModesToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(DialogueTreeEditor.IsValid());
	TSharedPtr<FDialogueEditor> DialogueTreeEditorPtr = DialogueTreeEditor.Pin();

	TAttribute<FName> GetActiveMode(DialogueTreeEditorPtr.ToSharedRef(), &FDialogueEditor::GetCurrentMode);
	FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(DialogueTreeEditorPtr.ToSharedRef(), &FDialogueEditor::SetCurrentMode);

	// Left side padding
	DialogueTreeEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

	DialogueTreeEditorPtr->AddToolbarWidget(
		SNew(SModeWidget, FDialogueEditor::GetLocalizedMode(FDialogueEditor::DialogueMode), FDialogueEditor::DialogueMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(DialogueTreeEditorPtr.Get(), &FDialogueEditor::CanAccessDialogueTreeMode)
		.ToolTipText(LOCTEXT("DialogueTreeModeButtonTooltip", "Switch to Dialogue Tree Mode"))
		.IconImage(FEditorStyle::GetBrush("BTEditor.SwitchToBehaviorTreeMode"))
		.SmallIconImage(FEditorStyle::GetBrush("BTEditor.SwitchToBehaviorTreeMode.Small"))
	);

	DialogueTreeEditorPtr->AddToolbarWidget(SNew(SDialogueTreeModeSeparator));
}

#undef LOCTEXT_NAMESPACE