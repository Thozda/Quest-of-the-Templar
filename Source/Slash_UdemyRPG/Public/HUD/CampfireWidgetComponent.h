// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "CampfireWidgetComponent.generated.h"

class AKnight;
class UCampfireWidget;
/**
 * 
 */
UCLASS()
class SLASH_UDEMYRPG_API UCampfireWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	void SetCampfireText(FString String);
	void FacePlayerCamera(AKnight* OtherActor);
	
private:
	UPROPERTY()
	UCampfireWidget* CampfireWidget;
};
