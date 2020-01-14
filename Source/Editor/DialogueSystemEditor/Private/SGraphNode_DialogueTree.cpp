// Fill out your copyright notice in the Description page of Project Settings.


#include "SGraphNode_DialogueTree.h"
#include "Types/SlateStructs.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SToolTip.h"
#include "DTNode.h"
#include "Editor.h"
#include "GraphEditorSettings.h"
#include "SGraphPanel.h"
#include "SCommentBubble.h"
#include "SGraphPreviewer.h"
#include "NodeFactory.h"
#include "IDocumentation.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SLevelOfDetailBranchNode.h"
#include "DialogueTreeColors.h"
#include "DTGraphNode.h"
#include "DialogueGraphNode_Root.h"

#include "DialogueEdGraph.h"

#define LOCTEXT_NAMESPACE "DialogueTreeEditor"

namespace
{
	static const bool bShowExecutionIndexInEditorMode = true;
}

static UDialogueTreeGraphNode* GetParentNode(UEdGraphNode* GraphNode)
{
	UDialogueTreeGraphNode* DTGraphNode = Cast<UDialogueTreeGraphNode>(GraphNode);
	if (DTGraphNode->ParentNode != nullptr)
	{
		DTGraphNode = Cast<UDialogueTreeGraphNode>(DTGraphNode->ParentNode);
	}
	UEdGraphPin* MyInputPin = DTGraphNode->GetInputPin();
	UEdGraphPin* MyParentOutputPin = nullptr;
	if (MyInputPin != nullptr && MyInputPin->LinkedTo.Num() > 0)
	{
		MyParentOutputPin = MyInputPin->LinkedTo[0];
		if (MyParentOutputPin != nullptr)
		{
			if (MyParentOutputPin->GetOwningNode() != nullptr)
			{
				return CastChecked<UDialogueTreeGraphNode>(MyParentOutputPin->GetOwningNode());
			}
		}
	}
	return nullptr;
}

class SDialogueTreePin : public SGraphPinDT
{
	SLATE_BEGIN_ARGS(SDialogueTreePin) {}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
protected:
	virtual FSlateColor GetPinColor() const override;
};

void SDialogueTreePin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPinDT::Construct(SGraphPinDT::FArguments(), InPin);
}

FSlateColor SDialogueTreePin::GetPinColor() const
{
	return GraphPinObj->bIsDiffing ? DialogueTreeColors::Pin::Diff : IsHovered() ? DialogueTreeColors::Pin::Hover :
		(GraphPinObj->PinType.PinCategory == UDialogueTreeEditorTypes::PinCategory_SingleComposite) ? DialogueTreeColors::Pin::CompositeOnly :
		(GraphPinObj->PinType.PinCategory == UDialogueTreeEditorTypes::PinCategory_SingleTask) ? DialogueTreeColors::Pin::TaskOnly :
		(GraphPinObj->PinType.PinCategory == UDialogueTreeEditorTypes::PinCategory_SingleNode) ? DialogueTreeColors::Pin::SingleNode :
		DialogueTreeColors::Pin::Default;
}

class SDialogueTreeIndex : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnHoverStateChanged, bool);
	DECLARE_DELEGATE_RetVal_OneParam(FSlateColor, FOnGetIndexColor, bool /* bHovered */);

	SLATE_BEGIN_ARGS(SDialogueTreeIndex){}
		SLATE_ATTRIBUTE(FText,Text)
		SLATE_EVENT(FOnHoverStateChanged,OnHoverStateChanged)
		SLATE_EVENT(FOnGetIndexColor,OnGetIndexColor)
	SLATE_END_ARGS()
public:
		void Construct(const FArguments& InArgs)
		{
			OnHoverStateChangedEvent = InArgs._OnHoverStateChanged;
			OnGetIndexColorEvent = InArgs._OnGetIndexColor;
			const FSlateBrush* IndexBrush = FEditorStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Index"));

			ChildSlot
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				[
					SNew(SBox)
					.WidthOverride(IndexBrush->ImageSize.X)
					.HeightOverride(IndexBrush->ImageSize.Y)
				]
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SBorder)
					.BorderImage(IndexBrush)
					.BorderBackgroundColor(this, &SDialogueTreeIndex::GetColor)
					.Padding(FMargin(4.0f, 0.0f, 4.0f, 1.0f))
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.Text(InArgs._Text)
						.Font(FEditorStyle::GetFontStyle("BTEditor.Graph.BTNode.IndexText"))
					]
				]
			];
		}

		virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override 
		{
			OnHoverStateChangedEvent.ExecuteIfBound(true);
			SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);
		}

		virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override
		{
			OnHoverStateChangedEvent.ExecuteIfBound(false);
			SCompoundWidget::OnMouseLeave(MouseEvent);
		}

		FSlateColor GetColor()const
		{
			return OnGetIndexColorEvent.Execute(IsHovered());
		}
