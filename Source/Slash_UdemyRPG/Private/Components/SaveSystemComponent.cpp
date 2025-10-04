// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SaveSystemComponent.h"

#include "Characters/Knight.h"
#include "Components/AttributeComponent.h"
#include "Components/SphereComponent.h"
#include "Enemy/Enemy.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/AutoSaveGame.h"


// Sets default values for this component's properties
USaveSystemComponent::USaveSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void USaveSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void USaveSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// TickFalse
}

void USaveSystemComponent::SaveGame()
{
	UAutoSaveGame* SaveGameInstance = Cast<UAutoSaveGame>(UGameplayStatics::CreateSaveGameObject(UAutoSaveGame::StaticClass()));
    
	if (!SaveGameInstance) return;

	SaveGameInstance->SaveSlotName = TEXT("GameSave");
	SaveGameInstance->UserIndex = 0;

	// Save Player Data
	SavePlayerData(SaveGameInstance);
    
	// Save Enemy Data
	SaveEnemyData(SaveGameInstance);
    
	// Save Treasure Data
	SaveTreasureData(SaveGameInstance);
    
	// Perform the save
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
    
	UE_LOG(LogTemp, Warning, TEXT("Game Saved Successfully"));
}

void USaveSystemComponent::LoadGame()
{
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("GameSave"), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("No save file found"));
		return;
	}

	UAutoSaveGame* LoadedGame = Cast<UAutoSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("GameSave"), 0));
	if (!LoadedGame)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load save game"));
		return;
	}

	// Clear existing world state
	ClearEnemiesAndTreasure();

	// Load all data
	LoadPlayerData(LoadedGame);
	LoadEnemyData(LoadedGame);
	LoadTreasureData(LoadedGame);

	UE_LOG(LogTemp, Warning, TEXT("Game Loaded Successfully"));
}

bool USaveSystemComponent::DoesSaveExist()
{
	return UGameplayStatics::DoesSaveGameExist(TEXT("GameSave"), 0);
}

void USaveSystemComponent::DeleteSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("GameSave"), 0))
	{
		UGameplayStatics::DeleteGameInSlot(TEXT("GameSave"), 0);
		UE_LOG(LogTemp, Warning, TEXT("Save game deleted"));
	}
}

void USaveSystemComponent::SavePlayerData(UAutoSaveGame* SaveGame)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !PC->GetPawn()) return;

	APawn* Player = PC->GetPawn();
	AKnight* Knight = Cast<AKnight>(Player);
	if (Knight == nullptr || Knight->GetAttributes() == nullptr) return;
	UAttributeComponent* KnightAttributes = Knight->GetAttributes();
	
	SaveGame->PlayerLocation = Knight->GetActorLocation();
	SaveGame->PlayerRotation = Knight->GetActorRotation();
	SaveGame->PlayerCharacterState = Knight->GetCharacterState();
	SaveGame->PlayerActionState = Knight->GetActionState();
	if (Knight->GetEquippedWeapon()) SaveGame->PlayerWeapon = Knight->GetEquippedWeapon()->GetClass();
	SaveGame->PlayerLevel = KnightAttributes->GetLevel();
	SaveGame->PlayerHealth = KnightAttributes->GetHealth();
	SaveGame->PlayerMaxHealth = KnightAttributes->GetMaxHealth();
	SaveGame->PlayerStamina = KnightAttributes->GetStamina();
	SaveGame->PlayerMaxStamina = KnightAttributes->GetMaxStamina();
	SaveGame->PlayerStaminaRegenRate = KnightAttributes->GetStaminaRegenRate();
	SaveGame->PlayerNextUpgradeGoldCost = KnightAttributes->GetNextUpgradeGoldCost();
	SaveGame->PlayerNextUpgradeSoulCost = KnightAttributes->GetNextUpgradeSoulCost();
	SaveGame->PlayerGold = KnightAttributes->GetGold();
	SaveGame->PlayerSouls = KnightAttributes->GetSouls();
	SaveGame->PlayerKeys = Knight->GetKeysHeld();
	SaveGame->PlayerLightDamageMultiplier = KnightAttributes->GetLightDamageMultiplier();
	SaveGame->PlayerHeavyDamageMultiplier = KnightAttributes->GetHeavyDamageMultiplier();
}

