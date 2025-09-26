// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SaveSystemComponent.generated.h"

class UAutoSaveGame;
class AWeapon;
class AEnemy;
class AItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_UDEMYRPG_API USaveSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USaveSystemComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "Save System")
	void LoadGame();

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool DoesSaveExist();

	UFUNCTION(BlueprintCallable, Category = "Save System")
	void DeleteSaveGame();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Internal save methods
	void SavePlayerData(UAutoSaveGame* SaveGame);
	void SaveEnemyData(UAutoSaveGame* SaveGame);
	void SaveTreasureData(UAutoSaveGame* SaveGame);

	// Internal load methods
	void LoadPlayerData(UAutoSaveGame* LoadedGame);
	void LoadEnemyData(UAutoSaveGame* LoadedGame);
	void LoadTreasureData(UAutoSaveGame* LoadedGame);

	// Utility methods
	void ClearEnemiesAndTreasure();
	FString GetWeaponTypeName(AWeapon* Weapon);
	AWeapon* CreateWeaponFromName(const FString& WeaponName);
	UClass* GetTreasureClassFromName(const FString& ClassName);

	// Configurable properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save System", meta = (AllowPrivateAccess = "true"))
	FString SaveSlotName = TEXT("GameSave");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save System", meta = (AllowPrivateAccess = "true"))
	int32 UserIndex = 0;

	// Enemy and Treasure class references for spawning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save System", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemy> DefaultEnemyClass; // Set this in Blueprint

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save System", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AItem>> TreasureClasses; // Set these in Blueprint
};
