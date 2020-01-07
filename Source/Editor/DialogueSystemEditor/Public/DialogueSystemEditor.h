// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "AssetTypeActions_Base.h"

class FDialogueSystemEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FDialogueSystemEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FDialogueSystemEditorModule>("DialogueSystemEditor");
	}

	TSharedRef<class FDialogueTreeGraphEditor> CreateDialogueSystemEditor(EToolkitMode::Type Mode, TSharedPtr<class IToolkitHost> InEditWithInLevel, UObject* InDialogue);
#if WITH_EDITOR
	virtual EAssetTypeCategories::Type GetDialogueTreeAssetCategoryBit() const { return DialogueTreeAssetCategoryBit; }
protected:
	EAssetTypeCategories::Type DialogueTreeAssetCategoryBit;
#endif

public:
	static const FName DialogueTreeEditorAppIdentifier;
};
