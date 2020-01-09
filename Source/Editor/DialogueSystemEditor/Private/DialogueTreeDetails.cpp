// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeDetails.h"
#include "DialogueTree.h"
#include "SlateOptMacros.h"
#include "AssertionMacros.h"

#define  LOCTEXT_NAMESPACE "DialogueTreeDetails"
TSharedRef<IDetailCustomization> FDialogueTreeDetails::MakeInstance()
{
	return MakeShareable(new FDialogueTreeDetails());
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FDialogueTreeDetails::CustomizeDetails(IDetailLayoutBuilder & DetailBuilder)
{
	DetailLayoutBuilder = &DetailBuilder;
	const TSharedPtr<IPropertyHandle> DataProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UDialogueTree, Data));
	DetailBuilder.HideProperty(DataProperty);

	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory("Dialogue Editor");
	IDetailCategoryBuilder& CurrentNodeCategory = DetailBuilder.EditCategory("Current Node", LOCTEXT("CurrentNode", "Current Node"), ECategoryPriority::Important);

	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	TArray<UObject*> StrongObjects;
	CopyFromWeakArray(StrongObjects, ObjectsBeingCustomized);
	if (StrongObjects.Num() == 0)
	{
		return;
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

bool FDialogueTreeDetails::IsEditorEnabled() const
{
	return false;
}

void FDialogueTreeDetails::TextCommited(const FText & InText, ETextCommit::Type InCommitType, UDialogueTree * Dialogue, int32 ID)
{
}

#undef LOCTEXT_NAMESPACE

