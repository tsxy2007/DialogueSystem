// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueEditor.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "AssetToolsModule.h"
#include "Components/TextBlock.h"
#include "DialogueTreeEditorToolbar.h"
#include "GraphEditorActions.h"
#include "DialogueSystemEditor.h"
#include "Toolkits/IToolkitHost.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Engine/Blueprint.h"
#include "Widgets/Layout/SBorder.h"
#include "UObject/Package.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "Modules/ModuleManager.h"
#include "EditorStyleSet.h"
#include "Editor/UnrealEdEngine.h"
#include "Factories/DataAssetFactory.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "UnrealEdGlobals.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"
#include "PropertyEditorModule.h"
#include "ClassViewerModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ClassViewerFilter.h"

#include "DialogueTree.h"
#include "DialogueTreeEditorCommands.h"
#include "DialogueTreeEditorTabs.h"
#include "DialogueEdGraph.h"
#include "DialogueEditorModes.h"
#include "DialogueTreeEditorTabFactories.h"
#include "EdGraphSchema_DialogueTree.h"
#include "DialogueGraphNode_Root.h"
#include "Util/DialogueTreeEditorUtils.h"

#define LOCTEXT_NAMESPACE "DialogueEditor"

const FName FDialogueEditor::DialogueMode(TEXT("Dialogue"));
FDialogueEditor::FDialogueEditor()
{
	SelectedNodesCount = 0;
}

FDialogueEditor::~FDialogueEditor()
{

}

void FDialogueEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	DocumentManager->SetTabManager(InTabManager);
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
} 

void FDialogueEditor::RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

void FDialogueEditor::InitDialogueEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UObject* InObject)
{

	UDialogueTree* DialogueTreeToEdit = Cast<UDialogueTree>(InObject);

	if (DialogueTreeToEdit != nullptr)
	{
		Dialogue = DialogueTreeToEdit;
	}

	TSharedPtr<FDialogueEditor> ThisPtr(SharedThis(this));
	if (!DocumentManager.IsValid())
	{
		DocumentManager = MakeShareable(new FDocumentTracker);
		DocumentManager->Initialize(ThisPtr);
		// Register the document factories
		{
			TSharedRef<FDocumentTabFactory> GraphEditorFactory = MakeShareable(new FDialogueGraphEditorSummoner(ThisPtr,
				FDialogueGraphEditorSummoner::FOnCreateGraphEditorWidget::CreateSP(this, &FDialogueEditor::CreateGraphEditorWidget)
			));

			// also store off a reference to the grapheditor factory so we can find all the tabs spawned by it later.
			GraphEditorTabFactoryPtr = GraphEditorFactory;
			DocumentManager->RegisterDocumentFactory(GraphEditorFactory);
		}
	}

	TArray<UObject*> ObjectsToEdit;
	if (Dialogue != nullptr)
	{
		ObjectsToEdit.Add(Dialogue);
	}

	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FDialogueTreeEditorToolbar(SharedThis(this)));
	}

	const TArray<UObject*>* EditedObjects = GetObjectsCurrentlyBeingEdited();
	if (EditedObjects == nullptr || EditedObjects->Num() == 0)
	{
		FGraphEditorCommands::Register();
		FSTCommonCommands::Register();


		const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());
		const bool bCreateDefaultStandaloneMenu = true;
		const bool bCreateDefaultToolbar = true;
		InitAssetEditor(Mode, InitToolkitHost, FDialogueSystemEditorModule::DialogueTreeEditorAppIdentifier, DummyLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectsToEdit);

	
		BindCommonCommands();
		ExtendMenu();
		CreateInternalWidgets();


		FDialogueSystemEditorModule& DialogueSystemEditorModule = FModuleManager::LoadModuleChecked<FDialogueSystemEditorModule>("DialogueSystemEditor");
		AddMenuExtender(DialogueSystemEditorModule.GetMenuExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));

		AddApplicationMode(DialogueMode, MakeShareable(new FDialogueEditorApplicationMode(SharedThis(this))));
	}
	else
	{
	
	}

	if (DialogueTreeToEdit)
	{
		SetCurrentMode(DialogueMode);
	}
	OnClassListUpdated();
	RegenerateMenusAndToolbars();
}

FName FDialogueEditor::GetToolkitFName() const
{

	return FName("Dialogue Tree");
}

FText FDialogueEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel","DialogueTree");
}

FString FDialogueEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "DialogueTree ").ToString();
}

FLinearColor FDialogueEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.0f, 0.0f, 0.2f, 0.5f);
}

