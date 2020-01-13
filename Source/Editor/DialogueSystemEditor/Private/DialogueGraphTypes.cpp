// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueGraphTypes.h"
#include "UObject/Object.h"
#include "UObject/Class.h"
#include "Misc/FeedbackContext.h"
#include "Modules/ModuleManager.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"
#include "Misc/PackageName.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Blueprint.h"
#include "AssetData.h"
#include "Editor.h"
#include "Logging/MessageLog.h"
#include "ARFilter.h"
#include "AssetRegistryModule.h"
#include "Misc/HotReloadInterface.h"

#define LOCTEXT_NAMESPACE "DialogueGraph"
FDialogueGraphNodeClassData::FDialogueGraphNodeClassData(UClass* InClass, const FString& InDeprecatedMessage)
	:bIsHidden(0),
	bHideParent(0),
	Class(InClass),
	DeprecatedMessage(InDeprecatedMessage)
{
	Category = GetCategory();
	if (InClass)
	{
		ClassName = InClass->GetName();
	}
}

FDialogueGraphNodeClassData::FDialogueGraphNodeClassData(const FString& InAssetName, const FString& InGenratedClassPackage, const FString& InClassName, UClass* InClass)
	:bIsHidden(0),
	bHideParent(0),
	Class(InClass),
	AssetName(InAssetName),
	GeneratedClassPackage(InGenratedClassPackage),
	ClassName(InClassName)
{
	Category = GetCategory();
}

