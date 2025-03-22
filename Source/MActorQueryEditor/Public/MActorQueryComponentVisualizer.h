// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ComponentVisualizer.h"

/**
 * 
 */
class FMActorQueryComponentVisualizer : public FComponentVisualizer
{
	virtual void DrawVisualizationHUD(
		const UActorComponent* Component,
		const FViewport* Viewport,
		const FSceneView* View,
		FCanvas* Canvas) override;
};
