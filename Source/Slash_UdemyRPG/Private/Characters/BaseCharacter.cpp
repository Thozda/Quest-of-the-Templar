// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "TimerManager.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "Slash_UdemyRPG/DebugMacros.h"
#include "Kismet/KismetMathLibrary.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABaseCharacter::BaseAttack(TArray<FName>& PossibleAttacks, int32& CurrentAttackIndex,
	FTimerHandle& ComboResetTimerHandle, float ComboResetTime, TFunction<void()> ResetFunc)
{
	if (PossibleAttacks.Num() <= 0) return false;

	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		LoseInterest();
		return false;
	}

	FName SelectedAttack = PossibleAttacks[CurrentAttackIndex];
	PlayAttackMontage(SelectedAttack);

	CurrentAttackIndex = (CurrentAttackIndex + 1) % PossibleAttacks.Num();

	GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);

	// Create a dynamic delegate that captures ResetFunc
	FTimerDelegate ResetDelegate;
	ResetDelegate.BindLambda(ResetFunc);
	GetWorldTimerManager().SetTimer(ComboResetTimerHandle, ResetDelegate, ComboResetTime, false);
	return true;
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

void ABaseCharacter::PlayAttackMontage(const FName& Selection)
{
	if (AttackMontageSections.Num() <= 0) return;

	PlayMontageSection(AttackMontage, Selection);
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

void ABaseCharacter::PlayDodgeMontage()
{
	if (DodgeMontage)
	{
		PlayMontageSection(DodgeMontage, FName("Default"));
	}
}

void ABaseCharacter::DodgeEnd()
{
	
}

void ABaseCharacter::InvulnerableStart()
{
	DisableMeshCollision();
}

void ABaseCharacter::InvulnerableEnd()
{
	EnableMeshCollision();
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();
	
	const FVector CombatTargetLoctaion = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLoctaion).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;

	return CombatTargetLoctaion + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::LoseInterest()
{

}

void ABaseCharacter::HitReactEnd()
{

}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);

	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Selection;
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else Die_Implementation();

	HitFX(ImpactPoint);
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	//UE_LOG(LogTemp, Warning, TEXT("Taking Damage"))
	if (Attributes)
	{
		Attributes->RevieceDamage(DamageAmount);
		//UE_LOG(LogTemp, Warning, TEXT("Damage Amount: %f"), DamageAmount)
	}
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	//DRAW_VECTOR_ARROW(GetActorLocation(), GetActorLocation() + PushDirection.GetSafeNormal() * 100.f);
	const float PushStrength = 700.f;
	LaunchCharacter(PushDirection * PushStrength, true, true);
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

void ABaseCharacter::HitFX(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint,
			UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ImpactPoint));
	}

	if (NiagaraHitParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, NiagaraHitParticles, ImpactPoint,
			UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ImpactPoint));
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

void ABaseCharacter::AttackEnd()
{

}

void ABaseCharacter::Die_Implementation()
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
	DisableMeshCollision();
}
