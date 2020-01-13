// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeEditorTabFactories.h"
#include "DialogueTreeEditorTabs.h"
#include "Engine/Blueprint.h"
#include "EditorStyleSet.h"
#include "Widgets/Docking/SDockTab.h"
#define LOCTEXT_NAMESPACE "DialogueTreeEditorFactories"
FDialogueTreeDetailSummoner::FDialogueTreeDetailSummoner(TSharedPtr<class FDialogueEditor> InDialogueEditorPtr)
	:FWorkflowTabFactory(FDialogueTreeEditorTabs::GraphDetailsID,InDialogueEditorPtr)
	,DialogueEditorPtr(InDialogueEditorPtr)
{
	TabLabel = LOCTEXT("DialogueTreeDetailsLabel","Details");
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Components");
	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("DialogueTreeDetailsView", "Details");
	ViewMenuTooltip = LOCTEXT("DialogueTreeDetailsView_ToolTip", "Show the details view");
}

TSharedRef<SWidget> FDialogueTreeDetailSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	check(DialogueEditorPtr.IsValid());
	return DialogueEditorPtr.Pin()->SpawnProperties();
}

FText FDialogueTreeDetailSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("DialogueTreeDetailsTabToolTip", "The Dialogue tree details tab allows editing of the properties of Dialogue tree nodes");
}

FDialogueTreeSearchSummoner::FDialogueTreeSearchSummoner(TSharedPtr<class FDialogueEditor> InDialogueEditorPtr)
	:FWorkflowTabFactory(FDialogueTreeEditorTabs::SearchID,InDialogueEditorPtr)
	,DialogueEditorPtr(InDialogueEditorPtr)
{
	TabLabel = LOCTEXT("BehaviorTreeSearchLabel", "Search");
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.FindResults");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("DialogueTreeSearchView", "Search");
	ViewMenuTooltip = LOCTEXT("DialogueTreeSearchView_ToolTip", "Show the Dialogue tree search tab");
}

TSharedRef<SWidget> FDialogueTreeSearchSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return DialogueEditorPtr.Pin()->SpawnSearch();
}

FText FDialogueTreeSearchSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("DialogueTreeSearchTabTooltip", "The Dialogue tree search tab allows searching within Dialogue tree nodes");
}

FDialogueGraphEditorSummoner::FDialogueGraphEditorSummoner(TSharedPtr<class FDialogueEditor> InDialogueTreeEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback)
	:FDocumentTabFactoryForObjects<UEdGraph>(FDialogueTreeEditorTabs::GraphEditorID, InDialogueTreeEditorPtr)
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

TAttribute<FText> FDialogueGraphEditorSummoner::ConstructTabNameForObject(UEdGraph* DocumentID) const
{
	return TAttribute<FText>(FText::FromString(DocumentID->GetName()));
}

TSharedRef<SWidget> FDialogueGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	return OnCreateGraphEditorWidget.Execute(DocumentID);
}

const FSlateBrush* FDialogueGraphEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	return FEditorStyle::GetBrush("NoBrush");
}

void FDialogueGraphEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
	check(DialogueTreeEditorPtr.IsValid());
	check(DialogueTreeEditorPtr.Pin()->GetDialogueTree());

	TSharedPtr<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	FVector2D ViewLocation;
	float ZoomAmount;
	GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);
	UEdGraph* Graph = FTabPayload_UObject::CastChecked<UEdGraph>(Payload);
	DialogueTreeEditorPtr.Pin()->GetDialogueTree()->LastEditedDocuments.Add(FEditedDocumentInfo(Graph, ViewLocation, ZoomAmount));
}
#undef LOCTEXT_NAMESPACE