private:
	FOnHoverStateChanged OnHoverStateChangedEvent;
	FOnGetIndexColor OnGetIndexColorEvent;
};

void SGraphNode_DialogueTree::Construct(const FArguments& InArgs,UDTGraphNode* InGraphNode)
{
	DebuggerStateDuration = 0.0f;
	DebuggerStateCounter = INDEX_NONE;
	bSupperessDebuggerTriggers = false;
	SGraphNodeDT::Construct(SGraphNodeDT::FArguments(), InGraphNode);
}


void SGraphNode_DialogueTree::UpdateGraphNode()
{
	bDragMarkerVisible = false;
	InputPins.Empty();
	OutputPins.Empty();

	if (ServicesBox.IsValid())
	{
		ServicesBox->ClearChildren();
	}
	else
	{
		SAssignNew(ServicesBox, SVerticalBox);
	}
	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	ServicesWidgets.Reset();
	SubNodes.Reset();
	OutputPins.Reset();

	UDTGraphNode* DTNode = Cast<UDTGraphNode>(GraphNode);
	if (DTNode)
	{
		
	}

	TSharedPtr<SNodeTitle> NodeTile = SNew(SNodeTitle, GraphNode);

	TWeakPtr<SNodeTitle> WeakNodeTitle = NodeTile;
	auto GetNodeTitlePlaceHolderWidth = [WeakNodeTitle]()->FOptionalSize
	{
		TSharedPtr<SNodeTitle> NodeTitlePin = WeakNodeTitle.Pin();
		const float DesiredWidth = (NodeTitlePin.IsValid()) ? NodeTitlePin->GetTitleSize().X : 0.f;
		return FMath::Max(75.f, DesiredWidth);
	};

	auto GetNodeTitlePlaceHolderHeight = [WeakNodeTitle]()->FOptionalSize
	{
		TSharedPtr<SNodeTitle> NodeTitlePin = WeakNodeTitle.Pin();
		const float DesiredHeight = (NodeTitlePin.IsValid()) ? NodeTitlePin->GetTitleSize().Y : 0.f;
		return FMath::Max(22.f, DesiredHeight);
	};

	const FMargin NodePadding = FMargin(8.f);
	
	IndexOverlay = SNew(SDialogueTreeIndex)
		.ToolTipText(this, &SGraphNode_DialogueTree::GetIndexTooltipText)
		.Visibility(this, &SGraphNode_DialogueTree::GetIndexVisibility)
		.Text(this, &SGraphNode_DialogueTree::GetIndexText)
		.OnHoverStateChanged(this,&SGraphNode_DialogueTree::OnIndexHoverStateChanged)
		.OnGetIndexColor(this,&SGraphNode_DialogueTree::GetIndexColor);
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(this, &SGraphNode_DialogueTree::GetBorderBackgroundColor)
			.OnMouseButtonDown(this,&SGraphNode_DialogueTree::OnMouseDown)
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.MinDesiredHeight(NodePadding.Top)
						[
							SAssignNew(LeftNodeBox,SVerticalBox)
						]
					]
					// STATE NAME AREA
					+SVerticalBox::Slot()
					.Padding(FMargin(NodePadding.Left,0.f,NodePadding.Right,0.f))
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SAssignNew(NodeBody,SBorder)
							.BorderImage(FEditorStyle::GetBrush("BTEditor.Graph.BTNode.Body"))
							.BorderBackgroundColor(this, &SGraphNode_DialogueTree::GetBackgroundColor)
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Center)
							.Visibility(EVisibility::SelfHitTestInvisible)
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							ServicesBox.ToSharedRef()
						]
					]
					// OUTPUT PIN AREA
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.MinDesiredHeight(NodePadding.Bottom)
						[
							SAssignNew(RightNodeBox,SVerticalBox)
							+SVerticalBox::Slot()
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							.Padding(20.f,0.f)
							.FillHeight(1.f)
							[
								SAssignNew(OutputPinBox,SHorizontalBox)
							]
						]
					]
				]
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SNew(SBorder)
					.BorderBackgroundColor(DialogueTreeColors::Action::DragMarker)
					.ColorAndOpacity(DialogueTreeColors::Action::DragMarker)
					.BorderImage(FEditorStyle::GetBrush("BTEditor.Graph.BTNode.Body"))
					.Visibility(this, &SGraphNode_DialogueTree::GetDragOverMarkerVisibility)
					[
						SNew(SBox)
						.HeightOverride(4)
					]
				]
				// Blueprint indicator overlay
				+SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				[
					SNew(SImage)
					.Image(FEditorStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Blueprint")))
					.Visibility(this, &SGraphNode_DialogueTree::GetBlueprintIconVisibility)
				]
			]
		];

	// Create comment bubble
	TSharedPtr<SCommentBubble> CommentBubble;
	const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;

	SAssignNew(CommentBubble, SCommentBubble)
		.GraphNode(GraphNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
		.ColorAndOpacity(CommentColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

	GetOrAddSlot(ENodeZone::TopCenter)
		.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
		.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
		.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
		.VAlign(VAlign_Top)
		[
			CommentBubble.ToSharedRef()
		];
	
	CreatePinWidgets();
}

void SGraphNode_DialogueTree::CreatePinWidgets()
{
	UDTGraphNode* StateNode = CastChecked<UDTGraphNode>(GraphNode);
	for (int32 PinIdx = 0;PinIdx < StateNode->Pins.Num();PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SDialogueTreePin, MyPin)
				.ToolTipText(this, &SGraphNode_DialogueTree::GetPinTooltip, MyPin);
			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SGraphNode_DialogueTree::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));
	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvanceParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvanceParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}
	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		LeftNodeBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.f)
			.Padding(20.f, 0.f)
			[
				PinToAdd
			];
		InputPins.Add(PinToAdd);
	}
	else
	{
		const bool bIsSingleTaskPin = PinObj && (PinObj->PinType.PinCategory == UDialogueTreeEditorTypes::PinCategory_SingleTask);
		if (bIsSingleTaskPin)
		{
			OutputPinBox->AddSlot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.FillWidth(0.4f)
				.Padding(0, 0, 20.0f, 0)
				[
					PinToAdd
				];
		}
		else
		{
			OutputPinBox->AddSlot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.FillWidth(1.0f)
				[
					PinToAdd
				];
		}
		OutputPins.Add(PinToAdd);
	}
}

