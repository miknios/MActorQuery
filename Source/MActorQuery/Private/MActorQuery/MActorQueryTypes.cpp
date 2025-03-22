// Copyright ZAKAZANE Studio. All Rights Reserved.

#include "MActorQuery/MActorQueryTypes.h"

#include "GameplayTagsManager.h"

void FMActorQueryInput::SetMatchActorClass(TSubclassOf<AActor> InActorClassToMatch)
{
	bMatchActorClass = true;
	ActorClassToMatch = InActorClassToMatch;
}

bool FMActorQueryInput::Matches(const FGameplayTagContainer& ActorIdentityTags)
{
	bool bMatches;
	if (!bUseTagContainer)
	{
		if (bIncludeChildrenTags)
		{
			bMatches = QueryTag.MatchesAny(ActorIdentityTags);
		}
		else
		{
			bMatches = QueryTag.MatchesAnyExact(ActorIdentityTags);
		}
	}
	else
	{
		if (QueryMatchType == EMQueryMatchType::Any)
		{
			if (bIncludeChildrenTags)
			{
				bMatches = ActorIdentityTags.HasAny(QueryTagContainer);
			}
			else
			{
				bMatches = ActorIdentityTags.HasAnyExact(QueryTagContainer);
			}
		}
		else // QueryMatchType == EMQueryMatchType::All
		{
			if (bIncludeChildrenTags)
			{
				bMatches = ActorIdentityTags.HasAll(QueryTagContainer);
			}
			else
			{
				bMatches = ActorIdentityTags.HasAllExact(QueryTagContainer);
			}
		}
	}

	return bMatches;
}

bool FMActorQueryInput::IsValid() const
{
	bool bValid = true;
	if (bUseTagContainer)
		bValid &= QueryTagContainer.IsValid();
	else
		bValid &= QueryTag.IsValid();

	if (bMatchActorClass)
		bValid &= ActorClassToMatch != nullptr;

	return bValid;
}

FGameplayTagContainer FMActorQueryInput::GetMatchedTags() const
{
	FGameplayTagContainer Tags;
	if (bUseTagContainer)
		Tags = QueryTagContainer;
	else
		Tags.AddTag(QueryTag);

	if (bIncludeChildrenTags)
	{
		TArray<FGameplayTag> TagArray = Tags.GetGameplayTagArray();

		const UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
		for (const FGameplayTag& Tag : TagArray)
			Tags.AppendTags(TagsManager.RequestGameplayTagChildren(Tag));
	}

	return Tags;
}

FMActorQueryInput FMActorQueryInput::GetActorForTag(FGameplayTag Tag)
{
	FMActorQueryInput Result;
	Result.QueryTag = Tag;
	Result.bUseTagContainer = false;
	return Result;
}

FMActorQueryInput FMActorQueryInput::GetActorsForTag(FGameplayTag Tag)
{
	FMActorQueryInput Result;
	Result.QueryTag = Tag;
	Result.bUseTagContainer = false;
	return Result;
}

/*FString FMActorQueryInput::GetShortName() const
{
	if (bUseTagContainer)
	{
		FString List;
		TArray<FGameplayTag> Tags = QueryTagContainer.GetGameplayTagArray();
		for (auto Tag : Tags)
		{
			if (!List.IsEmpty())
				List.Append(TEXT(","));
			List.Append(UMStringStatics::GetTagShortName(Tag));
		}
		return List;
	}
	else
		return UMStringStatics::GetTagShortName(QueryTag);
}*/

FString FMActorQueryInput::ToString() const
{
	if (bUseTagContainer)
	{
		return QueryTagContainer.ToString();
	}

	return QueryTag.ToString();
}
