// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/SlashGameMode.h"

#include "Components/SaveSystemComponent.h"
#include "Kismet/GameplayStatics.h"

ASlashGameMode::ASlashGameMode()
{
	SaveSystemComponent = CreateDefaultSubobject<USaveSystemComponent>(TEXT("Save System"));
}

void ASlashGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (SaveSystemComponent)
	{
		SaveSystemComponent->LoadGame();
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->bShowMouseCursor = true;
	}

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);

}
