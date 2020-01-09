// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "IDetailCustomization.h"
#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"

class IDetailLayoutBuilder;
class IPropertyHandle;
class SWidget;
class UDialogueTree;
/**
 * 
 */
class DIALOGUESYSTEMEDITOR_API FDialogueTreeDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	bool IsEditorEnabled() const;

	void TextCommited(const FText& InText, ETextCommit::Type InCommitType, UDialogueTree* Dialogue, int32 ID);
private:
	FReply RefreshOnClicked();

	IDetailLayoutBuilder* DetailLayoutBuilder;
};
