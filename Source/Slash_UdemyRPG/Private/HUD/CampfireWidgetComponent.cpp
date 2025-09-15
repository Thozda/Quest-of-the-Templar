// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CampfireWidgetComponent.h"

#include "Camera/CameraComponent.h"
#include "Characters/Knight.h"
#include "Components/TextBlock.h"
#include "HUD/CampfireWidget.h"

void UCampfireWidgetComponent::SetCampfireText(FString String)
{
	if (CampfireWidget == nullptr)
	{
		CampfireWidget = Cast<UCampfireWidget>(GetUserWidgetObject());
	}
	if (CampfireWidget && CampfireWidget->Text)
	{
		CampfireWidget->Text->SetText(FText::FromString(String));
	}
}

void UCampfireWidgetComponent::FacePlayerCamera(UCameraComponent* Camera)
{
	if (Camera)
	{
		FVector FaceTarget = Camera->GetComponentLocation();
		FVector VectorToTarget = FaceTarget - GetComponentLocation();
		FRotator LookAtRotation = VectorToTarget.Rotation();
		SetWorldRotation(LookAtRotation);
	}
}
