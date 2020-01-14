// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraphSchema_DialogueTree.h"
#include "ScopedTransaction.h"
#include "DTGraphNode.h"
#include "EdGraph/EdGraph.h"
#include "DialogueSystemEditor.h"
#include "DialogueGraphTypes.h"
#include "DTNode.h"
#include "DialogueGraphNode_Composite.h"
#include "DialogueGraphNode_Root.h"
#include "DialogueTreeConnectionDrawingPolicy.h"
#include "DialogueEdGraph.h"
#include "Modules/ModuleManager.h"
#include "DialogueTreeEditorTypes.h"
#include "DTCompositeNode.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GraphEditorActions.h"
#include "DialogueEditor.h"
#include "Toolkits/ToolkitManager.h"
#include "EdGraphNode_Comment.h"
#include "DialogueTree.h"

#define LOCTEXT_NAMESPACE "DialogueTreeEditor"

int32 UEdGraphSchema_DialogueTree::CurrentCacheRefreshID = 0;

UEdGraphNode* FDialogueTreeSchemaAction_AutoArrange::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UDialogueEdGraph* Graph = Cast<UDialogueEdGraph>(ParentGraph);
	if (Graph)
	{
		Graph->AutoArrange();
	}
	return NULL;
}

UEdGraphNode* FDialogueTreeSchemaAction_AddComment::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UEdGraphNode_Comment* const CommentTemplate = NewObject<UEdGraphNode_Comment>();
	FVector2D SpawnLocation = Location;
	TSharedPtr<IDialogueEditor> STEditor;
	if (UDialogueTree* const STAsset = Cast<UDialogueTree>(ParentGraph->GetOuter()))
	{
		TSharedPtr<IToolkit> STAssetEditor = FToolkitManager::Get().FindEditorForAsset(STAsset);
		if (STAssetEditor.IsValid())
		{
			STEditor = StaticCastSharedPtr<IDialogueEditor>(STAssetEditor);
		}
	}
	FSlateRect Bounds;
	if (STEditor.IsValid()&&STEditor->GetBoundsForSelectedNodes(Bounds,50.f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;

	}
	UEdGraphNode* const NewNode = FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation, bSelectNewNode);
	return NewNode;
}


UEdGraphSchema_DialogueTree::UEdGraphSchema_DialogueTree(const FObjectInitializer& Obj) : Super(Obj)
{
}

void UEdGraphSchema_DialogueTree::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<UDialogueGraphNode_Root> NodeCreator(Graph);
	UDialogueGraphNode_Root * MyNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(MyNode, FNodeMetadata::DefaultGraphNode);
}

void UEdGraphSchema_DialogueTree::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const FName PinCategory = ContextMenuBuilder.FromPin ? ContextMenuBuilder.FromPin->PinType.PinCategory : UDialogueTreeEditorTypes::PinCategory_MultipleNodes;
	const bool bNoParent = (ContextMenuBuilder.FromPin == NULL);
	const bool bOnlyTasks = (PinCategory == UDialogueTreeEditorTypes::PinCategory_SingleTask);
	const bool bOnlyComposite = (PinCategory == UDialogueTreeEditorTypes::PinCategory_SingleComposite);
	const bool bAllowComposites = bNoParent || !bOnlyTasks || bOnlyComposite;
	const bool bAllowTasks = bNoParent || !bOnlyComposite || bOnlyTasks;

	FDialogueSystemEditorModule& EditorModule = FModuleManager::GetModuleChecked<FDialogueSystemEditorModule>(TEXT("DialogueSystemEditor"));

	FDialogueGraphNodeClassHelper* ClassCache = EditorModule.GetClassCache().Get();

	if (bAllowComposites)
	{
		FCategorizedGraphActionListBuilder CompositesBuilder(TEXT("Composites"));
		TArray<FDialogueGraphNodeClassData> NodeClasses;
		ClassCache->GatherClasses(UDTCompositeNode::StaticClass(), NodeClasses);
		
		for (const auto& NodeClass : NodeClasses)
		{
			const FText NodeTypeName = FText::FromString(FName::NameToDisplayString(NodeClass.ToString(), false));
			TSharedPtr<FSTSchemaAction_NewNode> AddOpAction = UDTGraphSchema::AddNewNodeAction(CompositesBuilder, NodeClass.GetCategory(), NodeTypeName, FText::GetEmpty());
			UClass* GraphNodeClass = UDialogueGraphNode_Composite::StaticClass();

			UDialogueTreeGraphNode* OpNode = NewObject<UDialogueTreeGraphNode>(ContextMenuBuilder.OwnerOfTemporaries, GraphNodeClass);
			OpNode->ClassData = NodeClass;
			AddOpAction->NodeTemplate = OpNode;
		}
		ContextMenuBuilder.Append(CompositesBuilder);
	}
	if (bAllowTasks)
	{
		// TODO: TASK;
		FCategorizedGraphActionListBuilder TaskBuilder(TEXT("Tasks"));
		TArray<FDialogueGraphNodeClassData> NodeClasses;
		//ClassCache->GatherClasses(USTTaskNode::)
	}
	if (bNoParent)
	{
		TSharedPtr<FDialogueTreeSchemaAction_AutoArrange> Action = TSharedPtr<FDialogueTreeSchemaAction_AutoArrange>(
			new FDialogueTreeSchemaAction_AutoArrange(FText::GetEmpty(), LOCTEXT("AutoArrange", "Auto Arrange"), FText::GetEmpty(), 0)
			);
		ContextMenuBuilder.AddAction(Action);
	}
}

