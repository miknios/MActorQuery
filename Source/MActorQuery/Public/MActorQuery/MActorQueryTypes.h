// Copyright ZAKAZANE Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "UObject/Object.h"

#include "MActorQueryTypes.generated.h"

UENUM(BlueprintType)
enum class EMQueryMatchType : uint8
{
	// Actors matching with any tags with QueryTag will be added
	Any,

	// Actors with all matching tags with QueryTag will be added
	All
};

USTRUCT(BlueprintType)
struct MACTORQUERY_API FMActorQueryInput
{
	GENERATED_BODY()

	// Enable for multiple tag selection (slower)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseTagContainer = false;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		meta = (WorldGameplayTag, EditCondition = "!bUseTagContainer", EditConditionHides))
	FGameplayTag QueryTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseTagContainer", EditConditionHides))
	FGameplayTagContainer QueryTagContainer = FGameplayTagContainer::EmptyContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseTagContainer", EditConditionHides))
	EMQueryMatchType QueryMatchType = EMQueryMatchType::Any;

	// Should we include actors that match any children of provided tag (slower) or do we want tags to match exactly (much faster)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	bool bIncludeChildrenTags = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	bool bMatchActorClass = false;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		AdvancedDisplay,
		meta = (EditCondition = "bMatchActorClass", EditConditionHides))
	TSubclassOf<AActor> ActorClassToMatch;

	void SetMatchActorClass(TSubclassOf<AActor> InActorClassToMatch);

	bool Matches(const FGameplayTagContainer& ActorIdentityTags);

	bool IsValid() const;

	FGameplayTagContainer GetMatchedTags() const;

	/* Start helper functions */
	static FMActorQueryInput GetActorForTag(FGameplayTag Tag);
	static FMActorQueryInput GetActorsForTag(FGameplayTag Tag);
	/* End helper functions */

	// FString GetShortName() const;

	FString ToString() const;
};
