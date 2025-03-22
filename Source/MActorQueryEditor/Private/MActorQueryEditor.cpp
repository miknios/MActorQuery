// Copyright (c) Miknios. All rights reserved.

#include "MActorQueryEditor.h"

#include "MActorQueryComponentVisualizer.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "MActorQuery/MActorQueryComponent.h"

#define LOCTEXT_NAMESPACE "FMActorQueryEditorModule"

void FMActorQueryEditorModule::StartupModule()
{
	if (GUnrealEd != nullptr)
	{
		GUnrealEd->RegisterComponentVisualizer(
			UMActorQueryComponent::StaticClass()->GetFName(),
			MakeShareable(new FMActorQueryComponentVisualizer));
	}
}

void FMActorQueryEditorModule::ShutdownModule()
{
	if (GUnrealEd != nullptr)
	{
		GUnrealEd->UnregisterComponentVisualizer(UMActorQueryComponent::StaticClass()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMActorQueryEditorModule, MActorQueryEditor)
