

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "DialogueGraphTypes.h"
#include "Modules/ModuleInterface.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "AssetTypeActions_Base.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDialogueEditor, Verbose, All);
/** Module interface for this game's loading screens */
class FDialogueSystemEditorModule : public IModuleInterface,
	public IHasMenuExtensibility, public IHasToolBarExtensibility
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager()override  { return MenuExtensibilityManager; }
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager()override { return ToolBarExtensibilityManager; }

	TSharedPtr<struct FDialogueGraphNodeClassHelper> GetClassCache() { return ClassCache; }

	static FDialogueSystemEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FDialogueSystemEditorModule >("DialogueSystemEditor");
	}

	TSharedRef<class FDialogueEditor> CreateDialogueSystemEditor(EToolkitMode::Type Mode,TSharedPtr<class IToolkitHost> InEditWithinLevelEditor,UObject* InDialogue);
#if WITH_EDITOR
	virtual EAssetTypeCategories::Type GetDialogueAssetCategoryBit() const { return DialogueAssetCategoryBit; }
protected:
	EAssetTypeCategories::Type DialogueAssetCategoryBit;
#endif
private:
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
	TArray<TSharedPtr<class FAssetTypeActions_Base>> ItemDataAssetTypeActions;
	//TSharedPtr< FDialogueEditor > NewDialogueTreeEditor;
	TSharedPtr<struct FDialogueGraphNodeClassHelper> ClassCache;
public:
	static const FName DialogueTreeEditorAppIdentifier;
};


