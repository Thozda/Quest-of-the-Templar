// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainOverlay.generated.h"

class UImage;
class UButton;
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

	UFUNCTION(BlueprintCallable)
	void PlayButton();
	
	void DungeonTransition();
	void Dead();
	void Complete();

protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeToBlackAnim;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeFromBlackAnim;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* TitleInAnim;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* TitleOutAnim;
	
private:
	void FadeToBlack();
	void FadeFromBlack();
	void TitleIn();
	void TitleOut();
	void ShowRetry();
	void ShowRestart();
	void WinText();

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Gold;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Souls;

	UPROPERTY(meta = (BindWidget))
	UImage* BlackScreen;
	
	UPROPERTY(meta = (BindWidget))
	UImage* TitleScreen;

	UPROPERTY(meta = (BindWidget))
	UButton* Play;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Retry;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Restart;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WinTextBlock;

};