TSharedPtr<SToolTip> SGraphNode_DialogueTree::GetComplexTooltip()
{
	return IDocumentation::Get()->CreateToolTip(TAttribute<FText>(this,&SGraphNode::GetNodeTooltip),nullptr,GraphNode->GetDocumentationLink(),GraphNode->GetDocumentationExcerptName());
}

void SGraphNode_DialogueTree::GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const
{
	UDialogueTreeGraphNode* DTNode = Cast<UDialogueTreeGraphNode>(GraphNode);
	if (DTNode == nullptr)
	{
		return;
	}
	//TODO: Debug
}

TArray<FOverlayWidgetInfo> SGraphNode_DialogueTree::GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const
{
	TArray<FOverlayWidgetInfo> Widgets;
	check(NodeBody.IsValid());
	check(IndexOverlay.IsValid());

	FVector2D Origin(0.f, 0.f);

	//build overlay for service sub-nodes

	FOverlayWidgetInfo Overlay(IndexOverlay);
	Overlay.OverlayOffset = FVector2D(WidgetSize.X - (IndexOverlay->GetDesiredSize().X*0.5f), Origin.Y);
	Widgets.Add(Overlay);
	Origin.Y += NodeBody->GetDesiredSize().Y;
	for (const auto& ServiceWidget : ServicesWidgets)
	{
		TArray<FOverlayWidgetInfo> OverlayWidgets = ServiceWidget->GetOverlayWidgets(bSelected, WidgetSize);
		for (auto& OverlayWidget : OverlayWidgets)
		{
			OverlayWidget.OverlayOffset.Y += Origin.Y;
		}
		Widgets.Append(OverlayWidgets);
		Origin.Y += ServiceWidget->GetDesiredSize().Y;
	}
	return Widgets;
}