void UEdGraphSchema_DialogueTree::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
	if (InGraphNode && !InGraphPin)
	{
		const UDialogueTreeGraphNode* DTGraphNode = Cast<const UDialogueTreeGraphNode>(InGraphNode);
		if (DTGraphNode && DTGraphNode->CanPlaceBreakpoints())
		{
			MenuBuilder->BeginSection("EdGraphSchemaBreakpoints", LOCTEXT("BreakpointsHeader", "Breakpoints"));
			{
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().ToggleBreakpoint);
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().AddBreakpoint);
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().RemoveBreakpoint);
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().EnableBreakpoint);
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().DisableBreakpoint);
			}
			MenuBuilder->EndSection();
		}
	}
	Super::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}

const FPinConnectionResponse UEdGraphSchema_DialogueTree::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameNode", "Both are on the same node"));
	}
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Connect nodes"));
}

const FPinConnectionResponse UEdGraphSchema_DialogueTree::CanMergeNodes(const UEdGraphNode* A, const UEdGraphNode* B) const
{
	if (A == B)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Both are the same node"));
	}
	//TODO: debug
	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT(""));
}

FLinearColor UEdGraphSchema_DialogueTree::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::White;
}

class FConnectionDrawingPolicy* UEdGraphSchema_DialogueTree::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	return new FDialogueTreeConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

int32 UEdGraphSchema_DialogueTree::GetNodeSelectionCount(const UEdGraph* Graph) const
{
	if (Graph)
	{
		TSharedPtr<IDialogueEditor> STEditor;
		if (UDialogueTree* STAsset  = Cast<UDialogueTree>(Graph->GetOuter()))
		{
			TSharedPtr<IToolkit> STAssetEditor = FToolkitManager::Get().FindEditorForAsset(STAsset);
			if (STAssetEditor.IsValid())
			{
				STEditor = StaticCastSharedPtr<IDialogueEditor>(STAssetEditor);
			}
		}
		if (STEditor.IsValid())
		{
			return STEditor->GetSelectedNodesCount();
		}
	}
	return 0;
}

bool UEdGraphSchema_DialogueTree::IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UEdGraphSchema_DialogueTree::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UEdGraphSchema_DialogueTree::ForceVisualizationCacheClear() const
{
	++CurrentCacheRefreshID;
}

TSharedPtr<FEdGraphSchemaAction> UEdGraphSchema_DialogueTree::GetCreateCommentAction() const
{
	return TSharedPtr<FEdGraphSchemaAction>(static_cast<FEdGraphSchemaAction*>(new FDialogueTreeSchemaAction_AddComment));
}

void UEdGraphSchema_DialogueTree::GetGraphNodeContextActions(FGraphContextMenuBuilder& ContextMenuBuilder, int32 SubNodeFlags) const
{
	Super::GetGraphNodeContextActions(ContextMenuBuilder, SubNodeFlags);
	// TODO: 
}

void UEdGraphSchema_DialogueTree::GetSubNodeClasses(int32 SubNodeFlags, TArray<FDialogueGraphNodeClassData>& ClassData, UClass*& GraphNodeClass) const
{
	FDialogueSystemEditorModule& EditorModule = FModuleManager::GetModuleChecked<FDialogueSystemEditorModule>(TEXT("DialogueSystemEditor"));
	FDialogueGraphNodeClassHelper* ClassCache = EditorModule.GetClassCache().Get();
	
	// TODO: вс╫з╣Ц
}
#undef LOCTEXT_NAMESPACE