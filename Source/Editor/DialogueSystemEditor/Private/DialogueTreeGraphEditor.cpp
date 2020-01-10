// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTreeGraphEditor.h"
#include "Editor/EditorEngine.h"
#include "GenericCommands.h"
#include "SlateApplication.h"
#include "ScopedTransaction.h"
#include "GraphEditor.h"
#include "HAL/PlatformApplicationMisc.h"
#include "EdGraphUtilities.h"
#include "DialogueTree.h"
#include "WorkflowTabManager.h"
#include "TabManager.h"
#include "EditorStyleSet.h"
#include "STextBlock.h"
#include "PropertyEditorModule.h"
#include "ModuleManager.h"
#include "AssetEditorToolkit.h"
#include "GraphEditorActions.h"
#include "DialogueTreeEditorModes.h"
#include "DialogueTreeEditorToolbar.h"
#include "DialogueGraph.h"
#include "BlueprintEditorUtils.h"
#include "WorkflowUObjectDocuments.h"
#include "SharedPointer.h"
#include "EdGraphSchema_DialogueTree.h"

#define LOCTEXT_NAMESPACE "DialogueTreeGraphEditor"

const FName FDialogueTreeGraphEditor::DialogueTreeMode(TEXT("DialogueTree"));

FDialogueTreeGraphEditor::FDialogueTreeGraphEditor()
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor != nullptr)
	{
		Editor->RegisterForUndo(this);
	}
	//OnClassListUpdatedDelegateHandle = FGraphNodeClassHelper::OnPackageListUpdated.

	DialogueTree = nullptr;
}

FDialogueTreeGraphEditor::~FDialogueTreeGraphEditor()
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor)
	{
		Editor->UnregisterForUndo(this);
	}
}

void FDialogueTreeGraphEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	DocumentManager->SetTabManager(InTabManager);
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

void FDialogueTreeGraphEditor::InitDialogueTreeEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* InObject)
{
	UDialogueTree* DialogueTreeToEdit = Cast<UDialogueTree>(InObject);
	if (DialogueTreeToEdit != nullptr)
	{
		DialogueTree = DialogueTreeToEdit;
	}
	TSharedPtr<FDialogueTreeGraphEditor> ThisPtr(SharedThis(this));
	if (!DocumentManager.IsValid())
	{
		DocumentManager = MakeShareable(new FDocumentTracker);
		DocumentManager->Initialize(ThisPtr);
		{
			TSharedRef<FDocumentTabFactory> GraphEditorFactory = MakeShareable(new FDialogueGraphEditorSummoner(ThisPtr,
				FDialogueGraphEditorSummoner::FOnCreateGraphEditorWidget::CreateSP(this, &FDialogueTreeGraphEditor::CreateGraphEditorWidget)
			));
			GraphEditorTabFactoryPtr = GraphEditorFactory;
			DocumentManager->RegisterDocumentFactory(GraphEditorFactory);
		}
	}

	TArray<UObject*> ObjectsToEdit;
	if (DialogueTree)
	{
		ObjectsToEdit.Add(DialogueTree);
	}

	// tool bar build 
	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FDialogueTreeEditorToolbar(SharedThis(this)));
	}
	// end;
	const TArray<UObject*>* EditedObjects = GetObjectsCurrentlyBeingEdited();
	if (EditedObjects == nullptr || EditedObjects->Num() == 0)
	{
		FGraphEditorCommands::Register();

		const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());
		const bool bCreateDefaultStandaloneMenu = true;
		const bool bCreateDefaultToolbar = true;
		InitAssetEditor(Mode, InitToolkitHost, FDialogueSystemEditorModule::DialogueTreeEditorAppIdentifier, DummyLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectsToEdit);

		BindCommonCommands();
		ExtendMenu();
		CreateInternalWidgets();

		AddApplicationMode(DialogueTreeMode, MakeShareable(new FDialogueTreeEditorApplicationMode(ThisPtr)));
	}


	SetCurrentMode(DialogueTreeMode);
	OnClassListUpdated();
	RegenerateMenusAndToolbars();
}

