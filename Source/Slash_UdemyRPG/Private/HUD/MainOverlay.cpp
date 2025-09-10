// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMainOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}
}

void UMainOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaBar)
	{
		StaminaBar->SetPercent(Percent);
	}
}

void UMainOverlay::SetGoldText(int32 Quantity)
{
	if (Gold)
	{
		Gold->SetText(FText::FromString(FString::Printf(TEXT("%d"), Quantity)));
	}
}

void UMainOverlay::SetSoulsText(int32 Quantity)
{
	if (Souls)
	{
		Souls->SetText(FText::FromString(FString::Printf(TEXT("%d"), Quantity)));
	}
}