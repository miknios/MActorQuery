// Copyright ZAKAZANE Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "MActorQueryStatics.generated.h"

struct FGameplayTag;
struct FMActorQueryInput;
/**
 * 
 */
UCLASS(meta = (ScriptName = "ActorQueryLibrary"))
class MACTORQUERY_API UMActorQueryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Actor Query Input (Actor For Tag)")
	static FMActorQueryInput GetActorQueryInput_ActorForTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Actor Query Input (Actors For Tag)")
	static FMActorQueryInput GetActorQueryInput_ActorsForTag(FGameplayTag Tag);
};
