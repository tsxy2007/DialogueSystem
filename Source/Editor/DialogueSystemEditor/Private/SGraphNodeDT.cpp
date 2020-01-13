// Fill out your copyright notice in the Description page of Project Settings.


#include "SGraphNodeDT.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Editor.h"
#include "DTGraphNode.h"
#include "SGraphPanel.h"
#include "ScopedTransaction.h"
#include "DialogueEdGraph.h"

#define LOCTEXT_NAMESPACE "DialogueGraph"

TSharedRef<FDragDTGraphNode> FDragDTGraphNode::New(const TSharedRef<SGraphPanel>& InGraphPanel, const TSharedRef<SGraphNode>& InDraggedNode)
{
	TSharedRef<FDragDTGraphNode> Operation = MakeShared<FDragDTGraphNode>();
	Operation->StartTime = FPlatformTime::Seconds();
	Operation->GraphPanel = InGraphPanel;
	Operation->DraggedNodes.Add(InDraggedNode);
	Operation->DecoratorAdjust = FSlateApplication::Get().GetCursorSize();
	Operation->Construct();
	return Operation;
}

TSharedRef<FDragDTGraphNode> FDragDTGraphNode::New(const TSharedRef<SGraphPanel>& InGraphPanel, const TArray<TSharedRef<SGraphNode>>& InDraggedNode)
{
	TSharedRef<FDragDTGraphNode> Operation = MakeShareable(new FDragDTGraphNode);
	Operation->StartTime = FPlatformTime::Seconds();
	Operation->GraphPanel = InGraphPanel;
	Operation->DraggedNodes.Append(InDraggedNode);
	Operation->DecoratorAdjust = FSlateApplication::Get().GetCursorSize();
	Operation->Construct();
	return Operation;
}

UDTGraphNode * FDragDTGraphNode::GetDropTargetNode() const
{
	return Cast<UDTGraphNode>(GetHoveredNode());
}

// SGraphNodeDT begin----------------------------------------------------------------------------

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SGraphNodeDT::Construct(const FArguments& InArgs, UDTGraphNode* InNode)
{
	SetCursor(EMouseCursor::CardinalCross);
	GraphNode = InNode;
	UpdateGraphNode();
	bDragMarkerVisible = false;
}
TSharedPtr<SToolTip> SGraphNodeDT::GetComplexTooltip()
{
	return nullptr;
}

void SGraphNodeDT::OnDragEnter(const FGeometry & MyGeometry, const FDragDropEvent & DragDropEvent)
{
	TSharedPtr<FDragNode> DragConnectionOp = DragDropEvent.GetOperationAs<FDragNode>();
	if (DragConnectionOp.IsValid())
	{
		TSharedPtr<SGraphNode> SubNode = GetSubNodeUnderCursor(MyGeometry, DragDropEvent);
		DragConnectionOp->SetHoveredNode(SubNode.IsValid() ? SubNode : SharedThis(this));

		UDTGraphNode* TestNode = Cast<UDTGraphNode>(GraphNode);
		if (DragConnectionOp->IsValidOperation() && TestNode&&TestNode->IsSubNode())
		{
			SetDragMarker(true);
		}
	}
	SGraphNode::OnDragEnter(MyGeometry, DragDropEvent);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SGraphNodeDT::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FDragNode> DragConnectionOp = DragDropEvent.GetOperationAs<FDragNode>();
	if (DragConnectionOp.IsValid())
	{
		TSharedPtr<SGraphNode> SubNode = GetSubNodeUnderCursor(MyGeometry, DragDropEvent);
		DragConnectionOp->SetHoveredNode(SubNode.IsValid() ? SubNode : SharedThis(this));
	}
	return SGraphNode::OnDragOver(MyGeometry,DragDropEvent);
}

