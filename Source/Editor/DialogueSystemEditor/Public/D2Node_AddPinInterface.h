// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "D2Node_AddPinInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UD2Node_AddPinInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DIALOGUESYSTEMEDITOR_API ID2Node_AddPinInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void AddInputPin() {}
	virtual bool CanAddPin()const  { return true; }
};
