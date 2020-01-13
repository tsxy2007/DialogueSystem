// Fill out your copyright notice in the Description page of Project Settings.


#include "DTComposite_Selector.h"

UDTComposite_Selector::UDTComposite_Selector(const FObjectInitializer& Obj) : Super(Obj)
{
	NodeName = "Selector";
}

#if WITH_EDITOR

FName UDTComposite_Selector::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Composite.Selector.Icon");
}

#endif