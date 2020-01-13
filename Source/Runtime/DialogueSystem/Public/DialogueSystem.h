

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDialogue, Verbose, All);
/** Module interface for this game's loading screens */
class FDialogueSystemModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool IsGameModule() const override;


	static FDialogueSystemModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FDialogueSystemModule >("DialogueSystem");
	}

};