TSharedRef<SGraphNode> SGraphNode_DialogueTree::GetNodeUnderMouse(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	TSharedPtr<SGraphNode> SubNode = GetSubNodeUnderCursor(MyGeometry, MouseEvent);
	return SubNode.IsValid()?SubNode.ToSharedRef():StaticCastSharedRef<SGraphNode>(AsShared());
}

void SGraphNode_DialogueTree::MoveTo(const FVector2D & NewPosition, FNodeSet & NodeFilter)
{
	SGraphNodeDT::MoveTo(NewPosition, NodeFilter);
	UDTGraphNode* DTGraphNode = Cast<UDTGraphNode>(GraphNode);
	if (DTGraphNode && !DTGraphNode->IsSubNode())
	{
		UDialogueEdGraph* STGraph = DTGraphNode->GetSTGraph();
		if (STGraph)
		{
			for (int32 i = 0; i < DTGraphNode->Pins.Num(); i++)
			{
				UEdGraphPin* Pin = DTGraphNode->Pins[i];
				if (Pin	&& Pin->Direction == EGPD_Input && Pin->LinkedTo.Num() == 1)
				{
					UEdGraphPin* ParentPin = Pin->LinkedTo[0];
					if (ParentPin)
					{
						STGraph->RebuildChildOrder(ParentPin->GetOwningNode());
					}
				}
			}
		}
	}
}

FReply SGraphNode_DialogueTree::OnMouseButtonDoubleClick(const FGeometry & InMyGeometry, const FPointerEvent & InMouseEvent)
{
	return FReply::Handled();
}

void SGraphNode_DialogueTree::Tick(const FGeometry & AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
}

void SGraphNode_DialogueTree::AddService(TSharedPtr<SGraphNode> ServiceWidget)
{
	ServicesBox->AddSlot().AutoHeight()
	[
		ServiceWidget.ToSharedRef()
	];
	ServicesWidgets.Add(ServiceWidget);
	AddSubNode(ServiceWidget);
}

EVisibility SGraphNode_DialogueTree::GetDebuggerSearchFailedMarkerVisility() const
{
	return EVisibility();
}

FSlateColor SGraphNode_DialogueTree::GetBorderBackgroundColor() const
{
	UDTGraphNode* DTGraphNode = Cast<UDTGraphNode>(GraphNode);
	UDTGraphNode* STParentNode = DTGraphNode ? Cast<UDTGraphNode>(DTGraphNode->ParentNode) : nullptr;
	const bool bSelectSubNode = STParentNode && GetOwnerPanel()->SelectionManager.SelectedNodes.Contains(GraphNode);
	UDTNode* NodeInstance = DTGraphNode ? Cast<UDTNode>(DTGraphNode->NodeInstance) : nullptr;
	
	const bool bIsConnectedTreeRoot = DTGraphNode && 
		DTGraphNode->IsA<UDialogueGraphNode_Root>() && 
		DTGraphNode->Pins.IsValidIndex(0) &&
		DTGraphNode->Pins[0]->LinkedTo.Num() > 0;
	const bool bIsDisconnected = NodeInstance && NodeInstance->GetExecutionIndex() == MAX_uint16;
	//const bool bIsRootDecorator = DTGraphNode;
	//const bool bIsInjected = DTGraphNode&& DTGraphNode->binje

	return bSelectSubNode ? DialogueTreeColors::NodeBorder::Selected : bIsDisconnected ? DialogueTreeColors::NodeBorder::Root : DialogueTreeColors::NodeBorder::Inactive;
}

FSlateColor SGraphNode_DialogueTree::GetBackgroundColor() const
{
	UDTGraphNode* DTGraphNode = Cast<UDTGraphNode>(GraphNode);
	
	FLinearColor NodeColor = DialogueTreeColors::NodeBody::Default;
	if (DTGraphNode && DTGraphNode->HasErrors())
	{
		NodeColor = DialogueTreeColors::NodeBody::Error;
	}
	else if (Cast<UDialogueGraphNode_Root>(GraphNode) && GraphNode->Pins.IsValidIndex(0)&& GraphNode->Pins[0]->LinkedTo.Num()>0)
	{
		NodeColor = DialogueTreeColors::NodeBody::Root;
	}

	return (FlashAlpha > 0.f) ? FMath::Lerp(NodeColor, FlashColor, FlashAlpha) : NodeColor;
}