FText FDialogueEditor::GetToolkitName() const
{
	const UObject* EditingObject = GetCurrentMode() == DialogueMode ? (UObject*)Dialogue : nullptr;
	if (EditingObject != nullptr)
	{
		return FAssetEditorToolkit::GetLabelForObject(EditingObject);
	}
	return FText();
}

FText FDialogueEditor::GetToolkitToolTipText() const
{
	const UObject* EditingObject = GetCurrentMode() == DialogueMode ? (UObject*)Dialogue : nullptr;
	if (EditingObject != nullptr)
	{
		return FAssetEditorToolkit::GetToolTipTextForObject(EditingObject);
	}
	return FText();
}


void FDialogueEditor::InitializeDebuggerState(class FBehaviorTreeDebugger* ParentDebugger) const
{

}

UEdGraphNode* FDialogueEditor::FindInjectedNode(int32 Index) const
{
	return nullptr;
}

void FDialogueEditor::DoubleClickNode(class UEdGraphNode* Node)
{

}

void FDialogueEditor::FocusWindow(UObject* ObjectToFocusOn /*= NULL*/)
{
	if (ObjectToFocusOn == Dialogue)
	{
		SetCurrentMode(DialogueMode);
	}
	FWorkflowCentricApplication::FocusWindow(ObjectToFocusOn);
}

bool FDialogueEditor::GetBoundsForSelectedNodes(class FSlateRect& Rect, float Padding) const
{
	return true;
}

void FDialogueEditor::PostUndo(bool bSuccess)
{

}

void FDialogueEditor::PostRedo(bool bSuccess)
{

}

void FDialogueEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged)
{

}

FText FDialogueEditor::GetLocalizedMode(FName InMode)
{
	static TMap<FName, FText> LocModes;
	if (LocModes.Num() == 0)
	{
		LocModes.Add(DialogueMode, LOCTEXT("DialogueMode", "Dialogue"));
	}
	check(InMode != NAME_None);
	const FText* OutDesc = LocModes.Find(InMode);
	check(OutDesc);
	return *OutDesc;
}

FGraphAppearanceInfo FDialogueEditor::GetGraphAppearance() const
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText", "Dialogue Tree");

	const int32 StepIdx = 0;
	AppearanceInfo.PIENotifyText = LOCTEXT("InactiveLabel", "INACTIVE");
	return AppearanceInfo;
}

void FDialogueEditor::OnNodeDoubleClicked(class UEdGraphNode* Node)
{

}

void FDialogueEditor::OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor)
{
	UpdateGraphEdptr = InGraphEditor;

	FGraphPanelSelectionSet CurrentSelection = InGraphEditor->GetSelectedNodes();
	OnSelectedNodesChanged(CurrentSelection);
}

void FDialogueEditor::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged)
{

}

bool FDialogueEditor::IsDebuggerReady() const
{
	return false;
}

UDialogueTree* FDialogueEditor::GetDialogueTree() const
{
	return Dialogue;
}

TSharedRef<SWidget> FDialogueEditor::SpawnProperties()
{
	return SNew(SVerticalBox)
		/*+ SVerticalBox::Slot()
		.FillHeight(0.2f)
		.HAlign(HAlign_Fill)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Properties")))
		]*/
		+ SVerticalBox::Slot()
		.FillHeight(0.8f)
		.HAlign(HAlign_Fill)
		[
			DetailsView.ToSharedRef()
		];
}

TSharedRef<SWidget> FDialogueEditor::SpawnSearch()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		.HAlign(HAlign_Fill)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Search")))
		];
}

void FDialogueEditor::BindCommonCommands()
{
	ToolkitCommands->MapAction(FSTCommonCommands::Get().SearchST,
		FExecuteAction::CreateSP(this, &FDialogueEditor::SearchTree),
		FCanExecuteAction::CreateSP(this, &FDialogueEditor::CanSearchTree));

}

void FDialogueEditor::ExtendMenu()
{
	struct Local
	{
		static void FillEditMenu(FMenuBuilder& MenuBuilder)
		{
			MenuBuilder.BeginSection("EditSearch", LOCTEXT("EditMenu_SearchHeading", "Search"));
			{
				MenuBuilder.AddMenuEntry(FSTCommonCommands::Get().SearchST);
			}
			MenuBuilder.EndSection();
		}
	};

	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);

	MenuExtender->AddMenuExtension(
		"EditHistory",
		EExtensionHook::After,
		GetToolkitCommands(),
		FMenuExtensionDelegate::CreateStatic(&Local::FillEditMenu)
	);
	AddMenuExtender(MenuExtender);
}

