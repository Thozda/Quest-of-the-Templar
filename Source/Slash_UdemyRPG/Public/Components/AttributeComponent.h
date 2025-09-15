// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_UDEMYRPG_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
	override;

	//
	//Upgrade
	//
	FString CheckUpgradeRequirements();
	bool Upgrade();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Level = 0;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StatMultiplyer = 1.2f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 NextUpgradeGoldCost = 50;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 NextUpgradeSoulCost = 25;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 MinSouls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 MaxSouls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 DodgeCost = 20.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 LightAttackCost = 10.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 HeavyAttackCost = 30.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float LightDamageMultiplier = 0.8;
	
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float HeavyDamageMultiplier = 1.2f;
	
public:
	//
	//Health
	//
	FORCEINLINE void RevieceDamage(float Damage);
	FORCEINLINE bool IsAlive() const { return Health > 0.f; }
	FORCEINLINE float GetHealthPercent() const { return Health / MaxHealth; }

	//
	//Attack
	//
	FORCEINLINE float GetLightDamageMultiplier() const { return LightDamageMultiplier; }
	FORCEINLINE float GetHeavyDamageMultiplier() const { return HeavyDamageMultiplier; }
	
	//
	//Stamina
	//
	FORCEINLINE void UseStamina(float Quantity);
	FORCEINLINE int32 GetStamina() const { return Stamina; }
	FORCEINLINE float GetStaminaPercent() const { return Stamina / MaxStamina; }
	FORCEINLINE void RegenStamina(float DeltaTime);
	FORCEINLINE int32 GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE int32 GetLightAttackCost() const { return LightAttackCost; }
	FORCEINLINE int32 GetHeavyAttackCost() const { return HeavyAttackCost; }

	//
	//Currencies
	//
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE void AddGold(int32 Quantity) { Gold += Quantity; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE int32 GetMinSouls() const { return MinSouls; }
	FORCEINLINE int32 GetMaxSouls() const { return MaxSouls; }
	FORCEINLINE void AddSouls(int32 Quantity) { Souls += Quantity; }
};