FReply SGraphNodeDT::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	SetDragMarker(false);
	TSharedPtr<FDragDTGraphNode> DragNodeOp = DragDropEvent.GetOperationAs<FDragDTGraphNode>();
	if (DragNodeOp.IsValid())
	{
		if (!DragNodeOp->IsValidOperation())
		{
			return FReply::Handled();
		}
		const float DragTime = float(FPlatformTime::Seconds() - DragNodeOp->StartTime);
		if (DragTime < 0.5f)
		{
			return FReply::Handled();
		}

		UDTGraphNode* MyNode = Cast<UDTGraphNode>(GraphNode);
		if (MyNode == nullptr || MyNode->IsSubNode())
		{
			return FReply::Unhandled();
		}

		const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_DragDropNode", "Drag&Drop Node"));
		bool bReorderOperation = true;
		const TArray<TSharedRef<SGraphNode>>& DraggedNodes = DragNodeOp->GetNodes();
		for (int32 Idx = 0; Idx < DraggedNodes.Num(); Idx++)
		{
			UDTGraphNode* DraggedNode = Cast<UDTGraphNode>(DraggedNodes[Idx]->GetNodeObj());
			if (DraggedNode&& DraggedNode->ParentNode)
			{
				if (DraggedNode->ParentNode != GraphNode)
				{
					bReorderOperation = false;
				}
				DraggedNode->ParentNode->RemoveSubNode(DraggedNode);
			}
		}

		UDTGraphNode* DropTargetNode = DragNodeOp->GetDropTargetNode();
		const int32 InsertIndex = MyNode->FindSubNodeDropIndex(DropTargetNode);

		for (int32 i = 0 ;i<DraggedNodes.Num();i++)
		{
			UDTGraphNode* DraggedTestNode = Cast<UDTGraphNode>(DraggedNodes[i]->GetNodeObj());
			DraggedTestNode->Modify();
			DraggedTestNode->ParentNode = MyNode;

			MyNode->Modify();
			MyNode->InsertSubNodeAt(DraggedTestNode, InsertIndex);
		}
		if (bReorderOperation)
		{
			UpdateGraphNode();
		}
		else
		{
			UDialogueEdGraph* MyGraph = MyNode->GetSTGraph();
			if (MyGraph)
			{
				MyGraph->OnSubNodeDropped();
			}
		}
	}
	return SGraphNode::OnDrop(MyGeometry, DragDropEvent);
}

void SGraphNodeDT::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FDragNode> DragNodeOp = DragDropEvent.GetOperationAs<FDragNode>();
	if (DragNodeOp.IsValid())
	{
		DragNodeOp->SetHoveredNode(TSharedPtr<SGraphNode>(NULL));
	}
	SetDragMarker(false);
	SGraphNode::OnDragLeave(DragDropEvent);
}

FReply SGraphNodeDT::OnMouseMove(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && !(GEditor->bIsSimulatingInEditor || GEditor->PlayWorld))
	{
		UDTGraphNode* TestNode = Cast<UDTGraphNode>(GraphNode);
		if (TestNode && TestNode->IsSubNode())
		{
			const TSharedRef<SGraphPanel>& Panel = GetOwnerPanel().ToSharedRef();
			const TSharedRef<SGraphNode>& Node = SharedThis(this);
			return FReply::Handled().BeginDragDrop(FDragDTGraphNode::New(Panel, Node));
		}
	}

	if (!MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && bDragMarkerVisible)
	{
		SetDragMarker(false);
	}

	return FReply::Unhandled();
}

void SGraphNodeDT::SetOwner(const TSharedRef<SGraphPanel>& OwnerPanel)
{
	SGraphNode::SetOwner(OwnerPanel);
	for (auto& ChildWidget : SubNodes)
	{
		if (ChildWidget.IsValid())
		{
			ChildWidget->SetOwner(OwnerPanel);
			OwnerPanel->AttachGraphEvents(ChildWidget);
		}
	}
}

void SGraphNodeDT::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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
			[
				PinToAdd
			];
		InputPins.Add(PinToAdd);
	}
	else
	{
		RightNodeBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.f)
			[
				PinToAdd
			];
		OutputPins.Add(PinToAdd);
	}
}