void USaveSystemComponent::SaveEnemyData(UAutoSaveGame* SaveGame)
{
	SaveGame->EnemyData.Empty();

	TArray<AActor*> EnemiesFound;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), EnemiesFound);
	
	for (int32 i = 0; i < EnemiesFound.Num(); i++)
	{
		AActor* ActorObject = EnemiesFound[i];
		AEnemy* Enemy = Cast<AEnemy>(ActorObject);
		if (Enemy == nullptr || Enemy->GetAttributes() == nullptr) continue;
		
		UAttributeComponent* EnemyAttributes = Enemy->GetAttributes();
		FEnemySaveData EnemyData;

		EnemyData.Location = Enemy->GetActorLocation();
		EnemyData.Rotation = Enemy->GetActorRotation();
		EnemyData.Health = EnemyAttributes->GetHealth();
		if (Enemy->GetClass()) EnemyData.EnemyClass = Enemy->GetClass();
		EnemyData.EnemyState = Enemy->GetEnemyState();
		
		if (Enemy->GetWeapon() && Enemy->GetWeapon()->GetClass())
		{
			EnemyData.WeaponType = Enemy->GetWeapon()->GetClass();
		}
		if (Enemy->GetPatrolPoints().Num() > 0)
		{
			for (AActor* Point : Enemy->GetPatrolPoints())
			{
				if (Point) EnemyData.PatrolPoints.Add(Point);
			}
			UE_LOG(LogTemp, Warning, TEXT("Saving: %d, %d"), Enemy->GetPatrolPoints().Num(), EnemyData.PatrolPoints.Num())
		}
		if (Enemy->GetPatrolTarget())
		{
			EnemyData.bHasPatrolTarget = true;
			EnemyData.PatrolPointName = Enemy->GetPatrolTarget()->GetName();
		}
		else
		{
			EnemyData.bHasPatrolTarget = false;
		}
		
		EnemyData.PatrolPointIndex = Enemy->GetPatrolPointIndex();
		EnemyData.PatrolTimerRemaining = Enemy->GetPatrolTimerRemaining();
		EnemyData.AttackTimerRemaining = Enemy->GetAttackTimerRemaining();
		EnemyData.AttackResetTimerRemaining = Enemy->GetAttackResetTimerRemaining();
		EnemyData.CurrentAttackIndex = Enemy->GetCurrentAttackIndex();
		EnemyData.ComboResetTimerRemaining = Enemy->GetComboResetTimerRemaining();
		EnemyData.bIsPatrolling = Enemy->bIsPatrolling();
		EnemyData.bIsDead = Enemy->IsDead();
		EnemyData.bIsBoss = Enemy->IsBoss();
		EnemyData.BossArena = Enemy->GetBossArena();

		SaveGame->EnemyData.Add(EnemyData);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Saved %d enemies"), SaveGame->EnemyData.Num());
}

void USaveSystemComponent::SaveTreasureData(UAutoSaveGame* SaveGame)
{
	SaveGame->TreasureData.Empty();

	TArray<AActor*> ItemsFound;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItem::StaticClass(), ItemsFound);
	
	for (AActor* ActorObject : ItemsFound)
	{
		AItem* Item = Cast<AItem>(ActorObject);
		if (Item == nullptr) continue;

		FTreasureSaveData ItemData;
		ItemData.Location = Item->GetActorLocation();

		if (Item->GetClass())
		{
			ItemData.TreasureClass = Item->GetClass();
		}
		
		if (ATreasure* Treasure = Cast<ATreasure>(Item))
		{
			ItemData.Value = Treasure->GetGold();
		}
		else if (ASoul* Soul = Cast<ASoul>(Item))
		{
			ItemData.Value = Soul->GetSouls();
		}
		else if (AWeapon* Weapon = Cast<AWeapon>(Item))
		{
			if (Weapon->GetIsEquipped()) continue;
			ItemData.Value = 0;
		}

		SaveGame->TreasureData.Add(ItemData);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Saved %d items"), SaveGame->TreasureData.Num());
}

