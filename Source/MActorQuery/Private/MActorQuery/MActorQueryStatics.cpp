// Copyright (c) Miknios. All rights reserved.

#include "MActorQuery/MActorQueryStatics.h"

#include "MActorQuery/MActorQueryTypes.h"

FMActorQueryInput UMActorQueryStatics::GetActorQueryInput_ActorForTag(FGameplayTag Tag)
{
	return FMActorQueryInput::GetActorForTag(Tag);
}

FMActorQueryInput UMActorQueryStatics::GetActorQueryInput_ActorsForTag(FGameplayTag Tag)
{
	return FMActorQueryInput::GetActorsForTag(Tag);
}
