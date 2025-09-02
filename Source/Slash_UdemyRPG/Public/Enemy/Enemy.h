// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;
class AWeapon;

UCLASS()
class SLASH_UDEMYRPG_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	//
	//Attacks
	//
	virtual void AttackEnd() override;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> PossibleAttacks;

	//
	//Death
	//
	virtual void Die() override;
	virtual int32 PlayDeathMontage() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void HitReactEnd() override;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

private:
	void InitializeEnemy();

	//
	//Attributes
	//
	void SetHealthBarVisibility(bool Visibility);
	void SetHealthBarPercent();

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

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
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrollingSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float ChasingSpeed = 300.f;

	void StartPatrolling();
	void PatrolTimerFinished();
	void MoveToTarget(AActor* Target);
	bool NewPatrolTarget();
	void CheckPatrolTarget();
	void ChaseTarget();

	FORCEINLINE bool IsOutsideCombatRadius() { return !InTargetRange(CombatTarget, CombatRadius); }
	FORCEINLINE bool IsOutsideAttackRadius() { return !InTargetRange(CombatTarget, AttackRadius); }
	FORCEINLINE bool IsInsideAttackRadius() { return InTargetRange(CombatTarget, AttackRadius); }
	FORCEINLINE bool IsChasing() { return EnemyState == EEnemyState::EES_Chasing; }
	FORCEINLINE bool IsAttacking() { return EnemyState == EEnemyState::EES_Attacking; }

	//
	//AI Combat
	//
	void SpawnDefaultWeapon();
	void CheckCombatTarget();
	bool InTargetRange(AActor* Target, double Radius);
	virtual bool CanAttack() override;
	void Attack();
	void StartAttackTimer();
	void LoseInterest();
	virtual void Destroyed() override;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 2000.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<TSubclassOf<AWeapon>> WeaponClasses;

	UPROPERTY(EditAnywhere, Category = Combat)
	FName WeaponSocket;

	UPROPERTY(EditAnywhere, Category = Combat)
	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	FTimerHandle AttackResetTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackResetTime = 2.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 60.f;

	int32 CurrentAttackIndex = 0;

	float ComboResetTime = 5.f;

	FTimerHandle ComboResetTimerHandle;

	FORCEINLINE void ClearAttackTimer() { GetWorldTimerManager().ClearTimer(AttackTimer); }
	FORCEINLINE bool IsDead() { return EnemyState == EEnemyState::EES_Dead; }
	FORCEINLINE bool IsEngaged() { return EnemyState == EEnemyState::EES_Engaged; }

public:	

};
