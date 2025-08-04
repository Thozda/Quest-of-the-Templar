// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UNiagaraSystem;
class UAttributeComponent;
class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;

UCLASS()
class SLASH_UDEMYRPG_API AEnemy : public ABaseCharacter, public IHitInterface
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

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	//
	//AI Navigation
	//
	UPROPERTY()
	AAIController* EnemyController;

	//Current Patrol Target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	//Possible Targets
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 150.f;

	int32 PatrolPointIndex = 0;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;

	void PatrolTimerFinished();

	void MoveToTarget(AActor* Target);
	bool NewPatrolTarget();

	void CheckPatrolTarget();

	//
	//AI Combat
	//
	UPROPERTY()
	AActor* CombatTarget;

	bool InTargetRange(AActor* Target, double Radius);

	UPROPERTY(EditAnywhere)
	double CombatRadius = 2000.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 200.f;

	void CheckCombatTarget();

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