void FDialogueEditor::CreateInternalWidgets()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, false);
	DetailViewArgs.NotifyHook = this;
	DetailViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
	DetailsView = PropertyEditorModule.CreateDetailView(DetailViewArgs);
	DetailsView->SetObject(NULL);
	DetailsView->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateSP(this, &FDialogueEditor::IsPropertyEditable));
	DetailsView->OnFinishedChangingProperties().AddSP(this, &FDialogueEditor::OnFinishedChangingProperties);
}

void FDialogueEditor::OnClassListUpdated()
{
	FDialogueGraphEditor::OnClassListUpdated();
	// Ìí¼Ó£»
	//const int32 NumTaskBP = FGraphNodeClassHelper::GetObservedBlueprintClassCount()

}

void FDialogueEditor::SearchTree()
{
	TabManager->InvokeTab(FDialogueTreeEditorTabs::SearchID);
}

bool FDialogueEditor::CanSearchTree() const
{
	return true;
}

bool FDialogueEditor::CanAccessDialogueTreeMode() const
{
	return Dialogue != nullptr;
}

void FDialogueEditor::OnAddInputPin()
{

}

bool FDialogueEditor::CanAddInputPin() const
{
	return true;
}

void FDialogueEditor::OnRemoveInputPin()
{

}

bool FDialogueEditor::CanRemoveInputPin() const
{
	return true;
}

bool FDialogueEditor::InEditingMode(bool bGraphIsEditable) const
{
	return true;
}

void FDialogueEditor::CreateNewTask() const
{
	
}

bool FDialogueEditor::CanCreateNewTask() const
{
	return !IsDebuggerReady();
}

bool FDialogueEditor::IsNewTaskButtonVisible() const
{
	return true;
}

bool FDialogueEditor::IsNewTaskComboVisible() const
{
	return true;
}

template<typename Type>
class FNewNodeClassFilter : public IClassViewerFilter
{
public:
	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<class FClassViewerFilterFuncs> InFilterFuncs) override
	{
		if (InClass != nullptr)
		{
			return InClass->IsChildOf(Type::StaticClass());
		}
		return false;
	}
	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const class IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<class FClassViewerFilterFuncs> InFilterFuncs) override
	{
		return InUnloadedClassData->IsChildOf(Type::StaticClass());
	}
};

TSharedRef<SWidget> FDialogueEditor::HandleCreateNewTaskMenu() const
{
	FClassViewerInitializationOptions Options;
	Options.bShowUnloadedBlueprints = true;
	Options.ClassFilter = MakeShareable(new FNewNodeClassFilter<UDTGraphNode>());

	FOnClassPicked OnPicked(FOnClassPicked::CreateSP(this, &FDialogueEditor::HandleNewNodeClassPicked));

	return FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(Options, OnPicked);
}

void FDialogueEditor::HandleNewNodeClassPicked(UClass* InClass) const
{

}

bool FDialogueEditor::IsPropertyEditable() const
{
	TSharedPtr<SGraphEditor> FocusedGraph = UpdateGraphEdptr.Pin();
	return FocusedGraph.IsValid() && FocusedGraph->GetCurrentGraph() && FocusedGraph->GetCurrentGraph()->bEditable;
}

void FDialogueEditor::OnPackageSaved(const FString& PackageFileName, UObject* Outer)
{
	UDialogueEdGraph* MyGraph = Dialogue ? Cast<UDialogueEdGraph>(Dialogue->BTGraph) : NULL;
	if (MyGraph)
	{
		const bool bUpdated = false; // 
		if (bUpdated)
		{
			MyGraph->UpdateAsset(UDialogueEdGraph::ClearDebuggerFlags);
		}
	}
}

void FDialogueEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	
	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetName() == TEXT("FlowAbortMode"))
		{
			
		}
	}

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("DialogueAsset"))
	{
		UDialogueEdGraph * MyGraph = Cast<UDialogueEdGraph>(Dialogue->BTGraph);
	}
}