void USaveSystemComponent::LoadPlayerData(UAutoSaveGame* LoadedGame)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !PC->GetPawn()) return;

	APawn* Player = PC->GetPawn();
	AKnight* Knight = Cast<AKnight>(Player);
	if (Knight == nullptr || Knight->GetAttributes() == nullptr) return;

	UAttributeComponent* KnightAttributes = Knight->GetAttributes();
    
	// Load transform
	Knight->SetActorLocation(LoadedGame->PlayerLocation);
	Knight->SetActorRotation(LoadedGame->PlayerRotation);
    
	// Load character states
	Knight->SetCharacterState(LoadedGame->PlayerCharacterState);
	//Knight->SetActionState(LoadedGame->PlayerActionState);

	//Load Weapon
	UClass* WeaponClass = nullptr;
	if (UClass* LoadedClass = LoadedGame->PlayerWeapon.TryLoadClass<AWeapon>())
	{
		WeaponClass = LoadedClass;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load class: %s"), *LoadedGame->PlayerWeapon.GetAssetName())
	}
	AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, LoadedGame->PlayerLocation, LoadedGame->PlayerRotation);
	if (Weapon)
	{
		Knight->EquipWeapon(Weapon);
	}
	
	// Load attributes
	KnightAttributes->SetLevel(LoadedGame->PlayerLevel);
	KnightAttributes->SetHealth(LoadedGame->PlayerHealth);
	KnightAttributes->SetMaxHealth(LoadedGame->PlayerMaxHealth);
	KnightAttributes->SetStamina(LoadedGame->PlayerStamina);
	KnightAttributes->SetMaxStamina(LoadedGame->PlayerMaxStamina);
	KnightAttributes->SetStaminaRegenRate(LoadedGame->PlayerStaminaRegenRate);
	KnightAttributes->SetNextUpgradeGoldCost(LoadedGame->PlayerNextUpgradeGoldCost);
	KnightAttributes->SetNextUpgradeSoulCost(LoadedGame->PlayerNextUpgradeSoulCost);
	KnightAttributes->SetGold(LoadedGame->PlayerGold);
	KnightAttributes->SetSouls(LoadedGame->PlayerSouls);
	Knight->SetKeysHeld(LoadedGame->PlayerKeys);
	KnightAttributes->SetLightDamageMultiplier(LoadedGame->PlayerLightDamageMultiplier);
	KnightAttributes->SetHeavyDamageMultiplier(LoadedGame->PlayerHeavyDamageMultiplier);
}