FReply SGraphNodeDT::OnMouseDown(const FGeometry & SenderGeometry, const FPointerEvent & MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && !(GEditor->bIsSimulatingInEditor || GEditor->PlayWorld))
	{
		UDTGraphNode* TestNode = Cast<UDTGraphNode>(GraphNode);
		if (TestNode&&TestNode->IsSubNode())
		{
			const TSharedRef<SGraphPanel>& Panel = GetOwnerPanel().ToSharedRef();
			const TSharedRef<SGraphNode>& Node = SharedThis(this);
			TSharedRef<FDragDTGraphNode> InGraphNode = FDragDTGraphNode::New(Panel, Node);
			return FReply::Handled().BeginDragDrop(InGraphNode);
		}
	}

	if (!MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)&&bDragMarkerVisible)
	{
		SetDragMarker(false);
	}

	return FReply::Unhandled();
}

void SGraphNodeDT::AddSubNode(TSharedPtr<SGraphNode> SubNodeWidget)
{
	SubNodes.Add(SubNodeWidget);
}

TSharedPtr<SGraphNode> SGraphNodeDT::GetSubNodeUnderCursor(const FGeometry & WidgetGeometry, const FPointerEvent & MouseEvent)
{
	TSharedPtr<SGraphNode> ResultNode;
	TSet<TSharedRef<SWidget>> SubWidgetsSet;
	for (int32 i = 0; i < SubNodes.Num(); i++)
	{
		SubWidgetsSet.Add(SubNodes[i].ToSharedRef());
	}
	TMap<TSharedRef<SWidget>, FArrangedWidget> Result;
	FindChildGeometries(WidgetGeometry, SubWidgetsSet, Result);

	if (Result.Num() > 0)
	{
		FArrangedChildren ArrangedChildren(EVisibility::Visible);
		Result.GenerateValueArray(ArrangedChildren.GetInternalArray());
		const int32 HoveredIndex = SWidget::FindChildUnderMouse(ArrangedChildren, MouseEvent);
		if (HoveredIndex != INDEX_NONE)
		{
			ResultNode = StaticCastSharedRef<SGraphNode>(ArrangedChildren[HoveredIndex].Widget);
		}
	}

	return ResultNode;
}

EVisibility SGraphNodeDT::GetDragOverMarkerVisibility() const
{
	return bDragMarkerVisible ? EVisibility::Visible : EVisibility::Collapsed;
}

void SGraphNodeDT::SetDragMarker(bool bEnabled)
{
	bDragMarkerVisible = bEnabled;
}

FText SGraphNodeDT::GetDescription() const
{
	UDTGraphNode* MyNode = CastChecked<UDTGraphNode>(GraphNode);
	return IsValid(MyNode) ? MyNode->GetDescription() : FText::GetEmpty();
}

EVisibility SGraphNodeDT::GetDescriptionVisibility() const
{
	TSharedPtr<SGraphPanel> MyOwnerPanel = GetOwnerPanel();
	return (!MyOwnerPanel.IsValid() || MyOwnerPanel->GetCurrentLOD() > EGraphRenderingLOD::LowDetail) ? EVisibility::Visible : EVisibility::Collapsed;
}

FText SGraphNodeDT::GetPreviewCornerText() const
{
	return FText::GetEmpty();
}

const FSlateBrush * SGraphNodeDT::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
}

// SGraphPinDT begin ------------------------------------------------------------------------------
void SGraphPinDT::Construct(const FArguments & InArgs, UEdGraphPin * InPin)
{
	this->SetCursor(EMouseCursor::Default);
	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != nullptr);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SGraphPinDT::GetPinBorder)
		.BorderBackgroundColor(this, &SGraphPinDT::GetPinColor)
		.OnMouseButtonDown(this, &SGraphPinDT::OnPinMouseDown)
		.Cursor(this, &SGraphPinDT::GetPinCursor)
		.Padding(FMargin(10.f))
	);
}

FSlateColor SGraphPinDT::GetPinColor() const
{
	return FSlateColor(IsHovered() ? FLinearColor::Yellow : FLinearColor::Black);
}

TSharedRef<SWidget> SGraphPinDT::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush * SGraphPinDT::GetPinBorder() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Body"));
}

// SGraphPinDT End---------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
