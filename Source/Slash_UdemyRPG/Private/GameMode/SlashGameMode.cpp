// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/SlashGameMode.h"

#include "Components/SaveSystemComponent.h"

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
}
