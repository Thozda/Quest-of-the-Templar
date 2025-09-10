// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainHUD.h"
#include "HUD/MainOverlay.h"

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (OverlayClass && World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller)
		{
			Overlay = CreateWidget<UMainOverlay>(Controller, OverlayClass);
			Overlay->AddToViewport();
		}
	}
}