FName FDialogueTreeGraphEditor::GetToolkitFName() const
{
	return FName("Dialogue Tree");
}

FText FDialogueTreeGraphEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel","DialogueTree");
}

FString FDialogueTreeGraphEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix","DialogueTree").ToString();
}

FLinearColor FDialogueTreeGraphEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.0f, 0.0f, 0.2f, 0.5f);
}

FText FDialogueTreeGraphEditor::GetToolkitName() const
{
	if (DialogueTree != nullptr)
	{
		return FAssetEditorToolkit::GetLabelForObject(DialogueTree);
	}
	return FText();
}

FText FDialogueTreeGraphEditor::GetToolkitToolTipText() const
{
	if (DialogueTree != nullptr)
	{
		return FAssetEditorToolkit::GetToolTipTextForObject(DialogueTree);
	}
	return FText();
}

void FDialogueTreeGraphEditor::NotifyPostChange(const FPropertyChangedEvent & PropertyChangedEvent, UProperty * PropertyThatChanged)
{
}

FGraphPanelSelectionSet FDialogueTreeGraphEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	if (TSharedPtr<SGraphEditor> FocusedGraphEd = UpdateGraphEdPtr.Pin())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}
	return CurrentSelection;
}

void FDialogueTreeGraphEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{

}

void FDialogueTreeGraphEditor::OnNodeDoubleClicked(UEdGraphNode * Node)
{
}

void FDialogueTreeGraphEditor::OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor)
{
}

void FDialogueTreeGraphEditor::OnNodeTitleCommitted(const FText & NewText, ETextCommit::Type CommitInfo, UEdGraphNode * PropertyThatChanged)
{
}

void FDialogueTreeGraphEditor::OnAddInputPin()
{
}

bool FDialogueTreeGraphEditor::CanAddInputPin() const
{
	return false;
}

void FDialogueTreeGraphEditor::OnRemoveInputPin()
{
}

bool FDialogueTreeGraphEditor::CanRemoveInputPin() const
{
	return false;
}

void FDialogueTreeGraphEditor::SearchTree()
{
}

bool FDialogueTreeGraphEditor::CanSearchTree() const
{
	return false;
}

void FDialogueTreeGraphEditor::JumpToNode(const UEdGraphNode * Node)
{
}

bool FDialogueTreeGraphEditor::IsPropertyEditable() const
{
	TSharedPtr<SGraphEditor> FocusedGraphEd = UpdateGraphEdPtr.Pin();
	return FocusedGraphEd.IsValid() && FocusedGraphEd->GetCurrentGraph() && FocusedGraphEd->GetCurrentGraph()->bEditable;
}

void FDialogueTreeGraphEditor::OnPackageSaved(const FString & PackageFileName, UObject * Outer)
{
}

void FDialogueTreeGraphEditor::OnFinishedChangingProperties(const FPropertyChangedEvent & PropertyChangedEvent)
{
}

void FDialogueTreeGraphEditor::UpdateToolBar()
{
}

FGraphAppearanceInfo FDialogueTreeGraphEditor::GetGraphAppearance() const
{
	return FGraphAppearanceInfo();
}

bool FDialogueTreeGraphEditor::IsEditingMode(bool bGraphIsEditable) const
{
	return false;
}

void FDialogueTreeGraphEditor::PostUndo(bool bSuccess)
{
	if (bSuccess)
	{
		if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin())
		{
			CurrentGraphEditor->ClearSelectionSet();
			CurrentGraphEditor->NotifyGraphChanged();
		}
		FSlateApplication::Get().DismissAllMenus();
	}
}

