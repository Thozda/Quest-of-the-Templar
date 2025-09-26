// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/AutoSaveGame.h"

UAutoSaveGame::UAutoSaveGame()
{
	SaveSlotName = TEXT("GameSave");
	UserIndex = 0;
    
	// Initialize player data with defaults
	PlayerLocation = FVector::ZeroVector;
	PlayerRotation = FRotator::ZeroRotator;
	PlayerLevel = 1;
	PlayerHealth = 100.0f;
	PlayerMaxHealth = 100.0f;
	PlayerStamina = 100.0f;
	PlayerMaxStamina = 100.0f;
	PlayerStaminaRegenRate = 10.0f;
	PlayerNextUpgradeGoldCost = 100;
	PlayerNextUpgradeSoulCost = 1;
	PlayerGold = 0;
	PlayerSouls = 0;
	PlayerKeys = 0;
	PlayerLightDamageMultiplier = 1.0f;
	PlayerHeavyDamageMultiplier = 1.0f;
    
	// Initialize save timestamp
	SaveDateTime = FDateTime::Now();
}
