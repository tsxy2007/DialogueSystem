// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Styling/SlateColor.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SNodePanel.h"
#include "DialogueTreeEditorTypes.h"
#include "SGraphNode.h"
#include "SGraphPin.h"
#include "SGraphNodeDT.h"

class SHorizontalBox;
class SToolTip;
class SVerticalBox;
//class SGraphNodeDT;
/**
 * 
 */
class DIALOGUESYSTEMEDITOR_API SGraphNode_DialogueTree : public SGraphNodeDT
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_DialogueTree){}
	SLATE_END_ARGS()

public:

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, SGraphNodeDT*InNode);

	void Construct(const FArguments & InArgs, UDTGraphNode * InGraphNode);

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	virtual void GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const override;
	virtual TArray<FOverlayWidgetInfo> GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const override;
	virtual TSharedRef<SGraphNode> GetNodeUnderMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter) override;
	// End of SGraphNode interface

	// handle double click
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;


	// tick this widget.
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	// show red
	EVisibility GetDebuggerSearchFailedMarkerVisility()const;

	FVector2D GetCachedPosition()const { return CachedPosition; }

protected:

	FLinearColor FlashColor;

	float FlashAlpha;
	
	TArray<FNodeBounds> TriggerOffsets;

	FVector2D CachedPosition;

	TArray<TSharedPtr<SGraphNode>> ServicesWidgets;
	TSharedPtr<SVerticalBox> ServicesBox;
	TSharedPtr<SHorizontalBox> OutputPinBox;

	TSharedPtr<SWidget> IndexOverlay;

	TSharedPtr<SBorder> NodeBody;

	FSlateColor GetBorderBackgroundColor() const;
	FSlateColor GetBackgroundColor()const;

	virtual const FSlateBrush* GetNameIcon() const override;
	virtual EVisibility GetBlueprintIconVisibility()const;

	EVisibility GetIndexVisibility()const;

	FText GetIndexText() const;

	FText GetIndexTooltipText()const;

	FSlateColor GetIndexColor(bool bHovered) const;
	void OnIndexHoverStateChanged(bool bHovered);

	FText GetPinTooltip(UEdGraphPin* GraphPinObj) const;
};
