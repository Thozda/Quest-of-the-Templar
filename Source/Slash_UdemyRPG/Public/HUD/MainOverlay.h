// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainOverlay.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS()
class SLASH_UDEMYRPG_API UMainOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGoldText(int32 Quantity);
	void SetSoulsText(int32 Quantity);

private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Gold;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Souls;

};
