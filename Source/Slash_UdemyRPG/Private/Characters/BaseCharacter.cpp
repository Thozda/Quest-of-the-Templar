// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::BaseAttack(TArray<int32> PossibleAttacks, int32 CurrentAttackIndex, FTimerHandle ComboResetTimerHandle, float ComboResetTime)
{
	if (CanAttack())
	{
		ActionState = EActionState::EAS_Attacking;
		int32 SelectedAttack = PossibleAttacks[CurrentAttackIndex];
		PlayAttackMontage(SelectedAttack);

		CurrentAttackIndex = (CurrentAttackIndex + 1) % PossibleAttacks.Num();

		GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);
		GetWorldTimerManager().SetTimer(ComboResetTimerHandle, this, &AKnight::ResetLightAttackIndex, ComboResetTime, false);
	}
}

void ABaseCharacter::SetWeaponCanDamage(bool state)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->EmptyIgnoreActors();
		EquippedWeapon->SetCanDamage(state);
	}
}
