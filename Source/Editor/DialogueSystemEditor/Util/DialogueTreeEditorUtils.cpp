// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeEditorUtils.h"
#include "DTNode.h"
#include "DialogueTreeGraphNode.h"

TArray<UObject*> DialogueTreeEditorUtils::GetSelectionForPropertyEditor(const TSet<UObject*>& InSelection, FPropertySelectionInfo& OutSelectionInfo)
{
	TArray<UObject*> Selection;
	for (UObject* SelectionEntry : InSelection)
	{
		UDialogueTreeGraphNode* Node = Cast<UDialogueTreeGraphNode>(SelectionEntry);
		if (Node)
		{
			Selection.Add(Node->NodeInstance);
		}
	}
	return Selection;
}
