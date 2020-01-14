// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UDTNode;
class UDTGraphNode;
/**
 * 
 */
namespace DialogueTreeEditorUtils
{
	struct FPropertySelectionInfo 
	{
		FPropertySelectionInfo()
			:FoundGraphNode(nullptr)
			, FoundNode(nullptr)
			, bInjectedNode(false)
			, bRootLevelNode(false)
		{

		}
		UDTGraphNode* FoundGraphNode;
		UDTNode*  FoundNode;
		bool bInjectedNode;
		bool bRootLevelNode;
	};
	TArray<UObject*> GetSelectionForPropertyEditor(const TSet<UObject*>& InSelection, FPropertySelectionInfo& OutSelectionInfo);

}