const FSlateBrush * SGraphNode_DialogueTree::GetNameIcon() const
{
	UDTGraphNode* DTGraphNode = Cast<UDTGraphNode>(GraphNode);
	return /*DTGraphNode != nullptr ? FEditorStyle::GetBrush(DTGraphNode->GetNameIcon()) : */FEditorStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}

EVisibility SGraphNode_DialogueTree::GetBlueprintIconVisibility() const
{
	return EVisibility();
}

EVisibility SGraphNode_DialogueTree::GetIndexVisibility() const
{
	// always hide the index on the root node
	if (GraphNode->IsA(UDialogueGraphNode_Root::StaticClass()))
	{
		return EVisibility::Collapsed;
	}
	UDTGraphNode* StateNode = CastChecked<UDTGraphNode>(GraphNode);
	UEdGraphPin* MyInputPin = StateNode->GetInputPin();
	UEdGraphPin* MyParentOutputPin = nullptr;
	if (MyInputPin != nullptr && MyInputPin->LinkedTo.Num() > 0)
	{
		MyParentOutputPin = MyInputPin->LinkedTo[0];
	}

	CA_SUPPRESS(6235);
	const bool bCanShowIndex = (bShowExecutionIndexInEditorMode || GEditor->bIsSimulatingInEditor || GEditor->PlayWorld != NULL) || (MyParentOutputPin && MyParentOutputPin->LinkedTo.Num() > 1);

	TSharedPtr<SGraphPanel> MyOwnerPanel = GetOwnerPanel();
	return (bCanShowIndex && (!MyOwnerPanel.IsValid() || MyOwnerPanel->GetCurrentLOD() > EGraphRenderingLOD::LowDetail)) ? EVisibility::Visible : EVisibility::Collapsed;
}

FText SGraphNode_DialogueTree::GetIndexText() const
{
	UDTGraphNode* StateNode = CastChecked<UDTGraphNode>(GraphNode);
	UEdGraphPin* MyInputPin = StateNode->GetInputPin();
	UEdGraphPin* MyParentOutputPin = nullptr;
	if (MyInputPin != nullptr && MyInputPin->LinkedTo.Num() > 0)
	{
		MyParentOutputPin = MyInputPin->LinkedTo[0];
	}
	int32 Index = 0;
	CA_SUPPRESS(6235);
	if (bShowExecutionIndexInEditorMode || GEditor->bIsSimulateInEditorQueued || GEditor->PlayWorld != nullptr)
	{
		UDTNode* DTNode = Cast<UDTNode>(StateNode->NodeInstance);
		Index = (DTNode&&DTNode->GetExecutionIndex() < 0xfff) ? DTNode->GetExecutionIndex() : -1;
	}
	else
	{
		if (MyParentOutputPin != nullptr)
		{
			for (Index = 0 ;Index<MyParentOutputPin->LinkedTo.Num();Index++)
			{
				if (MyParentOutputPin->LinkedTo[Index] == MyInputPin)
				{
					break;
				}
			}
		}
	}
	return FText::AsNumber(Index);
}

FText SGraphNode_DialogueTree::GetIndexTooltipText() const
{
	CA_SUPPRESS(6235);
	if (bShowExecutionIndexInEditorMode|| GEditor->bIsSimulatingInEditor||GEditor->PlayWorld != nullptr)
	{
		return LOCTEXT("ExecutionIndexToolTip", "Execution Index: this shows the order in which nodes are executed.");
	}
	else
	{
		return LOCTEXT("ChildIndexToolTip", "Child index: this shows the order in which child nodes are executed.");
	}
	return FText();
}

FSlateColor SGraphNode_DialogueTree::GetIndexColor(bool bHovered) const
{
	UDialogueTreeGraphNode* ParentNode = GetParentNode(GraphNode);
	const bool bHighlightHover = bHovered || (ParentNode && ParentNode->bHighlightChildNodeIndices);
	static const FName HoveredColor("BTEditor.Graph.BTNode.Index.HoveredColor");
	static const FName DefaultColor("BTEditor.Graph.BTNode.Index.Color");

	return bHighlightHover ? FEditorStyle::Get().GetSlateColor(HoveredColor) : FEditorStyle::Get().GetSlateColor(DefaultColor);
}

void SGraphNode_DialogueTree::OnIndexHoverStateChanged(bool bHovered)
{
}

FText SGraphNode_DialogueTree::GetPinTooltip(UEdGraphPin * GraphPinObj) const
{
	return FText();
}

#undef LOCTEXT_NAMESPACE