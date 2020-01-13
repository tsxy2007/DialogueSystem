// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DialogueGraphTypes.generated.h"

USTRUCT()
struct DIALOGUESYSTEMEDITOR_API FDialogueGraphNodeClassData
{
	GENERATED_USTRUCT_BODY()
public:
	FDialogueGraphNodeClassData() {}
	FDialogueGraphNodeClassData(UClass* InClass, const FString& InDeprecatedMessage);
	FDialogueGraphNodeClassData(const FString& InAssetName, const FString& InGenratedClassPackage, const FString& InClassName, UClass* InClass);

	FString ToString()const;
	FString GetClassName()const;
	FText GetCategory()const;
	FString GetDisplayName()const;
	UClass* GetClass(bool bSilent = false);
	bool IsAbstract()const;

	FORCEINLINE bool IsBlueprint()const { return AssetName.Len() > 0; }
	FORCEINLINE bool IsDeprecated()const { return DeprecatedMessage.Len() > 0; }
	FORCEINLINE FString GetDeprecatedMessage()const { return DeprecatedMessage; }
	FORCEINLINE FString GetPackageName()const { return GeneratedClassPackage; }
public:
	uint32 bIsHidden : 1;
	uint32 bHideParent : 1;
private:
	TWeakObjectPtr<UClass> Class;

	UPROPERTY()
		FString AssetName;
	UPROPERTY()
		FString GeneratedClassPackage;
	UPROPERTY()
		FString ClassName;
	UPROPERTY()
		FText Category;
	FString DeprecatedMessage;
};


struct DIALOGUESYSTEMEDITOR_API FDialogueGraphNodeClassNode
{
	FDialogueGraphNodeClassData Data;
	FString ParentClassName;
	TSharedPtr<FDialogueGraphNodeClassNode> ParentNode;
	TArray<TSharedPtr<FDialogueGraphNodeClassNode>> SubNodes;
	void AddUniqueSubNode(TSharedPtr<FDialogueGraphNodeClassNode> SubNode);

};

struct DIALOGUESYSTEMEDITOR_API FDialogueGraphNodeClassHelper
{
	DECLARE_MULTICAST_DELEGATE(FOnPackageListUpdated);
	FDialogueGraphNodeClassHelper(UClass* InRootClass);
	~FDialogueGraphNodeClassHelper();

	void GatherClasses(const UClass* BaseClass, TArray<FDialogueGraphNodeClassData>& AvailableClasses);
	static FString GetDeprecationMessage(const UClass* Class);

	void OnAssetAdded(const struct FAssetData& AssetData);
	void OnAssetRemoved(const struct FAssetData& AssetData);
	void InvalidateCache();
	void OnHotReload(bool bWasTriggeredAutomatically);

	static void AddUnknownClass(const FDialogueGraphNodeClassData& ClassData);
	static bool IsClassKnown(const FDialogueGraphNodeClassData& ClassData);
	static FOnPackageListUpdated OnPackageListUpdated;

	static int32 GetObservedBlueprintClassCount(UClass* BaseNativeClass);
	static void AddObservedBlueprintClasses(UClass* BaseNativeClass);
	void UpdateAvailableBlueprintClasses();

private:

	UClass* RootNodeClass;
	TSharedPtr<FDialogueGraphNodeClassNode> RootNode;
	static TArray<FName> UnknownPackages;
	static TMap<UClass*, int32> BlueprintClassCount;

	TSharedPtr<FDialogueGraphNodeClassNode> CreateClassDataNode(const struct FAssetData& AssetData);
	TSharedPtr<FDialogueGraphNodeClassNode> FindBaseClassNode(TSharedPtr<FDialogueGraphNodeClassNode> Node, const FString& ClassName);
	void FindAllSubClasses(TSharedPtr<FDialogueGraphNodeClassNode> Node, TArray<FDialogueGraphNodeClassData>& AvailableClasses);

	UClass* FindAssetClass(const FString& GeneratedClassPackage, const FString& AssetName);
	void BuildClassGraph();
	void AddClassGraphChildren(TSharedPtr<FDialogueGraphNodeClassNode> Node, TArray<TSharedPtr<FDialogueGraphNodeClassNode> >& NodeList);

	bool IsHidingClass(UClass* Class);
	bool IsHidingParentClass(UClass* Class);
	bool IsPackageSaved(FName PackageName);
};