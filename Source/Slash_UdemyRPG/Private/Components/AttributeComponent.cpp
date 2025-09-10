// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

#include "Math/UnitConversion.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
