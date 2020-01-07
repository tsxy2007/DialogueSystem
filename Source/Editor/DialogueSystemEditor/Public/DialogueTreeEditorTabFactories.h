// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowTabFactory.h"
#include "WorkflowUObjectDocuments.h"
#include "EdGraph/EdGraph.h"

/**
 * @auth 飞舞轻扬
 */
struct FDialogueDetailsSummoner : public FWorkflowTabFactory
{
public:
	FDialogueDetailsSummoner(TSharedPtr<class FDialogueTreeGraphEditor> InDialogueTreeEditorPtr);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
protected:
	TWeakPtr<class FDialogueTreeGraphEditor> DialogueTreeEditorPtr;
};

struct FDialogueTreeSearchSummoner : public FWorkflowTabFactory
{
public:
	FDialogueTreeSearchSummoner(TSharedPtr<class FDialogueTreeGraphEditor> InDialogueTreeEditorPtr);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
protected:
	TWeakPtr<class FDialogueTreeGraphEditor> DialogueTreeEditorPtr;
};

struct FDialogueGraphEditorSummoner : public FDocumentTabFactoryForObjects<UEdGraph>
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FOnCreateGraphEditorWidget, UEdGraph*);
public:
	FDialogueGraphEditorSummoner(TSharedPtr<class FDialogueTreeGraphEditor> InBehaviorTreeEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback);
	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override;
	virtual void OnTabRefreshed(TSharedPtr<SDockTab> Tab) const override;
protected:
	virtual TAttribute<FText> ConstructTabNameForObject(UEdGraph* DocumentID) const override;
	virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual const FSlateBrush* GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual void SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const  override;
protected:
	TWeakPtr<class FDialogueTreeGraphEditor> DialogueTreeEditorPtr;
	FOnCreateGraphEditorWidget OnCreateGraphEditorWidget;
};