void USaveSystemComponent::LoadEnemyData(UAutoSaveGame* LoadedGame)
{
	UWorld* World = GetWorld();
    if (!World) return;

    for (const FEnemySaveData& EnemyData : LoadedGame->EnemyData)
    {
        // Skip dead enemies if you don't want to respawn them
        if (EnemyData.bIsDead) continue;

        // Spawn enemy at saved location
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    	// Get the item class from the saved class name
    	UClass* EnemyClass = nullptr;

    	if (UClass* LoadedClass = EnemyData.EnemyClass.TryLoadClass<AEnemy>())
    	{
    		EnemyClass = LoadedClass;
    	}
    	else
    	{
    		UE_LOG(LogTemp, Warning, TEXT("Failed to load class: %s"), *EnemyData.EnemyClass.GetAssetName())
    	}
    	
    	if (!EnemyClass) continue;
        
		AEnemy* SpawnedEnemy = World->SpawnActor<AEnemy>(
            EnemyClass,
            EnemyData.Location, 
            EnemyData.Rotation, 
            SpawnParams
        );
    	
        if (SpawnedEnemy && SpawnedEnemy->GetAttributes())
        {
            // Restore enemy data
            SpawnedEnemy->GetAttributes()->SetHealth(EnemyData.Health);
            SpawnedEnemy->SetEnemyState(EnemyData.EnemyState);
            SpawnedEnemy->SetCurrentAttackIndex(EnemyData.CurrentAttackIndex);
            SpawnedEnemy->SetPatrolPointIndex(FMath::Clamp(EnemyData.PatrolPointIndex - 1, 0, EnemyData.PatrolPoints.Num() - 1));
        	

        	for (const FSoftObjectPath& Point : EnemyData.PatrolPoints)
        	{
        		if (AActor* Loaded = Cast<AActor>(Point.ResolveObject()))
        		{
        			SpawnedEnemy->AddPatrolTarget(Loaded);
        		}
        	}
        	
			//UE_LOG(LogTemp, Warning, TEXT("Loading: %d, %d"), SpawnedEnemy->GetPatrolPoints().Num(), EnemyData.PatrolPoints.Num())
        	
            /* Restore weapon if needed
			UClass* WeaponClass = nullptr;
        	if (UClass* LoadedClass = EnemyData.EnemyClass.TryLoadClass<AWeapon>())
        	{
        		WeaponClass = LoadedClass;
        	}
        	else
        	{
        		UE_LOG(LogTemp, Warning, TEXT("Failed to load class: %s"), *EnemyData.EnemyClass.GetAssetName())
        	}
        	
        	AWeapon* Weapon = World->SpawnActor<AWeapon>(WeaponClass, EnemyData.Location, EnemyData.Rotation);
        	if (Weapon)
        	{
        		Weapon->Equip(SpawnedEnemy->GetMesh(), SpawnedEnemy->GetWeaponSocket(), SpawnedEnemy, SpawnedEnemy);
        	}
            */
            // Restore patrol data
        	
        	// Restore timers with remaining time
        	SpawnedEnemy->RestoreTimers(
				EnemyData.PatrolTimerRemaining,
				EnemyData.AttackTimerRemaining,
				EnemyData.AttackResetTimerRemaining,
				EnemyData.ComboResetTimerRemaining
			);
            
            if (EnemyData.bHasPatrolTarget)
            {
                SpawnedEnemy->SetPatrolTarget(EnemyData.PatrolPointName);
            }

        	SpawnedEnemy->LoadedPatrolling();

        	if (EnemyData.bIsBoss)
        	{
        		SpawnedEnemy->SetIsBoss(true);
        		if (AActor* Loaded = Cast<AActor>(EnemyData.BossArena.ResolveObject()))
        		{
					SpawnedEnemy->SetBossArena(Loaded);
        		}
        	}
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Loaded %d enemies"), LoadedGame->EnemyData.Num());
}

void USaveSystemComponent::LoadTreasureData(UAutoSaveGame* LoadedGame)
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (const FTreasureSaveData& TreasureData : LoadedGame->TreasureData)
	{
		// Get the item class from the saved class name
		UClass* ItemClass = nullptr;
		
		if (UClass* LoadedClass = TreasureData.TreasureClass.TryLoadClass<AItem>())
		{
			ItemClass = LoadedClass;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load class: %s"), *TreasureData.TreasureClass.GetAssetName())
		}
		
		if (!ItemClass) continue;

		// Spawn item at saved location
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AItem* SpawnedItem = World->SpawnActor<AItem>(
			ItemClass, 
			TreasureData.Location, 
			FRotator::ZeroRotator, 
			SpawnParams
		);

		if (SpawnedItem)
		{
			// Set the value based on item type
			if (ATreasure* Treasure = Cast<ATreasure>(SpawnedItem))
			{
				Treasure->SetGold(TreasureData.Value);
			}
			else if (ASoul* Soul = Cast<ASoul>(SpawnedItem))
			{
				Soul->SetSouls(TreasureData.Value);
				if (Soul->GetSphere()) Soul->GetSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Loaded %d items"), LoadedGame->TreasureData.Num());
}

void USaveSystemComponent::ClearEnemiesAndTreasure()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// Clear all existing enemies
	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(World, AEnemy::StaticClass(), AllEnemies);
	for (AActor* Enemy : AllEnemies)
	{
		if (Enemy)
		{
			Enemy->Destroy();
		}
	}

	// Clear all existing items
	TArray<AActor*> AllItems;
	UGameplayStatics::GetAllActorsOfClass(World, AItem::StaticClass(), AllItems);
	for (AActor* Item : AllItems)
	{
		if (Item)
		{
			Item->Destroy();
		}
	}
}

FString USaveSystemComponent::GetWeaponTypeName(AWeapon* Weapon)
{
	if (!Weapon) return TEXT("");
    
	// Return the class name
	return Weapon->GetClass()->GetName();
}

AWeapon* USaveSystemComponent::CreateWeaponFromName(const FString& WeaponName)
{
	if (WeaponName.IsEmpty()) return nullptr;

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	// Try to find the weapon class by name
	UClass* WeaponClass = FindObject<UClass>(ANY_PACKAGE, *WeaponName);
	if (WeaponClass && WeaponClass->IsChildOf(AWeapon::StaticClass()))
	{
		return World->SpawnActor<AWeapon>(WeaponClass);
	}

	// If not found, you might need a lookup table or different approach
	UE_LOG(LogTemp, Warning, TEXT("Could not find weapon class: %s"), *WeaponName);
	return nullptr;
}

UClass* USaveSystemComponent::GetTreasureClassFromName(const FString& ClassName)
{
	if (ClassName.IsEmpty()) return nullptr;

	UClass* FoundClass = nullptr;
	// Try to find the class by name
	for (auto Treasure : TreasureClasses)
	{
		if (Treasure->GetClass()->GetName() == ClassName)
		{
			FoundClass = Treasure->GetClass();
		}
	}	

	if (FoundClass && FoundClass->IsChildOf(AItem::StaticClass()))
	{
		return FoundClass;
	}

	UE_LOG(LogTemp, Warning, TEXT("Could not find item class: %s"), *ClassName);
	return nullptr;
}

UClass* USaveSystemComponent::GetEnemyClassFromName(const FString& ClassName)
{
	if (ClassName.IsEmpty())
	{
		return nullptr;
	}
//UClass* EnemyClass = EnemyData.EnemyClassPath.TryLoadClass<AEnemy>();

	for (TSubclassOf<AEnemy> EnemyClass : EnemyClasses)
	{
		if (EnemyClass && EnemyClass->GetName() == ClassName)
		{
			return EnemyClass;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Could not find Enemy class: %s"), *ClassName);
	return nullptr;
}

