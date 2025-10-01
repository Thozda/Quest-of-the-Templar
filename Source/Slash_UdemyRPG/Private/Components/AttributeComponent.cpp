// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

#include "Components/SaveSystemComponent.h"
#include "GameMode/SlashGameMode.h"
#include "Math/UnitConversion.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FString UAttributeComponent::CheckUpgradeRequirements()
{
	if (Souls < NextUpgradeSoulCost)
	{
		return FString::Printf(TEXT("Next Upgrade Available At %d Souls"), NextUpgradeSoulCost);
	}
	else if (Gold < NextUpgradeGoldCost)
	{
		return FString::Printf(TEXT("Next Upgrade Costs %d Gold"), NextUpgradeGoldCost);
	}
	else
	{
		return FString::Printf(TEXT("Interact To Buy Upgrade For %d Gold"), NextUpgradeGoldCost);
	}
}

bool UAttributeComponent::Upgrade()
{
	if (Souls >= NextUpgradeSoulCost && Gold >= NextUpgradeGoldCost)
	{
		ASlashGameMode* GameMode = Cast<ASlashGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode && GameMode->GetSaveSystem()) GameMode->GetSaveSystem()->SaveGame();
		else UE_LOG(LogTemp, Warning, TEXT("Failed to get game mode : Attribute Component"));
		
		Level++;
		Gold -= NextUpgradeGoldCost;
		NextUpgradeGoldCost += 50;
		NextUpgradeSoulCost += 20;
		MaxHealth *= (StatMultiplyer + HealthStatMultiplyerAddition);
		Health = MaxHealth;
		MaxStamina *= StatMultiplyer;
		Stamina = MaxStamina;
		StaminaRegenRate *= StatMultiplyer;
		LightDamageMultiplier *= StatMultiplyer;
		HeavyDamageMultiplier *= StatMultiplyer;
		return true;
	}
	return false;
}

void UAttributeComponent::RevieceDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

void UAttributeComponent::UseStamina(float Quantity)
{
	Stamina = FMath::Clamp(Stamina - Quantity, 0.f, MaxStamina);
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}
