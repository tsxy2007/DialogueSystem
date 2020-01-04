// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions_Base.h"
/**
 * 
 */
class DIALOGUESYSTEMEDITOR_API FAssetTypeActions_DialogueTree : public FAssetTypeActions_Base
{
public:
	virtual FText GetName()const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_DialogueTree", "DialogueTree"); }
	virtual FColor GetTypeColor()const override { return FColor(149, 70, 255); }
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject *>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override;
	virtual void PerformAssetDiff(UObject* OldAsset, UObject* NewAsset, const struct FRevisionInfo& OldRevision, const struct FRevisionInfo& NewRevision) const override;
private:
	void OpenInDefaults(class UDialogueTree* OldDialogueTree, class UDialogueTree* NewDialogueTree) const;
};
