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

	UFUNCTION(BlueprintCallable)
	void SetWeaponCanDamage(bool state);

protected:
	virtual void BeginPlay() override;

	//
	//Attack
	//
	virtual bool CanAttack();
	virtual bool BaseAttack(TArray<int32>& PossibleAttacks, int32& CurrentAttackIndex, FTimerHandle& ComboResetTimerHandle, float ComboResetTime, TFunction<void()> ResetFunc);

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	virtual void Die();

	//
	//Damage
	//
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	//
	//Hit
	//
	void DirectionalHitReact(const FVector& ImpactPoint);

	//
	//Attributes
	//
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	//
	//Anim Montages
	//
	virtual void PlayAttackMontage(const int32& Selection);

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	virtual void PlayHitReactMontage(const FName& SelectionName);

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	//
	//VFX
	//
	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UNiagaraSystem* NiagaraHitParticles;

private:
	int32 AttackIndex;
	FORCEINLINE void ResetAttackIndex(int32& Index) { Index = 0; };
};
