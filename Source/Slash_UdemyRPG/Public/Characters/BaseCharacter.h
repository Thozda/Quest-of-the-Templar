// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class UNiagaraSystem;
class UAttributeComponent;
class AWeapon;

UCLASS()
class SLASH_UDEMYRPG_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;

	//
	//Attack
	//
	virtual bool CanAttack();
	virtual bool BaseAttack(TArray<FName>& PossibleAttacks, int32& CurrentAttackIndex,
		FTimerHandle& ComboResetTimerHandle, float ComboResetTime, TFunction<void()> ResetFunc, bool bIsBoss);
	virtual void LoseInterest();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCanDamage(bool state);

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	virtual void InvulnerableStart();

	UFUNCTION(BlueprintCallable)
	virtual void InvulnerableEnd();

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 75.f;

	//
	//Damage
	//
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	void DisableCapsule();

	UFUNCTION(BlueprintNativeEvent)
	void Die();

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	//
	//Attributes
	//
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	bool IsAlive();

	//
	//Anim Montages
	//
	void StopAttackMontage();
	virtual void PlayDodgeMontage();
	virtual void PlayHitReactMontage(const FName& SelectionName);
	virtual int32 PlayDeathMontage();
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	UFUNCTION(BlueprintCallable)
	virtual void HitReactEnd();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> DeathMontageSections;

	//
	//VFX
	//
	void HitFX(const FVector& ImpactPoint);

private:
	//
	//Anim Montages
	//
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	virtual void PlayAttackMontage(const FName& Selection, bool bIsBoss);
	void ResetAttackSpeed();

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> AttackMontageSections;
	
	FTimerHandle AttackSpeedTimerHandle;
	
	//
	//VFX
	//
	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UNiagaraSystem* NiagaraHitParticles;

	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	int32 AttackIndex;

	FORCEINLINE void ResetAttackIndex(int32& Index) { Index = 0; };
	FORCEINLINE void EnableMeshCollision() { GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); };
	FORCEINLINE void DisableMeshCollision() { GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); };
};
