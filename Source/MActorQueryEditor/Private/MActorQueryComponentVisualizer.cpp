// Copyright (c) Miknios. All rights reserved.

#include "MActorQueryComponentVisualizer.h"

#include "CanvasTypes.h"
#include "MActorQuery/MActorQueryComponent.h"

TAutoConsoleVariable<bool> CVar_ActorQuery_DrawQueryTagsInViewport(
	TEXT("m.ActorQuery.DrawQueryTagsInViewport"),
	true,
	TEXT("Keep it disabled if you need better performance while actors with Query Component are selected"));

void FMActorQueryComponentVisualizer::DrawVisualizationHUD(
	const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	if (Component == nullptr)
		return;

	if (CVar_ActorQuery_DrawQueryTagsInViewport.GetValueOnGameThread())
	{
		const FVector WorldLocation = Component->GetOwner()->GetActorLocation();

		FVector2D ScreenPosition;
		if (View->WorldToPixel(WorldLocation, ScreenPosition))
		{
			const float DPIScale = Canvas->GetDPIScale();
			ScreenPosition /= DPIScale;

			const UMActorQueryComponent* QueryComponent = Cast<UMActorQueryComponent>(Component);
			FString DebugString;
			for (auto Tag : QueryComponent->GetTagContainer())
				DebugString += FString::Printf(TEXT("%s\n"), *Tag.ToString());

			if (DebugString.Len() == 0)
				DebugString = TEXT("NO TAG");

			Canvas->DrawShadowedString(
				ScreenPosition.X, ScreenPosition.Y, *DebugString, GEngine->GetLargeFont(), FColor::White);
		}
	}
}
