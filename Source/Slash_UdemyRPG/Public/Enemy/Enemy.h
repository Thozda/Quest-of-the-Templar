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
class ASoul;

UCLASS()
class SLASH_UDEMYRPG_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	void RestoreTimers(float PatrolRemaining, float AttackRemaining, float AttackResetRemaining, float ComboRemaining);
	void LoadedPatrolling();
	
	//Called by Arena when player enters -> Bosses only
	void LookAtPlayer(const AActor* Player);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	//
	//Attacks
	//
	virtual void AttackEnd() override;
	virtual void LoseInterest() override;

	UFUNCTION(BlueprintCallable)
	void SetSecondaryWeaponCanDamage(bool state);

	UFUNCTION(BlueprintCallable)
	void AttackResetComboExtention();

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> PossibleAttacks;

	//
	//Death
	//
	virtual void Die_Implementation() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void HitReactEnd() override;

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
	//Keys
	//
	void SpawnKey();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Key;

	//
	//Souls
	//
	void SpawnSoul();

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<ASoul> SoulClass;

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
	void InitializeWeapon();
	void CheckCombatTarget();
	bool InTargetRange(AActor* Target, double Radius);
	virtual bool CanAttack() override;
	void Attack();
	void StartAttackTimer();
	virtual void Destroyed() override;

	UPROPERTY(EditAnywhere, Category = Combat)
	double CombatRadius = 2000.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AcceptanceRadius = 60.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<TSubclassOf<AWeapon>> WeaponClasses;

	UPROPERTY(EditAnywhere, Category = Combat)
	AWeapon* SecondaryEquippedWeapon;

	UPROPERTY(EditAnywhere, Category = Combat)
	FName WeaponSocket;

	UPROPERTY(EditAnywhere, Category = Combat)
	FName SecondaryWeaponSocket = FName("Null");

	UPROPERTY()
	FTimerHandle AttackTimer;

	UPROPERTY()
	FTimerHandle AttackResetTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackResetTime = 2.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackResetTimeMin = 2.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackResetTimeMax = 3.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 60.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	bool bIsBoss;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	AActor* BossArena;

	UPROPERTY(VisibleAnywhere, Category = Combat)
	int32 CurrentAttackIndex = 0;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ComboResetTime = 15.f;

	FTimerHandle ComboResetTimerHandle;

	FORCEINLINE void ClearAttackTimer() { GetWorldTimerManager().ClearTimer(AttackTimer); }
	FORCEINLINE void ResetAttackindex() { CurrentAttackIndex = 0; }
	FORCEINLINE bool IsEngaged() const { return EnemyState == EEnemyState::EES_Engaged; }

public:	
	FORCEINLINE bool IsBoss() const { return bIsBoss; }
	FORCEINLINE void SetIsBoss(bool value) { bIsBoss = value; }
	FORCEINLINE AActor* GetBossArena() const { return BossArena; }
	FORCEINLINE void SetBossArena(AActor* Arena) { BossArena = Arena; }
	FORCEINLINE UAttributeComponent* GetAttributes() const { return Attributes; }
	FORCEINLINE AWeapon* GetWeapon() const { return EquippedWeapon; }
	FORCEINLINE AActor* GetPatrolTarget() const { return PatrolTarget; }
	void SetPatrolTarget(FString TargetName);
	FORCEINLINE int32 GetPatrolPointIndex() const { return PatrolPointIndex; }
	FORCEINLINE void SetPatrolPointIndex(int32 NewPatrolPointIndex) { PatrolPointIndex = NewPatrolPointIndex; }
	FORCEINLINE float GetPatrolTimerRemaining() const { return GetWorldTimerManager().GetTimerRemaining(PatrolTimer); }
	FORCEINLINE float GetAttackTimerRemaining() const { return GetWorldTimerManager().GetTimerRemaining(AttackTimer); }
	FORCEINLINE float GetComboResetTimerRemaining() const { return GetWorldTimerManager().GetTimerRemaining(ComboResetTimerHandle); }
	FORCEINLINE float GetAttackResetTimerRemaining() const { return GetWorldTimerManager().
		GetTimerRemaining(AttackResetTimer); }
	FORCEINLINE int32 GetCurrentAttackIndex() const { return CurrentAttackIndex; }
	FORCEINLINE void SetCurrentAttackIndex(int32 NewAttackIndex) { CurrentAttackIndex = NewAttackIndex; }
	FORCEINLINE bool bHasPatrolTarget() { if (PatrolTarget) return true; return false;}
	FORCEINLINE bool IsDead() const { return EnemyState == EEnemyState::EES_Dead; }
	FORCEINLINE bool bIsPatrolling() { return EnemyState == EEnemyState::EES_Patrolling;}
	FORCEINLINE EEnemyState GetEnemyState() const { return EnemyState; }
	FORCEINLINE void SetEnemyState(EEnemyState NewState) { EnemyState = NewState; }
	FORCEINLINE FName GetWeaponSocket() const { return WeaponSocket; }
	FORCEINLINE TArray<AActor*> GetPatrolPoints() const { return PatrolTargets; }
	FORCEINLINE void AddPatrolTarget(AActor* Point) { PatrolTargets.Add(Point); }
};