void FDialogueEditor::RestoreBehaviorTree()
{
	UDialogueEdGraph * MyGraph = Cast<UDialogueEdGraph>(Dialogue->BTGraph);
	const bool bNewGraph = MyGraph == nullptr;
	if (MyGraph == nullptr)
	{
		Dialogue->BTGraph = FBlueprintEditorUtils::CreateNewGraph(Dialogue, TEXT("Dialogue Tree"), UDialogueEdGraph::StaticClass(), UEdGraphSchema_DialogueTree::StaticClass());
		MyGraph = Cast<UDialogueEdGraph>(Dialogue->BTGraph);

		const UEdGraphSchema* Schema = MyGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*MyGraph);
		MyGraph->OnCreated();
	}
	else
	{
		MyGraph->OnLoaded();
	}
	MyGraph->Initialize();

	TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(MyGraph);
	TSharedPtr<SDockTab> DocumentTab = DocumentManager->OpenDocument(Payload, bNewGraph ? FDocumentTracker::OpenNewDocument : FDocumentTracker::RestorePreviousDocument);
	if (Dialogue->LastEditedDocuments.Num()>0)
	{
		TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(DocumentTab->GetContent());
		GraphEditor->SetViewLocation(Dialogue->LastEditedDocuments[0].SavedViewOffset, Dialogue->LastEditedDocuments[0].SavedZoomAmount);
	}
	// ²»È«
	const bool bIncreaseVersionNum = false;
	if (bNewGraph)
	{
		MyGraph->UpdateAsset(true);
	}
	else
	{
		MyGraph->UpdateAsset(true);
	}

}

void FDialogueEditor::SaveEditedObjectState()
{
	Dialogue->LastEditedDocuments.Empty();
	DocumentManager->SaveAllState();
}

//
//void FDialogueEditor::OnClassListUpdated()
//{
//
//}

void FDialogueEditor::SaveAsset_Execute()
{
	if (Dialogue)
	{
		UDialogueEdGraph* DialogueGraph = Cast<UDialogueEdGraph>(Dialogue->BTGraph);
		if (DialogueGraph)
		{
			DialogueGraph->OnSave();
		}
	}
	IDialogueEditor::SaveAsset_Execute();
}

TSharedRef<class SGraphEditor> FDialogueEditor::CreateGraphEditorWidget(UEdGraph* InGraph)
{
	check(InGraph != NULL);
	if (!GraphEditorCommands.IsValid())
	{
		CreateCommandList();
		GraphEditorCommands->MapAction(FGraphEditorCommands::Get().RemoveExecutionPin,
			FExecuteAction::CreateSP(this, &FDialogueEditor::OnRemoveInputPin),
			FCanExecuteAction::CreateSP(this, &FDialogueEditor::CanRemoveInputPin)
		);

		GraphEditorCommands->MapAction(FGraphEditorCommands::Get().AddExecutionPin,
			FExecuteAction::CreateSP(this, &FDialogueEditor::OnAddInputPin),
			FCanExecuteAction::CreateSP(this,&FDialogueEditor::CanAddInputPin)
			);
	}

	SGraphEditor::FGraphEditorEvents InEnvents;
	InEnvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FDialogueEditor::OnSelectedNodesChanged);
	InEnvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FDialogueEditor::OnNodeDoubleClicked);
	InEnvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FDialogueEditor::OnNodeTitleCommitted);

	TSharedRef<SWidget> TitleBarWidget =
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush(TEXT("Graph.TitleBackground")))
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("DialogueTreeGraphLabel","Dialogue Tree"))
				.TextStyle(FEditorStyle::Get(),TEXT("GraphBreadcrumbButtonText"))
			]
		];
	const bool bGraphIsEditable = InGraph->bEditable;
	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(this, &FDialogueEditor::InEditingMode, bGraphIsEditable)
		.Appearance(this, &FDialogueEditor::GetGraphAppearance)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(InGraph)
		.GraphEvents(InEnvents);
}

void FDialogueEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	SelectedNodesCount = NewSelection.Num();

	DialogueTreeEditorUtils::FPropertySelectionInfo SelectionInfo;
	TArray<UObject*> Selection = DialogueTreeEditorUtils::GetSelectionForPropertyEditor(NewSelection, SelectionInfo);
	UDialogueEdGraph* MyGraph = Cast<UDialogueEdGraph>(Dialogue->BTGraph);
	if (SelectedNodesCount == 1)
	{
		if (DetailsView.IsValid())
		{
			DetailsView->SetObjects(Selection);
		}
	}
	else if (DetailsView.IsValid())
	{
		if (Selection.Num() == 0)
		{
			UDialogueTreeGraphNode* RootNode = nullptr;
			for (const auto& Node : MyGraph->Nodes)
			{
				RootNode = Cast<UDialogueGraphNode_Root>(Node);
				if (RootNode != nullptr)
				{
					break;
				}
			}
			DetailsView->SetObject(Dialogue);
		}
		else
		{
			DetailsView->SetObject(nullptr);
		}
	}

}
#undef LOCTEXT_NAMESPACE