void FDialogueTreeGraphEditor::PostRedo(bool bSuccess)
{
	if (bSuccess)
	{
		if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin())
		{
			CurrentGraphEditor->ClearSelectionSet();
			CurrentGraphEditor->NotifyGraphChanged();
		}
		FSlateApplication::Get().DismissAllMenus();
	}
}

void FDialogueTreeGraphEditor::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}
	GraphEditorCommands = MakeShareable(new FUICommandList);

	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CanSelectAllNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CanDeleteNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CanCopyNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CanCutNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CanPasteNodes));

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FDialogueTreeGraphEditor::CanDuplicateNodes));
}

void FDialogueTreeGraphEditor::SelectAllNodes()
{
	if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FDialogueTreeGraphEditor::CanSelectAllNodes() const
{
	return true;
}

void FDialogueTreeGraphEditor::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}
	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());
	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes);NodeIt;++NodeIt)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
		{
			Node->Modify();
			Node->DestroyNode();
		}
	}
}

bool FDialogueTreeGraphEditor::CanDeleteNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes);SelectedIter;++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node&&Node->CanUserDeleteNode())
		{
			return true;
		}
	}
	return false;
}

void FDialogueTreeGraphEditor::DeleteSelectedDuplicatableNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}
	const FGraphPanelSelectionSet OldSelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectIter(OldSelectedNodes);SelectIter;++SelectIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectIter);
		if (Node&& Node->CanDuplicateNode())
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
	DeleteSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();
	for (FGraphPanelSelectionSet::TConstIterator SelectIter(OldSelectedNodes); SelectIter; ++SelectIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectIter))
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FDialogueTreeGraphEditor::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FDialogueTreeGraphEditor::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FDialogueTreeGraphEditor::CopySelectedNodes()
{
	FGraphPanelSelectionSet SelectNodes = GetSelectedNodes();
	
}

bool FDialogueTreeGraphEditor::CanCopyNodes() const
{
	const FGraphPanelSelectionSet SelectdNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectdNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			return true;
		}
	}
	return false;
}

void FDialogueTreeGraphEditor::PasteNodes()
{

}

void FDialogueTreeGraphEditor::PasterNodesHere(const FVector2D& Location)
{

}

bool FDialogueTreeGraphEditor::CanPasteNodes() const
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return false;
	}
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
	return FEdGraphUtilities::CanImportNodesFromText(CurrentGraphEditor->GetCurrentGraph(),ClipboardContent);
}

void FDialogueTreeGraphEditor::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FDialogueTreeGraphEditor::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FDialogueTreeGraphEditor::OnClassListUpdated()
{

}

bool FDialogueTreeGraphEditor::InEditingMode(bool bGraphIsEditable) const
{
	return bGraphIsEditable;
}

bool FDialogueTreeGraphEditor::CanAccessDialogueTreeMode() const
{
	return DialogueTree != nullptr;
}

FText FDialogueTreeGraphEditor::GetLocalizeMode(FName InMode)
{
	static TMap<FName, FText> LocModes;
	if (LocModes.Num() == 0)
	{
		LocModes.Add(DialogueTreeMode, LOCTEXT("DialogueTreeMode", "Dialogue Tree"));
	}
	check(InMode != NAME_None);
	const FText* OutDesc = LocModes.Find(InMode);
	check(OutDesc);
	return *OutDesc;
}

UDialogueTree* FDialogueTreeGraphEditor::GetDialogueTree() const
{
	return DialogueTree;
}

TSharedRef<SWidget> FDialogueTreeGraphEditor::SpawnProperties()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		[
			DetailsView.ToSharedRef()
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("DialogueText","nicai"))
		];
}

TSharedRef<SWidget> FDialogueTreeGraphEditor::SpawnSearch()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0)
		.HAlign(HAlign_Fill)
		[
			SNew(STextBlock)
		];
}

void FDialogueTreeGraphEditor::RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManger)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManger);
}

