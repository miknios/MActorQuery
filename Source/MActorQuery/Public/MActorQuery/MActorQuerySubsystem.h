// Copyright ZAKAZANE Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MActorQueryComponent.h"
#include "MActorQueryTypes.h"

#include "MActorQuerySubsystem.generated.h"

struct FMActorQueryInput;

USTRUCT(BlueprintType)
struct FMOnQueryableActorRegisteredData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UMActorQueryComponent> ActorQueryComponent;

	FMOnQueryableActorRegisteredData()
	{
	}

	FMOnQueryableActorRegisteredData(const TWeakObjectPtr<UMActorQueryComponent>& ActorQueryComponent)
		: ActorQueryComponent(ActorQueryComponent)
	{
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FMOnQueryableActorRegisteredSignature, FMOnQueryableActorRegisteredData, OnQueryableActorRegisteredData);

DECLARE_MULTICAST_DELEGATE_OneParam(FMActorQueryComponentOnCreatedSignature, UMActorQueryComponent*)
DECLARE_MULTICAST_DELEGATE_OneParam(FMActorQueryComponentOnDestroyedSignature, UMActorQueryComponent*)

UCLASS()
class MACTORQUERY_API UMActorQuerySubsystem final : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, DisplayName = "Get Actor Query Subsystem",
		meta = (WorldContext = "_WorldContextObject"))
	static UMActorQuerySubsystem* Get(const UObject* WorldContextObject)
	{
		if (IsValid(WorldContextObject))
		{
			UWorld* World = WorldContextObject->GetWorld();
			if (IsValid(World))
			{
				return World->GetSubsystem<UMActorQuerySubsystem>();
			}
		}

		return nullptr;
	}

#if WITH_EDITOR
	static UMActorQuerySubsystem* GetEditorSubsystem()
	{
		if (IsValid(GEditor))
		{
			UWorld* EditorContextWorld = GEditor->GetEditorWorldContext().World();
			if (IsValid(EditorContextWorld))
			{
				return EditorContextWorld->GetSubsystem<UMActorQuerySubsystem>();
			}
		}

		return nullptr;
	}
#endif

	UMActorQuerySubsystem();

	// ~ USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// ~ USubsystem

	UFUNCTION(BlueprintCallable)
	void QueryActors(const FMActorQueryInput& QueryInput, TArray<AActor*>& OutActors) const;

	UFUNCTION(BlueprintCallable)
	AActor* QueryActor(const FMActorQueryInput& QueryInput) const;

	void RegisterQueryComponent(UMActorQueryComponent* QueryComponent);

	void UnregisterQueryComponent(UMActorQueryComponent* QueryComponent);

	template <typename T>
	T* GetActorOfClass(FGameplayTag _ActorTag)
	{
		AActor* actor = QueryActor(FMActorQueryInput::GetActorForTag(_ActorTag));
		return Cast<T>(actor);
	};

private:
	void FindEntriesMatchingTag(
		const FGameplayTag& QueryTag,
		const bool bIncludeChildrenTags,
		TArray<TWeakObjectPtr<UMActorQueryComponent>>& OutComponents) const;

public:
	UPROPERTY(BlueprintAssignable)
	FMOnQueryableActorRegisteredSignature OnQueryableActorRegisteredDelegate;

	UPROPERTY(BlueprintAssignable)
	FMOnQueryableActorRegisteredSignature OnQueryableActorUnregisteredDelegate;

private:
	TMultiMap<FGameplayTag, TWeakObjectPtr<UMActorQueryComponent>> Registry;
};
