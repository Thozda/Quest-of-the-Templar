// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "TimerManager.h"
#include "Components/AttributeComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABaseCharacter::BaseAttack(TArray<int32>& PossibleAttacks, int32& CurrentAttackIndex, FTimerHandle& ComboResetTimerHandle, float ComboResetTime, TFunction<void()> ResetFunc)
{
	if (CanAttack())
	{
		int32 SelectedAttack = PossibleAttacks[CurrentAttackIndex];
		PlayAttackMontage(SelectedAttack);

		CurrentAttackIndex = (CurrentAttackIndex + 1) % PossibleAttacks.Num();

		GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);

		// Create a dynamic delegate that captures ResetFunc
		FTimerDelegate ResetDelegate;
		ResetDelegate.BindLambda(ResetFunc);
		GetWorldTimerManager().SetTimer(ComboResetTimerHandle, ResetDelegate, ComboResetTime, false);

		return true;
	}

	return false;
}


bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::Die()
{

}

void ABaseCharacter::AttackEnd()
{

}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	if (CrossProduct.Z < 0)
	{
		Theta *= -1;
	}

	FName Section("FromBack");
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}

	PlayHitReactMontage(Section);

	const FVector PushDirection = -ToHit;
	const float PushStrength = 700.f;
	LaunchCharacter(PushDirection * PushStrength, true, true);
}

void ABaseCharacter::PlayAttackMontage(const int32& Selection)
{

}

void ABaseCharacter::PlayHitReactMontage(const FName& SelectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SelectionName, HitReactMontage);
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
