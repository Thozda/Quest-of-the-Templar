// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SlashGameMode.generated.h"

class USaveSystemComponent;
/**
 * 
 */
UCLASS()
class SLASH_UDEMYRPG_API ASlashGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ASlashGameMode();
	
protected:
	USaveSystemComponent* SaveSystemComponent;
};
