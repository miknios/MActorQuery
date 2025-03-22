// Copyright (c) Miknios. All rights reserved.

#include "MActorQuery/MActorQueryComponent.h"

#include "MActorQuery/MActorQuerySubsystem.h"

UMActorQueryComponent::UMActorQueryComponent()
{
}

void UMActorQueryComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR
	// Register using OnRegister/OnUnregister only for actors contained in Editor World, so they are available for query for editor tools
	UWorld* World = GetWorld();
	if (World == GEditor->GetEditorWorldContext().World())
	{
		UMActorQuerySubsystem* ActorQuerySubsystem = UMActorQuerySubsystem::Get(this);
		ActorQuerySubsystem->RegisterQueryComponent(this);
	}
#endif
}

void UMActorQueryComponent::OnUnregister()
{
	Super::OnUnregister();

#if WITH_EDITOR
	UWorld* World = GetWorld();
	if (World == GEditor->GetEditorWorldContext().World())
	{
		UMActorQuerySubsystem* ActorQuerySubsystem = UMActorQuerySubsystem::Get(this);
		ActorQuerySubsystem->RegisterQueryComponent(this);
	}
#endif
}

FGameplayTag UMActorQueryComponent::GetPrimaryTag() const
{
	return TagContainer.First();
}

FGameplayTagContainer UMActorQueryComponent::GetTagContainer() const
{
	return TagContainer;
}

void UMActorQueryComponent::RegisterQueryComponent()
{
	const UWorld* World = GetWorld();
	if (UMActorQuerySubsystem* QuerySubsystem = UMActorQuerySubsystem::Get(World))
	{
		QuerySubsystem->RegisterQueryComponent(this);
	}
}

void UMActorQueryComponent::UnregisterQueryComponent()
{
	const UWorld* World = GetWorld();
	if (UMActorQuerySubsystem* QuerySubsystem = UMActorQuerySubsystem::Get(World))
	{
		QuerySubsystem->UnregisterQueryComponent(this);
	}
}

void UMActorQueryComponent::SetPrimaryTag(const FGameplayTag& PrimaryTag)
{
	if (TagContainer.Num() > 0)
	{
		// Get array of tags and change first element
		TArray<FGameplayTag> Tags;
		TagContainer.GetGameplayTagArray(Tags);

		Tags[0] = PrimaryTag;
		TagContainer.Reset();
		for (FGameplayTag Tag : Tags)
		{
			TagContainer.AddTag(Tag);
		}
	}
	else // TagContainer.Num == 0
	{
		TagContainer.AddTag(PrimaryTag);
	}
}

void UMActorQueryComponent::SetTagContainer(const FGameplayTagContainer& InTagContainer)
{
	TagContainer = InTagContainer;
}

void UMActorQueryComponent::AddTag(const FGameplayTag& TagToAdd)
{
	TagContainer.AddTag(TagToAdd);
}

void UMActorQueryComponent::RemoveTag(const FGameplayTag& TagToRemove)
{
	TagContainer.RemoveTag(TagToRemove);
}

void UMActorQueryComponent::BeginPlay()
{
	Super::BeginPlay();

	RegisterQueryComponent();
}

void UMActorQueryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Destroyed || EndPlayReason == EEndPlayReason::RemovedFromWorld
		|| EndPlayReason == EEndPlayReason::LevelTransition)
	{
		UnregisterQueryComponent();
	}
}
