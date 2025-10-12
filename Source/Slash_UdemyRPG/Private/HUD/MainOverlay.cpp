// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainOverlay.h"

#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

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

void UMainOverlay::PlayButton()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	TitleOut();
	Play->SetVisibility(ESlateVisibility::Hidden);
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->bShowMouseCursor = false;
	}
}

void UMainOverlay::DungeonTransition()
{
	FadeToBlack();
	FTimerHandle TimerHandle;
	if (UWorld* World = GetWorld()) World->GetTimerManager().SetTimer(TimerHandle, this, &UMainOverlay::FadeFromBlack, 0.5f);
}

void UMainOverlay::Dead()
{
	FTimerHandle BlackTimerHandle;
	FTimerHandle TitleTimerHandle;
	FTimerHandle RetryTimerHandle;
	if (UWorld* World = GetWorld()) World->GetTimerManager().SetTimer(BlackTimerHandle, this, &UMainOverlay::FadeToBlack, 2.f);
	if (UWorld* World = GetWorld()) World->GetTimerManager().SetTimer(TitleTimerHandle, this, &UMainOverlay::TitleIn, 3.f);
	if (UWorld* World = GetWorld()) World->GetTimerManager().SetTimer(RetryTimerHandle, this, &UMainOverlay::ShowRetry, 4.f);
}

void UMainOverlay::Complete()
{
	FTimerHandle BlackTimerHandle;
	FTimerHandle TitleTimerHandle;
	FTimerHandle RestartTimerHandle;
	FTimerHandle WinTextTimerHandle;
	if (UWorld* World = GetWorld()) World->GetTimerManager().SetTimer(BlackTimerHandle, this, &UMainOverlay::FadeToBlack, 5.f);
	if (UWorld* World = GetWorld()) World->GetTimerManager().SetTimer(TitleTimerHandle, this, &UMainOverlay::TitleIn, 6.f);
	if (UWorld* World = GetWorld()) World->GetTimerManager().SetTimer(RestartTimerHandle, this, &UMainOverlay::ShowRestart, 7.f);
	if (UWorld* World = GetWorld()) World->GetTimerManager().SetTimer(WinTextTimerHandle, this, &UMainOverlay::WinText, 7.f);
}

void UMainOverlay::FadeToBlack()
{
	if (BlackScreen && FadeToBlackAnim)
	{
		PlayAnimation(FadeToBlackAnim);
	}
}

void UMainOverlay::FadeFromBlack()
{
	if (BlackScreen && FadeFromBlackAnim)
	{
		PlayAnimation(FadeFromBlackAnim);
	}
}

void UMainOverlay::TitleIn()
{
	if (TitleScreen && TitleInAnim)
	{
		PlayAnimation(TitleInAnim);
	}
}

void UMainOverlay::TitleOut()
{
	if (TitleScreen && TitleOutAnim)
	{
		PlayAnimation(TitleOutAnim);
	}
}

void UMainOverlay::ShowRetry()
{
	if (Retry)
	{
		Retry->SetVisibility(ESlateVisibility::Visible);
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->bShowMouseCursor = true;
		}
	}
}

void UMainOverlay::ShowRestart()
{
	if (Restart)
	{
		Restart->SetVisibility(ESlateVisibility::Visible);
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->bShowMouseCursor = true;
		}
	}
}

void UMainOverlay::WinText()
{
	if (WinTextBlock)
	{
		WinTextBlock->SetVisibility(ESlateVisibility::Visible);
	}
}
