// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DialogueSystemEditor.h"
#include "AssetTypeActions_Dialogue.h"
#include "DialogueEditor.h"
#include "PropertyEditorModule.h"
#include "DTNode.h"
#include "DTGraphNode.h"
#include "PropertyEditorDelegates.h"
#include "EdGraphUtilities.h"
#include "SGraphNode_DialogueTree.h"
#include "SGraphNodeDT.h"

IMPLEMENT_GAME_MODULE(FDialogueSystemEditorModule, DialogueSystemEditor);
DEFINE_LOG_CATEGORY(LogDialogueEditor);
#define LOCTEXT_NAMESPACE "DialogueSystemEditorModule"


const FName FDialogueSystemEditorModule::DialogueTreeEditorAppIdentifier(TEXT("DialogueTreeEditorApp"));

class FGraphPanelNodeFactory_DialogueTree : public FGraphPanelNodeFactory
{
public:
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override
	{
		if (UDTGraphNode* DTNode = Cast<UDTGraphNode>(Node))
		{
			return SNew(SGraphNode_DialogueTree, DTNode);
		}
		return NULL;
	}
};

TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory_Dialogue;

void FDialogueSystemEditorModule::StartupModule()
{
	MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
	ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);

	GraphPanelNodeFactory_Dialogue = MakeShareable(new FGraphPanelNodeFactory_DialogueTree);
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_Dialogue);

	IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	TSharedPtr<FAssetTypeActions_Dialogue> BehaviorTreeAssetTypeAction = MakeShareable(new FAssetTypeActions_Dialogue);
	ItemDataAssetTypeActions.Add(BehaviorTreeAssetTypeAction);
	AssetToolsModule.RegisterAssetTypeActions(BehaviorTreeAssetTypeAction.ToSharedRef());

	if (GIsEditor)
	{
		// register AI category so that AI assets can register to it
		DialogueAssetCategoryBit = AssetToolsModule.RegisterAdvancedAssetCategory(FName(TEXT("Dialogue")), LOCTEXT("DialogueAssetCategory", "Custom Dialogue"));
	}

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//PropertyModule.RegisterCustomPropertyTypeLayout("BlackboardKeySelector", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FBlackboardSelectorDetails::MakeInstance));
	//PropertyModule.RegisterCustomClassLayout("BTDecorator_Blackboard", FOnGetDetailCustomizationInstance::CreateStatic(&FBlackboardDecoratorDetails::MakeInstance));
	//PropertyModule.RegisterCustomClassLayout("BTDecorator", FOnGetDetailCustomizationInstance::CreateStatic(&FBehaviorDecoratorDetails::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();
}

void FDialogueSystemEditorModule::ShutdownModule()
{
	if (!UObjectInitialized())
	{
		return;
	}

	MenuExtensibilityManager.Reset();
	ToolBarExtensibilityManager.Reset();
	ClassCache.Reset();

	if (GraphPanelNodeFactory_Dialogue.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory_Dialogue);
		GraphPanelNodeFactory_Dialogue.Reset();
	}

	if (FModuleManager::Get().IsModuleLoaded(TEXT("AssetTools")))
	{
		IAssetTools& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (auto& AssetTypeAction : ItemDataAssetTypeActions)
		{
			if (AssetTypeAction.IsValid())
			{
				AssetToolsModule.UnregisterAssetTypeActions(AssetTypeAction.ToSharedRef());
			}
		}
	}
	ItemDataAssetTypeActions.Empty();

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		//PropertyModule.UnregisterCustomPropertyTypeLayout("BlackboardKeySelector");
		//PropertyModule.UnregisterCustomClassLayout("BTDecorator_Blackboard");
		//PropertyModule.UnregisterCustomClassLayout("BTDecorator");
		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

TSharedRef<FDialogueEditor> FDialogueSystemEditorModule::CreateDialogueSystemEditor(EToolkitMode::Type Mode, TSharedPtr<class IToolkitHost> InEditWithinLevelEditor, UObject* InDialogue)
{
	if (!ClassCache.IsValid())
	{
		ClassCache = MakeShareable(new FDialogueGraphNodeClassHelper(UDTNode::StaticClass()));
		//FGraphNodeClassHelper::AddObservedBlueprintClasses(UBTTask_BlueprintBase::StaticClass());
		//FGraphNodeClassHelper::AddObservedBlueprintClasses(UBTDecorator_BlueprintBase::StaticClass());
		//FGraphNodeClassHelper::AddObservedBlueprintClasses(UBTService_BlueprintBase::StaticClass());
		ClassCache->UpdateAvailableBlueprintClasses();
	}

	TSharedPtr< FDialogueEditor >NewDialogueTreeEditor = MakeShareable(new FDialogueEditor);
	/*if (!NewDialogueTreeEditor.IsValid())
	{
		NewDialogueTreeEditor = MakeShareable(new FDialogueEditor);
	}*/
	if (NewDialogueTreeEditor.IsValid())
	{
		NewDialogueTreeEditor->InitDialogueEditor(Mode, InEditWithinLevelEditor, InDialogue);
	}
	return NewDialogueTreeEditor.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE