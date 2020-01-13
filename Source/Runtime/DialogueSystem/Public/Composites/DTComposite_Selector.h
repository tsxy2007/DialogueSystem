// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DTCompositeNode.h"
#include "DTComposite_Selector.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API UDTComposite_Selector : public UDTCompositeNode
{
	GENERATED_UCLASS_BODY()
public:

#if WITH_EDITOR
	virtual FName GetNodeIconName()const override;
#endif
	
};
