// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 2000.f;
	PawnSensing->SetPeripheralVisionAngle(30.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}

void AEnemy::InitializeEnemy()
{
	if (PatrolTargets.Num() > 0) PatrolTarget = PatrolTargets[PatrolPointIndex];
	EnemyController = Cast<AAIController>(GetController());

	MoveToTarget(PatrolTarget);
	SetHealthBarVisibility(false);
	SetHealthBarPercent();
	SpawnDefaultWeapon();
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClasses.Num() > 0)
	{
		TSubclassOf<AWeapon> RandomWeapon = WeaponClasses[FMath::RandRange(0, WeaponClasses.Num() - 1)];
		if (!RandomWeapon)
		{
			SpawnDefaultWeapon();
			return;
		}

		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(RandomWeapon);

		DefaultWeapon->Equip(GetMesh(), WeaponSocket, this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius) && !GetWorldTimerManager().IsTimerActive(PatrolTimer))
	{
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, FMath::RandRange(PatrolWaitMin, PatrolWaitMax));
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged()) StartPatrolling();
	}
	else if (!IsChasing() && IsOutsideAttackRadius())
	{
		ClearAttackTimer();
		if (!IsEngaged()) ChaseTarget();
	}
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState == EEnemyState::EES_Patrolling && SeenPawn->ActorHasTag(FName("Knight")))
	{
		CombatTarget = SeenPawn;
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		ChaseTarget();
	}
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::PatrolTimerFinished()
{
	if (NewPatrolTarget())
	{
		MoveToTarget(PatrolTarget);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot Set Patrol Target"))
	}
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(60.f);

	EnemyController->MoveTo(MoveRequest);
}

bool AEnemy::NewPatrolTarget()
{
	if (PatrolTargets.Num() <= 0) return false;

	PatrolPointIndex = (PatrolPointIndex + 1) % PatrolTargets.Num();
	PatrolTarget = PatrolTargets[PatrolPointIndex];
	return true;
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	if (CombatTarget)
	{
		MoveToTarget(CombatTarget);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Combat Target"))
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	SetHealthBarVisibility(false);
}

bool AEnemy::CanAttack()
{
	return IsInsideAttackRadius() && !IsAttacking() && !IsEngaged() && !IsDead();
}

void AEnemy::Attack()
{
	if (BaseAttack(PossibleAttacks, CurrentAttackIndex, ComboResetTimerHandle, ComboResetTime, [this]() { CurrentAttackIndex = 0; } ))
	{
		EnemyState = EEnemyState::EES_Engaged;

		GetWorldTimerManager().SetTimer(AttackResetTimer, this, &AEnemy::AttackEnd, AttackResetTime);
	}
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;

	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
	GetWorldTimerManager().ClearTimer(AttackResetTimer);
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	CombatTarget = EventInstigator->GetPawn();
	EnemyState = EEnemyState::EES_NoState;
	return DamageAmount;
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) SetHealthBarVisibility(true);
	GetWorldTimerManager().ClearTimer(PatrolTimer);
	GetWorldTimerManager().ClearTimer(AttackTimer);
	StopAttackMontage();
	SetWeaponCanDamage(false);
}

void AEnemy::HitReactEnd()
{
	CheckCombatTarget();
}

void AEnemy::Die()
{
	Super::Die();

	EnemyState = EEnemyState::EES_Dead;

	PlayDeathMontage();
	ClearAttackTimer();
	SetHealthBarVisibility(false);
	GetWorldTimerManager().ClearTimer(AttackResetTimer);
	DisableCapsule();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetLifeSpan(DeathLifeSpan);
	SetWeaponCanDamage(false);
}

int32 AEnemy::PlayDeathMontage()
{
	const int32 Selection = Super::PlayDeathMontage();

	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Selection;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::SetHealthBarVisibility(bool Visibility)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(Visibility);
	}
}

void AEnemy::SetHealthBarPercent()
{
	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}
