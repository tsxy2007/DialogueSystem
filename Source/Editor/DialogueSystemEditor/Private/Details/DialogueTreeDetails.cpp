// Fill out your copyright notice in the Description page of Project Settings.

#include "Details/DialogueTreeDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailPropertyRow.h"
#include "DetailCategoryBuilder.h"
#include "IPropertyUtilities.h"
#include "Layout/Margin.h"
#include "Misc/Attribute.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SlateOptMacros.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Widgets/SWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Input/SComboButton.h"

#define LOCTEXT_NAMESPACE "DialogueTreeDetails"
TSharedRef<IDetailCustomization> FDialogueTreeDetails::MakeInstance()
{
	return MakeShareable(new FDialogueTreeDetails());
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FDialogueTreeDetails::CustomizeDetails(IDetailLayoutBuilder & DetailBuilder)
{
	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory("CategoryName", FText::FromString("Extra Info"), ECategoryPriority::Default);
	MyCategory.AddCustomRow(LOCTEXT("Extra info", "Row header name"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("Extra info", "Custom row header name"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
		]
	.ValueContent().MinDesiredWidth(500)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("Extra info", "Custom row content"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
		];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

bool FDialogueTreeDetails::IsEditorOnlyObject() const
{
	return false;
}

void FDialogueTreeDetails::InitPropertyValues()
{
}

#undef LOCTEXT_NAMESPACE