void FDialogueTreeGraphEditor::RestoreDialogueTree()
{
	UDialogueGraph* MyGraph = Cast<UDialogueGraph>(DialogueTree->DTGraph);
	const bool bNewGraph = MyGraph == nullptr;
	if (MyGraph == nullptr)
	{
		DialogueTree->DTGraph = FBlueprintEditorUtils::CreateNewGraph(DialogueTree, TEXT("Dialogue Tree"), UDialogueGraph::StaticClass(), UEdGraphSchema_DialogueTree::StaticClass());
		MyGraph = Cast<UDialogueGraph>(DialogueTree->DTGraph);
		const UEdGraphSchema* Schema = MyGraph->GetSchema();
		MyGraph->OnCreated();
	}
	else
	{
		MyGraph->OnLoaded();
	}

	MyGraph->Initialize();
	TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(MyGraph);
	TSharedPtr<SDockTab> DocumentTab = DocumentManager->OpenDocument(Payload, bNewGraph ? FDocumentTracker::OpenNewDocument : FDocumentTracker::RestorePreviousDocument);
	if (DialogueTree->LastEditedDocuments.Num() > 0)
	{
		TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(DocumentTab->GetContent());
		GraphEditor->SetViewLocation(DialogueTree->LastEditedDocuments[0].SavedViewOffset, DialogueTree->LastEditedDocuments[0].SavedZoomAmount);
	}
	const bool bIncreaseVersionNum = false;
	MyGraph->UpdateAsset(UDialogueGraph::KeepRebuildCounter);
}

void FDialogueTreeGraphEditor::SaveEditedObjectState()
{
	DialogueTree->LastEditedDocuments.Empty();
	DocumentManager->SaveAllState();
}

TSharedRef<class SGraphEditor> FDialogueTreeGraphEditor::CreateGraphEditorWidget(UEdGraph* InGraph)
{
	check(InGraph != nullptr);
	if (!GraphEditorCommands.IsValid())
	{
		CreateCommandList();
	}

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FDialogueTreeGraphEditor::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FDialogueTreeGraphEditor::OnNodeDoubleClicked);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FDialogueTreeGraphEditor::OnNodeTitleCommitted);


	// make title bar
	TSharedPtr<SWidget> TitleBarWidget =
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush(TEXT("Graph.TitleBackground")))
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("BehaviorTreeGraphLabel", "Dialogue Tree"))
			.TextStyle(FEditorStyle::Get(), TEXT("GraphBreadcrumbButtonText"))
			]
		];
	//make full graph editor

	const bool bGraphIsEditor = InGraph->bEditable;

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(this, &FDialogueTreeGraphEditor::InEditingMode, bGraphIsEditor)
		.Appearance(this, &FDialogueTreeGraphEditor::GetGraphAppearance)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(InGraph)
		.GraphEvents(InEvents);
}

void FDialogueTreeGraphEditor::CreateInternalWidgets()
{
	// ≥ı ºªØ Ù–‘
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, false);
	DetailViewArgs.NotifyHook = this;
	DetailViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
	DetailsView = PropertyEditorModule.CreateDetailView(DetailViewArgs);
	DetailsView->SetObject(nullptr);
	DetailsView->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateSP(this, &FDialogueTreeGraphEditor::IsPropertyEditable));
	DetailsView->OnFinishedChangingProperties().AddSP(this, &FDialogueTreeGraphEditor::OnFinishedChangingProperties);
}

void FDialogueTreeGraphEditor::ExtendMenu()
{
	//struct Local
	//{
	//	static void FillEditMenu(FMenuBuilder& MenuBuilder)
	//	{
	//		MenuBuilder.BeginSection("EditSearch", LOCTEXT("EditMenu_SearchHeading", "Search"));
	//		{
	//			MenuBuilder.AddMenuEntry();
	//		}
	//	}
	//};
}

void FDialogueTreeGraphEditor::BindCommonCommands()
{
	
}

#undef LOCTEXT_NAMESPACE