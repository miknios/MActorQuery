// Copyright (c) Miknios. All rights reserved.

#include "MActorQuery/MActorQuerySubsystem.h"

UMActorQuerySubsystem::UMActorQuerySubsystem()
{
}

void UMActorQuerySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UMActorQuerySubsystem::QueryActors(const FMActorQueryInput& QueryInput, TArray<AActor*>& OutActors) const
{
	TArray<TWeakObjectPtr<UMActorQueryComponent>> ComponentsFound;

	if (!QueryInput.bUseTagContainer)
	{
		FindEntriesMatchingTag(QueryInput.QueryTag, QueryInput.bIncludeChildrenTags, ComponentsFound);
	}
	else // Using Tag Container
	{
		if (QueryInput.QueryMatchType == EMQueryMatchType::Any)
		{
			for (const FGameplayTag& Tag : QueryInput.QueryTagContainer)
			{
				FindEntriesMatchingTag(Tag, QueryInput.bIncludeChildrenTags, ComponentsFound);
			}
		}
		else // EWestQueryMatchType::All
		{
			TArray<TWeakObjectPtr<UMActorQueryComponent>> ComponentsWithAnyTag;
			for (const FGameplayTag& Tag : QueryInput.QueryTagContainer)
			{
				FindEntriesMatchingTag(Tag, QueryInput.bIncludeChildrenTags, ComponentsWithAnyTag);
			}

			for (const TWeakObjectPtr<UMActorQueryComponent>& Component : ComponentsWithAnyTag)
			{
				if (Component.IsValid() && Component->GetTagContainer().HasAllExact(QueryInput.QueryTagContainer))
				{
					ComponentsFound.Emplace(Component);
				}
			}
		}
	}

	// Filter out by additional conditions and add to final collection
	for (int32 i = 0; i < ComponentsFound.Num(); ++i)
	{
		TWeakObjectPtr<AActor> Actor = ComponentsFound[i]->GetOwner();

		if (QueryInput.bMatchActorClass && !Actor->GetClass()->IsChildOf(QueryInput.ActorClassToMatch))
			continue;

		OutActors.Emplace(Actor.Get());
	}
}

AActor* UMActorQuerySubsystem::QueryActor(const FMActorQueryInput& QueryInput) const
{
	TArray<AActor*> Actors;
	QueryActors(QueryInput, Actors);

	if (!Actors.IsEmpty())
		return Actors[0];

	return nullptr;
}

void UMActorQuerySubsystem::RegisterQueryComponent(UMActorQueryComponent* QueryComponent)
{
	bool bRegistered = false;
	for (const FGameplayTag& Tag : QueryComponent->GetTagContainer())
	{
		if (Tag.IsValid())
		{
			Registry.Emplace(Tag, QueryComponent);
			bRegistered = true;
		}
	}

	if (bRegistered)
		OnQueryableActorRegisteredDelegate.Broadcast(FMOnQueryableActorRegisteredData(QueryComponent));
}

void UMActorQuerySubsystem::UnregisterQueryComponent(UMActorQueryComponent* QueryComponent)
{
	bool bUnregistered = false;
	for (const FGameplayTag& Tag : QueryComponent->GetTagContainer())
	{
		if (Tag.IsValid())
		{
			Registry.Remove(Tag, QueryComponent);
			bUnregistered = true;
		}
	}

	if (bUnregistered)
		OnQueryableActorUnregisteredDelegate.Broadcast(FMOnQueryableActorRegisteredData(QueryComponent));
}

void UMActorQuerySubsystem::FindEntriesMatchingTag(
	const FGameplayTag& QueryTag,
	const bool bIncludeChildrenTags,
	TArray<TWeakObjectPtr<UMActorQueryComponent>>& OutComponents) const
{
	if (!bIncludeChildrenTags)
	{
		Registry.MultiFind(QueryTag, OutComponents);
	}
	else
	{
		for (TMultiMap<FGameplayTag, TWeakObjectPtr<UMActorQueryComponent>>::TConstIterator It(Registry); It; ++It)
		{
			if (It.Key().MatchesTag(QueryTag))
				OutComponents.Emplace(It.Value());
		}
	}
}
