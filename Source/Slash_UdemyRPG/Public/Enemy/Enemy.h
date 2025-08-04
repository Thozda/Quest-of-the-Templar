// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;

UCLASS()
class SLASH_UDEMYRPG_API AEnemy : public ABaseCharacter
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

	virtual void Die() override;

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;


private:
	//
	//Attributes
	//
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
	//SFX
	//
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	void HitFX(const FVector& ImpactPoint);


public:	

};
