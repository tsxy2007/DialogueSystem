// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeEditorTabFactories.h"
#include "DialogueTreeEditorTabs.h"
#include "EditorStyleSet.h"
#include "DialogueTreeGraphEditor.h"
#include "SharedPointer.h"
#include "GraphEditor.h"
#include "Engine/Blueprint.h"
#include "EditorStyleSet.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "DialogueTreeEditorFactories"

FDialogueDetailsSummoner::FDialogueDetailsSummoner(TSharedPtr<class FDialogueTreeGraphEditor> InDialogueTreeEditorPtr)
	:FWorkflowTabFactory(FDialogueTreeGraphEditorTabs::GraphDetailsID,InDialogueTreeEditorPtr)
{
	TabLabel = LOCTEXT("DialogueTreeDetailsLabel", "Details");
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Components");

	bIsSingleton = true;
	ViewMenuDescription = LOCTEXT("DialogueTreeDetailsView", "Details");
	ViewMenuTooltip = LOCTEXT("DialogueTreeDetailsView_ToolTip", "Show the details view");
}

TSharedRef<SWidget> FDialogueDetailsSummoner::CreateTabBody(const FWorkflowTabSpawnInfo & Info) const
{
	check(DialogueTreeEditorPtr.IsValid());
	return DialogueTreeEditorPtr.Pin()->SpawnProperties();
}

FText FDialogueDetailsSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo & Info) const
{
	return LOCTEXT("DialogueTreeDetailsTabTooltip", "The Dialogue tree details tab allows editing of the properties of behavior tree nodes");
}

FDialogueTreeSearchSummoner::FDialogueTreeSearchSummoner(TSharedPtr<class FDialogueTreeGraphEditor> InDialogueTreeEditorPtr)
	:FWorkflowTabFactory(FDialogueTreeGraphEditorTabs::SearchID,InDialogueTreeEditorPtr)
	,DialogueTreeEditorPtr(InDialogueTreeEditorPtr)
{
	TabLabel = LOCTEXT("DialogueTreeSearchLabel", "Search");
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.FindResults");
	bIsSingleton = true;
	ViewMenuDescription = LOCTEXT("DialogueTreeSearchView", "Search");
	ViewMenuTooltip = LOCTEXT("DialogueTreeSearchView_ToolTip", "Show the dialogue tree search tab");
}

TSharedRef<SWidget> FDialogueTreeSearchSummoner::CreateTabBody(const FWorkflowTabSpawnInfo & Info) const
{
	check(DialogueTreeEditorPtr.IsValid());
	return DialogueTreeEditorPtr.Pin()->SpawnSearch();
}

FText FDialogueTreeSearchSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo & Info) const
{
	return LOCTEXT("DialogueTreeSearchTabTooltip", "The dialogue tree search tab allows searching within dialogue tree nodes");
}

FDialogueGraphEditorSummoner::FDialogueGraphEditorSummoner(TSharedPtr<class FDialogueTreeGraphEditor> InDialogueTreeEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback)
	:FDocumentTabFactoryForObjects<UEdGraph>(FDialogueTreeGraphEditorTabs::GraphEditorID, InDialogueTreeEditorPtr)
	,DialogueTreeEditorPtr(InDialogueTreeEditorPtr)
	,OnCreateGraphEditorWidget(CreateGraphEditorWidgetCallback)
{
}

void FDialogueGraphEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
	check(DialogueTreeEditorPtr.IsValid());
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	DialogueTreeEditorPtr.Pin()->OnGraphEditorFocused(GraphEditor);
}

void FDialogueGraphEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
{
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	GraphEditor->NotifyGraphChanged();
}

TAttribute<FText> FDialogueGraphEditorSummoner::ConstructTabNameForObject(UEdGraph * DocumentID) const
{
	return TAttribute<FText>(FText::FromString(DocumentID->GetName()));
}

TSharedRef<SWidget> FDialogueGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo & Info, UEdGraph * DocumentID) const
{
	return OnCreateGraphEditorWidget.Execute(DocumentID);
}

const FSlateBrush * FDialogueGraphEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo & Info, UEdGraph * DocumentID) const
{
	return FEditorStyle::GetBrush("NoBrush");
}

void FDialogueGraphEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
	check(DialogueTreeEditorPtr.IsValid());
	check(DialogueTreeEditorPtr.Pin()->GetDialogueTree());
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());

	FVector2D ViewLocation;
	float ZoomAmount;
	GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);
	UEdGraph* Graph = FTabPayload_UObject::CastChecked<UEdGraph>(Payload);
	DialogueTreeEditorPtr.Pin()->GetDialogueTree()->LastEditedDocuments.Add(FEditedDocumentInfo(Graph, ViewLocation, ZoomAmount));
}

#undef LOCTEXT_NAMESPACE