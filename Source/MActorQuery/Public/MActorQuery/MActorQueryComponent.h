// Copyright ZAKAZANE Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "MActorQueryComponent.generated.h"

struct FGameplayTagContainer;

UCLASS(ClassGroup = (ActorQuery), meta = (BlueprintSpawnableComponent))
class MACTORQUERY_API UMActorQueryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMActorQueryComponent();

	// ~ UActorComponent
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~ UActorComponent

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetPrimaryTag() const;

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetTagContainer() const;

	UFUNCTION(BlueprintCallable)
	void RegisterQueryComponent();

	UFUNCTION(BlueprintCallable)
	void UnregisterQueryComponent();

	UFUNCTION(BlueprintCallable)
	void SetPrimaryTag(const FGameplayTag& PrimaryTag);

	UFUNCTION(BlueprintCallable)
	void SetTagContainer(const FGameplayTagContainer& InTagContainer);

	UFUNCTION(BlueprintCallable)
	void AddTag(const FGameplayTag& TagToAdd);

	UFUNCTION(BlueprintCallable)
	void RemoveTag(const FGameplayTag& TagToRemove);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Query")
	FGameplayTagContainer TagContainer;
};