FString FDialogueGraphNodeClassData::ToString() const
{
	FString ShortName = GetDisplayName();
	if (!ShortName.IsEmpty())
	{
		return ShortName;
	}
	UClass* MyClass = Class.Get();
	if (MyClass)
	{
		FString ClassDesc = MyClass->GetName();
		if (MyClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
		{
			return ClassDesc.LeftChop(2);
		}
		const int32 ShortNameIdx = ClassDesc.Find(TEXT("_"));
		if (ShortNameIdx != INDEX_NONE)
		{
			ClassDesc = ClassDesc.Mid(ShortNameIdx + 1);
		}
		return ClassDesc;
	}
	return AssetName;
}

FString FDialogueGraphNodeClassData::GetClassName() const
{
	return Class.IsValid() ? Class->GetName() : ClassName;
}

FText FDialogueGraphNodeClassData::GetCategory() const
{
	return Class.IsValid() ? Class->GetMetaDataText(TEXT("Category"), TEXT("UObjectCategory"), Class->GetFullGroupName(false)) : Category;
}

FString FDialogueGraphNodeClassData::GetDisplayName() const
{
	return Class.IsValid() ? Class->GetMetaData(TEXT("DisplayName")) : FString();
}

UClass * FDialogueGraphNodeClassData::GetClass(bool bSilent)
{
	UClass* RetClass = Class.Get();
	if (RetClass == nullptr&& GeneratedClassPackage.Len())
	{
		GWarn->BeginSlowTask(LOCTEXT("LoadPackage", "Loading Package..."), true);
		UPackage* Package = LoadPackage(nullptr, *GeneratedClassPackage, LOAD_NoRedirects);
		if (Package)
		{
			Package->FullyLoad();
			UObject* Object = FindObject<UObject>(Package, *AssetName);
			GWarn->EndSlowTask();

			UBlueprint* BlueprintOb = Cast<UBlueprint>(Object);
			RetClass = BlueprintOb ? *BlueprintOb->GeneratedClass : Object ? Object->GetClass() : NULL;
			Class = RetClass;
		}
		else
		{
			GWarn->EndSlowTask();
			if (!bSilent)
			{
				FMessageLog EditorErrors("EditorErrors");
				EditorErrors.Error(LOCTEXT("PackageLoadFail", "Package Load Failed"));
				EditorErrors.Info(FText::FromString(GeneratedClassPackage));
				EditorErrors.Notify(LOCTEXT("PackageLoadFail", "Package Load Failed"));
			}
		}
	}
	return RetClass;
}

bool FDialogueGraphNodeClassData::IsAbstract() const
{
	return Class.IsValid() ? Class.Get()->HasAnyClassFlags(CLASS_Abstract) : false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void FDialogueGraphNodeClassNode::AddUniqueSubNode(TSharedPtr<FDialogueGraphNodeClassNode> SubNode)
{
	for (int32 Idx = 0; Idx < SubNodes.Num(); Idx++)
	{
		if (SubNode->Data.GetClassName() == SubNodes[Idx]->Data.GetClassName())
		{
			return;
		}
	}
	SubNodes.Add(SubNode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
TArray<FName> FDialogueGraphNodeClassHelper::UnknownPackages;
TMap<UClass*, int32> FDialogueGraphNodeClassHelper::BlueprintClassCount;
FDialogueGraphNodeClassHelper::FOnPackageListUpdated FDialogueGraphNodeClassHelper::OnPackageListUpdated;

FDialogueGraphNodeClassHelper::FDialogueGraphNodeClassHelper(UClass* InRootClass)
{
	RootNodeClass = InRootClass;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnFilesLoaded().AddRaw(this, &FDialogueGraphNodeClassHelper::InvalidateCache);
	AssetRegistryModule.Get().OnAssetAdded().AddRaw(this, &FDialogueGraphNodeClassHelper::OnAssetAdded);
	AssetRegistryModule.Get().OnAssetRemoved().AddRaw(this, &FDialogueGraphNodeClassHelper::OnAssetRemoved);

	IHotReloadInterface& HotReloadSupport = FModuleManager::GetModuleChecked<IHotReloadInterface>("HotReload");
	GEditor->OnBlueprintCompiled().AddRaw(this, &FDialogueGraphNodeClassHelper::InvalidateCache);
	GEditor->OnClassPackageLoadedOrUnloaded().AddRaw(this, &FDialogueGraphNodeClassHelper::InvalidateCache);

	UpdateAvailableBlueprintClasses();
}

FDialogueGraphNodeClassHelper::~FDialogueGraphNodeClassHelper()
{
	if (FModuleManager::Get().IsModuleLoaded(TEXT("AssetRegistry")))
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		AssetRegistryModule.Get().OnFilesLoaded().RemoveAll(this);
		AssetRegistryModule.Get().OnAssetAdded().RemoveAll(this);
		AssetRegistryModule.Get().OnAssetRemoved().RemoveAll(this);
		if (FModuleManager::Get().IsModuleLoaded(TEXT("HotReload")))
		{
			IHotReloadInterface& HotReloadSupport = FModuleManager::GetModuleChecked<IHotReloadInterface>("HotReload");
			HotReloadSupport.OnHotReload().RemoveAll(this);
		}
		if (UObjectInitialized())
		{
			GEditor->OnBlueprintCompiled().RemoveAll(this);
			GEditor->OnClassPackageLoadedOrUnloaded().RemoveAll(this);
		}
	}
}

void FDialogueGraphNodeClassHelper::GatherClasses(const UClass* BaseClass, TArray<FDialogueGraphNodeClassData>& AvailableClasses)
{
	const FString BaseClassName = BaseClass->GetName();
	if (!RootNode.IsValid())
	{
		BuildClassGraph();
	}
	TSharedPtr<FDialogueGraphNodeClassNode> BaseNode = FindBaseClassNode(RootNode, BaseClassName);
	FindAllSubClasses(BaseNode, AvailableClasses);
}

FString FDialogueGraphNodeClassHelper::GetDeprecationMessage(const UClass* Class)
{
	static FName DialogueMetaDeprecated = TEXT("DeprecatedNode");
	static FName DialogueMetaDeprecatedMessage = TEXT("DeprecationMessage");
	FString DefDeprecatedMessage("Please remove it!");
	FString DeprecatedPrefix("DEPRECATED");
	FString DeprecatedMessage;
	if (Class && Class->HasAnyClassFlags(CLASS_Native) && Class->HasMetaData(DialogueMetaDeprecated))
	{
		DeprecatedMessage = DeprecatedPrefix + TEXT(": ");
		DeprecatedMessage += Class->HasMetaData(DialogueMetaDeprecatedMessage) ? Class->GetMetaData(DialogueMetaDeprecatedMessage) : DefDeprecatedMessage;
	}

	return DeprecatedMessage;
}

void FDialogueGraphNodeClassHelper::OnAssetAdded(const struct FAssetData& AssetData)
{
	TSharedPtr<FDialogueGraphNodeClassNode> Node = CreateClassDataNode(AssetData);
	TSharedPtr<FDialogueGraphNodeClassNode> ParentNode;
	if (Node.IsValid())
	{
		ParentNode = FindBaseClassNode(RootNode, Node->ParentClassName);
		if (!IsPackageSaved(AssetData.PackageName))
		{
			UnknownPackages.AddUnique(AssetData.PackageName);
		}
		else
		{
			const int32 PrevListCount = UnknownPackages.Num();
			UnknownPackages.RemoveSingleSwap(AssetData.PackageName);
			if (UnknownPackages.Num() != PrevListCount)
			{
				OnPackageListUpdated.Broadcast();
			}
		}
	}
	if (ParentNode.IsValid())
	{
		ParentNode->AddUniqueSubNode(Node);
		Node->ParentNode = ParentNode;
	}
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	if (!AssetRegistryModule.Get().IsLoadingAssets())
	{
		UpdateAvailableBlueprintClasses();
	}
}

void FDialogueGraphNodeClassHelper::OnAssetRemoved(const struct FAssetData& AssetData)
{
	FString AssetClassName;
	if (AssetData.GetTagValue(FBlueprintTags::GeneratedClassPath, AssetClassName))
	{
		ConstructorHelpers::StripObjectClass(AssetClassName);
		AssetClassName = FPackageName::ObjectPathToObjectName(*AssetClassName);

		TSharedPtr<FDialogueGraphNodeClassNode> Node = FindBaseClassNode(RootNode, AssetClassName);
		if (Node.IsValid() && Node->ParentNode.IsValid())
		{
			Node->ParentNode->SubNodes.RemoveSingleSwap(Node);
		}
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	if (!AssetRegistryModule.Get().IsLoadingAssets())
	{
		UpdateAvailableBlueprintClasses();
	}
}

void FDialogueGraphNodeClassHelper::InvalidateCache()
{
	RootNode.Reset();

	UpdateAvailableBlueprintClasses();
}

void FDialogueGraphNodeClassHelper::OnHotReload(bool bWasTriggeredAutomatically)
{
	InvalidateCache();
}

void FDialogueGraphNodeClassHelper::AddUnknownClass(const FDialogueGraphNodeClassData& ClassData)
{
	if (ClassData.IsBlueprint())
	{
		UnknownPackages.AddUnique(*ClassData.GetPackageName());
	}
}

bool FDialogueGraphNodeClassHelper::IsClassKnown(const FDialogueGraphNodeClassData& ClassData)
{
	return !ClassData.IsBlueprint() || !UnknownPackages.Contains(*ClassData.GetPackageName());
}

int32 FDialogueGraphNodeClassHelper::GetObservedBlueprintClassCount(UClass* BaseNativeClass)
{
	return BlueprintClassCount.FindRef(BaseNativeClass);
}

void FDialogueGraphNodeClassHelper::AddObservedBlueprintClasses(UClass* BaseNativeClass)
{
	BlueprintClassCount.Add(BaseNativeClass, 0);
}

void FDialogueGraphNodeClassHelper::UpdateAvailableBlueprintClasses()
{
	if (FModuleManager::Get().IsModuleLoaded(TEXT("AssetRegistry")))
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		const bool bSearchSubClasses = true;

		TArray<FName> ClassNames;
		TSet<FName> DerivedClassNames;

		for (TMap<UClass*, int32>::TIterator It(BlueprintClassCount); It; ++It)
		{
			ClassNames.Reset();
			ClassNames.Add(It.Key()->GetFName());

			DerivedClassNames.Empty(DerivedClassNames.Num());
			AssetRegistryModule.Get().GetDerivedClassNames(ClassNames, TSet<FName>(), DerivedClassNames);

			int32& Count = It.Value();
			Count = DerivedClassNames.Num();
		}
	}
}

TSharedPtr<FDialogueGraphNodeClassNode> FDialogueGraphNodeClassHelper::CreateClassDataNode(const struct FAssetData& AssetData)
{
	TSharedPtr<FDialogueGraphNodeClassNode> Node;
	FString AssetClassName;
	FString AssetParentClassName;
	if (AssetData.GetTagValue(FBlueprintTags::GeneratedClassPath, AssetClassName) && AssetData.GetTagValue(FBlueprintTags::ParentClassPath, AssetParentClassName))
	{
		UObject* Outer1(NULL);
		ResolveName(Outer1, AssetClassName, false, false);

		UObject* Outer2(NULL);
		ResolveName(Outer2, AssetParentClassName, false, false);

		Node = MakeShareable(new FDialogueGraphNodeClassNode);
		Node->ParentClassName = AssetParentClassName;

		UObject* AssetOb = AssetData.IsAssetLoaded() ? AssetData.GetAsset() : NULL;
		UBlueprint* AssetBP = Cast<UBlueprint>(AssetOb);

		UClass* AssetClass = AssetBP ? *AssetBP->GeneratedClass : AssetBP ? AssetOb->GetClass() : nullptr;

		FDialogueGraphNodeClassData NewData(AssetData.AssetName.ToString(), AssetData.PackageName.ToString(), AssetClassName, AssetClass);
		Node->Data = NewData;
	}
	return Node;
}

TSharedPtr<FDialogueGraphNodeClassNode> FDialogueGraphNodeClassHelper::FindBaseClassNode(TSharedPtr<FDialogueGraphNodeClassNode> Node, const FString& ClassName)
{
	TSharedPtr<FDialogueGraphNodeClassNode> RetNode;
	if (Node.IsValid())
	{
		if (Node->Data.GetClassName() == ClassName)
		{
			return Node;
		}
		for (int32 i = 0 ;i<Node->SubNodes.Num();i++)
		{
			RetNode = FindBaseClassNode(Node->SubNodes[i], ClassName);
			if (RetNode.IsValid())
			{
				break;
			}
		}
	}
	return RetNode;
}

void FDialogueGraphNodeClassHelper::FindAllSubClasses(TSharedPtr<FDialogueGraphNodeClassNode> Node, TArray<FDialogueGraphNodeClassData>& AvailableClasses)
{
	if (Node.IsValid())
	{
		if (!Node->Data.IsAbstract()&&!Node->Data.IsDeprecated()&&!Node->Data.bIsHidden)
		{
			AvailableClasses.Add(Node->Data);
		}
		for (int32 i = 0 ;i<Node->SubNodes.Num();i++)
		{
			FindAllSubClasses(Node->SubNodes[i], AvailableClasses);
		}
	}
}

UClass* FDialogueGraphNodeClassHelper::FindAssetClass(const FString& GeneratedClassPackage, const FString& AssetName)
{
	UPackage* Package = FindPackage(NULL, *GeneratedClassPackage);
	if (Package)
	{
		UObject* Object = FindObject<UObject>(Package, *AssetName);
		if (Object)
		{
			UBlueprint* BlueprintOb = Cast<UBlueprint>(Object);
			return BlueprintOb ? *BlueprintOb->GeneratedClass : Object->GetClass();
		}
	}
	return nullptr;
}

void FDialogueGraphNodeClassHelper::BuildClassGraph()
{
	TArray<TSharedPtr<FDialogueGraphNodeClassNode>> NodeList;
	TArray<UClass*> HideParentList;
	RootNode.Reset();

	for (TObjectIterator<UClass> It;It;++It)
	{
		UClass* TestClass = *It;
		if (TestClass->HasAnyClassFlags(CLASS_Native) && TestClass->IsChildOf(RootNodeClass))
		{
			TSharedPtr<FDialogueGraphNodeClassNode> NewNode = MakeShareable(new FDialogueGraphNodeClassNode);
			NewNode->ParentClassName = TestClass->GetSuperClass()->GetName();
			FString DeprecatedMessage = GetDeprecationMessage(TestClass);
			FDialogueGraphNodeClassData NewData(TestClass, DeprecatedMessage);
			NewData.bHideParent = IsHidingParentClass(TestClass);
			if (NewData.bHideParent)
			{
				HideParentList.Add(TestClass->GetSuperClass());
			}
			NewData.bIsHidden = IsHidingClass(TestClass);
			NewNode->Data = NewData;
			if (TestClass == RootNodeClass)
			{
				RootNode = NewNode;
			}
			NodeList.Add(NewNode);
		}
	}

	for (int32 i = 0 ;i<NodeList.Num();i++)
	{
		TSharedPtr<FDialogueGraphNodeClassNode> TestNode = NodeList[i];
		if (HideParentList.Contains(TestNode->Data.GetClass()))
		{
			TestNode->Data.bIsHidden = true;
		}
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	TArray<FAssetData> BlueprintList;

	FARFilter filter;
	filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
	AssetRegistryModule.Get().GetAssets(filter, BlueprintList);

	for (int32 i = 0 ;i<BlueprintList.Num();i++)
	{
		TSharedPtr<FDialogueGraphNodeClassNode> NewNode = CreateClassDataNode(BlueprintList[i]);
		NodeList.Add(NewNode);
	}

	AddClassGraphChildren(RootNode, NodeList);
}

void FDialogueGraphNodeClassHelper::AddClassGraphChildren(TSharedPtr<FDialogueGraphNodeClassNode> Node, TArray<TSharedPtr<FDialogueGraphNodeClassNode> >& NodeList)
{
	if (!Node.IsValid())
	{
		return;
	}
	const FString NodeClassName = Node->Data.GetClassName();
	for (int32 i = NodeList.Num() - 1;i>=0;i--)
	{
		if (NodeList[i]->ParentClassName == NodeClassName)
		{
			TSharedPtr<FDialogueGraphNodeClassNode> MatchingNode = NodeList[i];
			NodeList.RemoveAt(i);
			MatchingNode->ParentNode = Node;
			Node->SubNodes.Add(MatchingNode);
			AddClassGraphChildren(MatchingNode, NodeList);
		}
	}
}

bool FDialogueGraphNodeClassHelper::IsHidingClass(UClass* Class)
{
	static FName DialogueMetaHideParent = TEXT("HideParentNode");
	return Class && Class->HasAnyClassFlags(CLASS_Native) && Class->HasMetaData(DialogueMetaHideParent);
}

bool FDialogueGraphNodeClassHelper::IsHidingParentClass(UClass* Class)
{
	static FName MetaHideInEditor = TEXT("HiddenNode");
	return Class && Class->HasAnyClassFlags(CLASS_Native) && Class->HasMetaData(MetaHideInEditor);
}

bool FDialogueGraphNodeClassHelper::IsPackageSaved(FName PackageName)
{
	const bool bFound = FPackageName::SearchForPackageOnDisk(PackageName.ToString());
	return bFound;
}

#undef LOCTEXT_NAMESPACE
