// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Enemy/Enemy.h"
#include "GameFramework/SaveGame.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Volumes/BossArena.h"
#include "AutoSaveGame.generated.h"

class ABossArena;
class AEnemy;
class AItem;
class AWeapon;
enum class EEnemyState : uint8;
enum class EActionState : uint8;
enum class ECharacterState : uint8;

/**
 * Enemy Save Data Structure
 */
USTRUCT(BlueprintType)
struct FEnemySaveData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    FVector Location;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    FRotator Rotation;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    float Health;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    FSoftClassPath EnemyClass;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    EEnemyState EnemyState;

    // Store weapon as string identifier instead of pointer
    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    FSoftClassPath WeaponType;
    
    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    TArray<FSoftObjectPath> PatrolPoints;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    FString PatrolPointName;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    int32 PatrolPointIndex;

    // Store timer remaining times instead of handles
    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    float PatrolTimerRemaining;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    float AttackTimerRemaining;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    float AttackResetTimerRemaining;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")    
    int32 CurrentAttackIndex = 0;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    float ComboResetTimerRemaining;

    // Additional useful data
    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    bool bHasPatrolTarget;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    bool bIsPatrolling;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    bool bIsBoss;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    FSoftObjectPath BossArena;

    UPROPERTY(VisibleAnywhere, Category = "Enemy")
    bool bIsDead;

    FEnemySaveData()
    {
        Location = FVector::ZeroVector;
        Rotation = FRotator::ZeroRotator;
        Health = 100.0f;
        EnemyClass = AEnemy::StaticClass();
        EnemyState = EEnemyState::EES_NoState;
        WeaponType = AWeapon::StaticClass();
        PatrolPointName = TEXT("");
        PatrolPointIndex = 0;
        PatrolTimerRemaining = 0.0f;
        AttackTimerRemaining = 0.0f;
        AttackResetTimerRemaining = 0.0f;
        CurrentAttackIndex = 0;
        ComboResetTimerRemaining = 0.0f;
        bHasPatrolTarget = false;
        bIsPatrolling = false;
        bIsBoss = false;
        bIsDead = false;
    }
};

/**
 * Treasure Save Data Structure
 */
USTRUCT(BlueprintType)
struct FTreasureSaveData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Treasure")
    FVector Location;

    // Store as string for reliable serialization
    UPROPERTY(VisibleAnywhere, Category = "Treasure")
    FSoftClassPath TreasureClass;

    UPROPERTY(VisibleAnywhere, Category = "Treasure")
    int32 Value;

    FTreasureSaveData()
    {
        Location = FVector::ZeroVector;
        TreasureClass = AItem::StaticClass();
        Value = 0;
    }
};

/**
 * Main Save Game Class
 */
UCLASS()
class SLASH_UDEMYRPG_API UAutoSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UAutoSaveGame();

    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString SaveSlotName;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    int32 UserIndex;

    //
    // Player Data
    //
    UPROPERTY(VisibleAnywhere, Category = "Player")
    FVector PlayerLocation;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    FRotator PlayerRotation;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    ECharacterState PlayerCharacterState;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    EActionState PlayerActionState;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    FSoftClassPath PlayerWeapon;
    
    UPROPERTY(VisibleAnywhere, Category = "Player")
    int32 PlayerLevel;
    
    UPROPERTY(VisibleAnywhere, Category = "Player")
    float PlayerHealth;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    float PlayerMaxHealth;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    float PlayerStamina;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    float PlayerMaxStamina;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    float PlayerStaminaRegenRate;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    int32 PlayerNextUpgradeGoldCost;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    int32 PlayerNextUpgradeSoulCost;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    int32 PlayerGold;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    int32 PlayerSouls;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    int32 PlayerKeys;

    UPROPERTY(VisibleAnywhere, Category = "Player")
    float PlayerLightDamageMultiplier;
	
    UPROPERTY(VisibleAnywhere, Category = "Player")
    float PlayerHeavyDamageMultiplier;
    
    //
    // Enemy Data
    //
    UPROPERTY(VisibleAnywhere, Category = "Enemies")
    TArray<FEnemySaveData> EnemyData;

    //
    // Treasure Data
    //
    UPROPERTY(VisibleAnywhere, Category = "Treasure")
    TArray<FTreasureSaveData> TreasureData;

    //
    // Level/World Data (optional but useful)
    //
    UPROPERTY(VisibleAnywhere, Category = "World")
    FDateTime SaveDateTime;
};