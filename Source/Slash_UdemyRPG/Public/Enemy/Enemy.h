// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UNiagaraSystem;
class UAttributeComponent;
class UHealthBarComponent;

UCLASS()
class SLASH_UDEMYRPG_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	void Die();

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	//
	//Play Montage Functions
	//
	void PlayHitReactMontage(const FName& SelectionName);

	//
	//VFX
	//
	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UNiagaraSystem* NiagaraHitParticles;

private:
	//
	//Attributes
	//
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;
	
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	//
	//AI
	//
	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 2000.f;

	//
	//Animation Montages
	//
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	//
	//SFX
	//
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	void HitFX(const FVector& ImpactPoint);

	void DirectionalHitReact(const FVector& ImpactPoint);


public:	

};
